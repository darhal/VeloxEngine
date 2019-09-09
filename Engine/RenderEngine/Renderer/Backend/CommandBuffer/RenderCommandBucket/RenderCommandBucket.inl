
template<typename T>
RenderCommandBucket<T>::RenderCommandBucket() :
	BaseClass(&RenderCommandBucket<T>::Submit, NUMBER_OF_BUFFERS, AUX_MEMORY, CMDS_PER_RENDER_TARGETS_SIZE),
	m_RenderTargetStack(NULL),
	m_SecondCurrent(NULL),
	m_ReadWriteKeysCountOffset(),
    m_StartLocation(BaseClass::DEFAULT_MAX_ELEMENTS), 
	m_RenderTargetCount(0),
    m_LastStateHash(RenderSettings::DEFAULT_STATE_HASH),
    mtx(),
    cv(),
    m_IsReading(false)
{
	m_ReadWriteKeysCountOffset[READ_BUFFER] = NUMBER_OF_RT;
	m_ReadWriteKeysCountOffset[WRITE_BUFFER] = 0;
	m_SecondCurrent = (uint32*) BaseClass::m_KeyPacketPtrAllocator.Allocate(CMDS_COUNT_PRE_RT_SIZE);
	m_RenderTargetStack = (RenderTarget*) BaseClass::m_KeyPacketPtrAllocator.Allocate(RENDER_TARGET_SIZE);
	
	RenderTarget render_t;
	this->PushRenderTarget(render_t);
    // cv.notify_one();
}

template<typename T>
template<typename U>
U* RenderCommandBucket<T>::CreateCommand(ShaderID shaderID, VaoID vaoID, MaterialID matID, uint32 blend_dist, usize aux_memory)
{
	return BaseClass::template AddCommand<U>(
		this->GenerateKey(shaderID, vaoID, matID, blend_dist),
		aux_memory
	);
}

template<typename T>
template<typename U>
U* RenderCommandBucket<T>::AddCommandInAllRenderTargets(Key key, usize aux_memory)
{
	CmdPacket packet = CommandPacket::Create<U>(BaseClass::m_CmdAllocator, aux_memory);

	{
		const uint32 current = BaseClass::m_Current;
		const uint32& rt_key_offset = m_ReadWriteKeysCountOffset[WRITE_BUFFER];
		BaseClass::m_Packets[current] = packet;

		for (uint32 i = 0; i < m_RenderTargetCount; i++) {
			uint32& tr_count = m_SecondCurrent[i + rt_key_offset];
			new (BaseClass::m_Keys + tr_count) Pair<Key, uint32>(key, current); // the bug might be here!
			// printf("[Adding command] m_SecondCurrent Index = %d | At Keys Index = %d (Key = %llu | Value = %d) [BASECLASS::CURRENT = %d]\n", i + rt_key_offset, tr_count, key, current, BaseClass::m_Current);
			tr_count++;
		}

		BaseClass::m_Current++;
		BaseClass::m_PacketCount++;
	}

	CommandPacket::StoreNextCommandPacket(packet, NULL);
	CommandPacket::StoreBackendDispatchFunction(packet, U::DISPATCH_FUNCTION);
	return CommandPacket::GetCommand<U>(packet);
}

template<typename T>
typename RenderCommandBucket<T>::Key RenderCommandBucket<T>::GenerateKey(ShaderID shaderID, VaoID vaoID, MaterialID matID, uint32 blend_dist) const
{
    Key key = 0;

    RMI<ShaderProgram>::Index sid = ResourcesManager::GetGRM().GetResourceContainer<ShaderProgram>().CompressID(shaderID);
    RMI<VAO>::Index vao_id  = ResourcesManager::GetGRM().GetResourceContainer<VAO>().CompressID(vaoID);
    RMI<Material>::Index mat_id = ResourcesManager::GetGRM().GetResourceContainer<Material>().CompressID(matID);

    key = 
        (Key(blend_dist) << (sizeof(mat_id) + sizeof(vao_id) + sizeof(sid)) * BITS_PER_BYTE) | 
        (Key(sid)        << (sizeof(mat_id) + sizeof(vao_id)) * BITS_PER_BYTE) | 
        (Key(vao_id)     << (sizeof(mat_id) * BITS_PER_BYTE)) | 
        Key(mat_id);

    return key;
}

template<typename T>
bool RenderCommandBucket<T>::DecodeKey(Key key, ShaderID& shaderID, VaoID& vaoID, MaterialID& matID) const
{
    bool is_blend = key >> (sizeof(Key) * BITS_PER_BYTE - 1);
    
    shaderID = ResourcesManager::GetGRM().GetResourceContainer<ShaderProgram>().CompressID(
        RMI<ShaderProgram>::ID(key >> (sizeof(RMI<VAO>::Index) + sizeof(RMI<Material>::Index)) * BITS_PER_BYTE));

    vaoID = ResourcesManager::GetGRM().GetResourceContainer<VAO>().CompressID(
        RMI<VAO>::ID(key >> (sizeof(RMI<Material>::Index) * BITS_PER_BYTE)));

    matID = ResourcesManager::GetGRM().GetResourceContainer<Material>().CompressID(
        RMI<Material>::ID(key));

    return is_blend;
}

template<typename T>
void RenderCommandBucket<T>::Submit()
{
	for (FboID current_target = 0; current_target < m_RenderTargetCount; current_target++) {

		const uint32& rt_key_offset = m_ReadWriteKeysCountOffset[READ_BUFFER];
		const RenderTarget& render_target = m_RenderTargetStack[current_target];
		const FBO& fbo = ResourcesManager::GetGRM().Get<FBO>(render_target.m_FboID);
		glViewport(0, 0, render_target.m_Width, render_target.m_Height);
		//printf("(Render*) Framebuffer id = %d\n", render_target.m_FboID);
		fbo.Use();
		ClearColor({ 51.f, 76.5f, 76.5f, 255.f });
		ClearBuffers();

		Mat4f pv;
		if (render_target.m_Projection && render_target.m_View) {
			pv = (*render_target.m_Projection) * (*render_target.m_View);
		}
		
		Key lastKey = -1;
		MaterialID lastMatID = -1;
		VaoID lastVaoID = -1;
		ShaderID lastShaderID = -1;
		ShaderProgram* lastShader = NULL;
		const uint32 max = m_SecondCurrent[rt_key_offset + current_target];
		const uint32 start = m_StartLocation + current_target * RT_KEYS_STRIDE;
		//printf("[Render Target : %d] Start = %d | End [INDEX : %d] = %d\n", current_target, start, rt_key_offset + current_target, max);

		for (uint32 i = start; i < max; i++) {
			const Pair<Key, uint32>& k = BaseClass::m_Keys[i];
			Key key = k.first;
			//printf("\t[RENDER] Key Index = %d - Key = %d - Cmd Index = %d\n", i, key, k.second);
			CmdPacket packet = BaseClass::m_Packets[k.second];

			if (key != lastKey) {
				Commands::BasicDrawCommand* command = reinterpret_cast<Commands::BasicDrawCommand*>(const_cast<void*>(CommandPacket::LoadCommand(packet)));

				MaterialID matID;
				VaoID vaoID;
				ShaderID shaderID;
				this->DecodeKey(key, shaderID, vaoID, matID);

				if (shaderID != lastShaderID) {
					lastShader = &ResourcesManager::GetGRM().Get<ShaderProgram>(shaderID);
					lastShader->Bind();
					// lastShader->SetVec3("viewPos", scene.GetCurrentCamera().Position);
					lastShaderID = shaderID;
				}

				if (vaoID != lastVaoID) {
					VAO& vao = ResourcesManager::GetGRM().Get<VAO>(vaoID);
					vao.Bind();
					lastVaoID = vaoID;
				}

				if (matID != lastMatID) {
					Material& material = ResourcesManager::GetGRM().Get<Material>(matID);

					StateGroup& state_grp = material.GetRenderStates();
					StateHash stateHash = state_grp.GetHash();
					if (stateHash != m_LastStateHash) {
						state_grp.ApplyStates();
						m_LastStateHash = stateHash;
					}

					if (!lastShader) {
						lastShader = &ResourcesManager::GetGRM().Get<ShaderProgram>(material.GetTechnique().GetShaderID());
						lastShader->Bind();
					}
	
					lastShader->SetMat4("MVP", pv * *(command->model));
					lastShader->SetMat4("model", *command->model);
					material.GetTechnique().UploadUnfiroms(*lastShader);
					lastMatID = matID;
				}
			}

			do {
				this->SubmitPacket(packet);
				packet = CommandPacket::LoadNextCommandPacket(packet);
			} while (packet != NULL);
		}
	}

    m_IsReading = 0;
    cv.notify_one();
}

template<typename T>
void RenderCommandBucket<T>::Clear()
{
	const uint32& rt_key_offset = m_ReadWriteKeysCountOffset[WRITE_BUFFER];

    if (m_StartLocation) {
        BaseClass::m_Current = 0;
    }else{
        BaseClass::m_Current = BaseClass::DEFAULT_MAX_ELEMENTS;
    }
	
	for (uint32 i = 0; i < m_RenderTargetCount; i++) {
		m_SecondCurrent[i + rt_key_offset] = BaseClass::m_Current + i * RT_KEYS_STRIDE;
		//printf("[CLEAR] m_SecondCurrent Index = %d | Amount = %d\n", i + rt_key_offset, BaseClass::m_Current + i * RT_KEYS_STRIDE);
	}
    
    // printf("(WRITE THREAD) CLEAR : CURRENT FOR WRITE = %d\n", BaseClass::m_Current);
    BaseClass::m_CmdAllocator.SetOffset(BaseClass::m_Current * BaseClass::m_Current);
    BaseClass::m_PacketCount = 0;

	//Dump();
}

template<typename T>
bool RenderCommandBucket<T>::SwapCmdBuffer()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return !m_IsReading;});

	//m_SecondCurrent[offset] = BaseClass::m_Current;
	const uint32 read_buffer = m_ReadWriteKeysCountOffset[READ_BUFFER];
	m_ReadWriteKeysCountOffset[READ_BUFFER] = m_ReadWriteKeysCountOffset[WRITE_BUFFER];
	m_ReadWriteKeysCountOffset[WRITE_BUFFER] = read_buffer;

    if (m_StartLocation){
        m_StartLocation = 0;
    }else{
        m_StartLocation = BaseClass::DEFAULT_MAX_ELEMENTS;
    }

	//Dump();
	//printf("[SWAP] Swapping...\n");
    m_IsReading = 1;
    lk.unlock();
    return true;
}

template<typename T>
void RenderCommandBucket<T>::PushRenderTarget(const RenderTarget& render_target)
{
	memcpy(m_RenderTargetStack + m_RenderTargetCount, &render_target, sizeof(RenderTarget));

	m_SecondCurrent[m_RenderTargetCount + m_ReadWriteKeysCountOffset[READ_BUFFER]] = BaseClass::DEFAULT_MAX_ELEMENTS + m_RenderTargetCount * RT_KEYS_STRIDE;
	m_SecondCurrent[m_RenderTargetCount + m_ReadWriteKeysCountOffset[WRITE_BUFFER]] = m_RenderTargetCount * RT_KEYS_STRIDE;
	//printf("[PUSH RENDER TARGET] m_SecondCurrent Index = %d | Amount = %d\n", m_RenderTargetCount + m_ReadWriteKeysCountOffset[READ_BUFFER], BaseClass::DEFAULT_MAX_ELEMENTS + m_RenderTargetCount * RT_KEYS_STRIDE);
	//printf("[PUSH RENDER TARGET] m_SecondCurrent Index = %d | Amount = %d\n", m_RenderTargetCount + m_ReadWriteKeysCountOffset[WRITE_BUFFER], m_RenderTargetCount * RT_KEYS_STRIDE);
	m_RenderTargetCount++;
}

template<typename T>
void RenderCommandBucket<T>::PopRenderTarget()
{
	if (m_RenderTargetCount)
		m_RenderTargetCount--;
}

template<typename T>
RenderTarget* RenderCommandBucket<T>::GetRenderTarget(uint32 index)
{
	if (index < m_RenderTargetCount) {
		return &m_RenderTargetStack[index];
	}

	return NULL;
}

template<typename T>
void RenderCommandBucket<T>::Dump()
{
	const uint32& rt_key_offset = m_ReadWriteKeysCountOffset[READ_BUFFER];
	printf("******************************************************************\n");

	for (FboID current_target = 0; current_target < m_RenderTargetCount; current_target++) {
		const uint32 max = m_SecondCurrent[rt_key_offset + current_target];
		const uint32 start = m_StartLocation + current_target * RT_KEYS_STRIDE;
		printf("\t*** (DUMP)[Render Target : %d] Start = %d | End [INDEX : %d] = %d ***\n", current_target, start, rt_key_offset + current_target, max);
		for (uint32 i = start; i < max; i++) {
			const Pair<Key, uint32>& k = BaseClass::m_Keys[i];
			Key key = k.first;
			printf("\t\t* (DUMP)[RENDER] Key Index = %d - Key = %d - Cmd Index = %d *\n", i, key, k.second);
		}
	}
	printf("******************************************************************\n");
}