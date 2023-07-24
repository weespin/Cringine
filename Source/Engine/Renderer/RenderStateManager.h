#pragma once
#include <d3d12.h>
#include <map>
#include <string>

class RenderStateManager
{
public:
	void Init(ID3D12Device* device);

	HRESULT CreatePipelineState(const std::string& name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

	ID3D12PipelineState* GetPipelineState(const std::string& name);

	const std::map<std::string, ID3D12PipelineState*>& GetPipelineStates() const;

private:
	ID3D12Device* m_device;
	std::map<std::string, ID3D12PipelineState*> m_pipelineStates = {};
};
