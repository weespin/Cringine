#include "RenderStateManager.h"

void RenderStateManager::Init(ID3D12Device* device)
{
	m_device = device;
}

HRESULT RenderStateManager::CreatePipelineState(const std::string& name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
{
	ID3D12PipelineState* pso;
	HRESULT hr = m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));

	if (SUCCEEDED(hr))
	{
		m_pipelineStates[name] = pso;
	}

	return hr;
}

ID3D12PipelineState* RenderStateManager::GetPipelineState(const std::string& name)
{
	auto it = m_pipelineStates.find(name);
	if (it != m_pipelineStates.end())
	{
		return it->second;
	}

	return nullptr;
}

const std::map<std::string, ID3D12PipelineState*>& RenderStateManager::GetPipelineStates() const
{
	return m_pipelineStates;
}
