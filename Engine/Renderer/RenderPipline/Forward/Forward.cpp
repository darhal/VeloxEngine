#include "Forward.hpp"

TRE_NS_START

ForwardRenderer::ForwardRenderer()
{
}

void ForwardRenderer::Initialize(uint32 scr_width, uint32 scr_height)
{
	this->SetupCommandBuffer(scr_width, scr_height);
	this->SetupLightsBuffer();
}

void ForwardRenderer::SetupCommandBuffer(uint32 scr_width, uint32 scr_height)
{
	CommandBucket& bucket = m_CommandQueue.CreateBucket();
	bucket.GetProjectionMatrix() = mat4::perspective((float)bucket.GetCamera().Zoom, (float)scr_width / (float)scr_height, NEAR_PLANE, FAR_PLANE);
}

void ForwardRenderer::SetupLightsBuffer()
{

}

void ForwardRenderer::Draw(IPrimitiveMesh& mesh)
{
	// Main Pass:
	mesh.Submit(m_CommandQueue.GetCommandBucker(MAIN_PASS));
}

void ForwardRenderer::Render()
{
	m_CommandQueue.GetCommandBucker(MAIN_PASS).Finalize();
	m_CommandQueue.DispatchCommands();
}

TRE_NS_END
