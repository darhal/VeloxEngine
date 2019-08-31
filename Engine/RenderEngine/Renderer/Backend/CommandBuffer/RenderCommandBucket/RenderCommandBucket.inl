
template<typename T>
RenderCommandBucket<T>::RenderCommandBucket() :
	BaseClass(&RenderCommandBucket<T>::Submit, NUMBER_OF_BUFFERS, AUX_MEMORY),
	m_RenderTargetStack((RenderTarget*) BaseClass::m_KeyPacketPtrAllocator.Allocate(AUX_MEMORY)),
    m_StartLocation(BaseClass::DEFAULT_MAX_ELEMENTS), 
    m_SecondCurrent(BaseClass::DEFAULT_MAX_ELEMENTS), 
	m_RenderTargetCount(0),
    m_LastStateHash(RenderSettings::DEFAULT_STATE_HASH),
    mtx(),
    cv(),
    m_IsReading(false)
{
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
		const uint32 max = m_SecondCurrent;
		const uint32 start = m_StartLocation;

		for (uint32 i = start; i < max; i++) {
			const Pair<Key, uint32>& k = BaseClass::m_Keys[i];
			Key key = k.first;
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
    if (m_StartLocation) {
        BaseClass::m_Current = 0;
    }else{
        BaseClass::m_Current = BaseClass::DEFAULT_MAX_ELEMENTS;
    }
    
    // printf("(WRITE THREAD) CLEAR : CURRENT FOR WRITE = %d\n", BaseClass::m_Current);
    BaseClass::m_CmdAllocator.SetOffset(BaseClass::m_Current * BaseClass::m_Current);
    BaseClass::m_PacketCount = 0;
}

template<typename T>
bool RenderCommandBucket<T>::SwapCmdBuffer()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return !m_IsReading;});

    m_SecondCurrent = BaseClass::m_Current;
    
    if (m_StartLocation){
        m_StartLocation = 0;
    }else{
        m_StartLocation = BaseClass::DEFAULT_MAX_ELEMENTS;
    }

    // printf("(WRITE THREAD) SWAP :READ WILL START FROM Start = %d | EnD =%d\n", m_StartLocation, m_SecondCurrent);

    m_IsReading = 1;
    // lk.unlock();
    return true;
}

template<typename T>
void RenderCommandBucket<T>::PushRenderTarget(const RenderTarget& render_target)
{
	memcpy(m_RenderTargetStack + m_RenderTargetCount, &render_target, sizeof(RenderTarget));
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