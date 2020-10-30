// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline const RenderPipelineInfo& VulkanRenderPipeline::GetPipelineInfo() const
	{
		return m_pipelineInfo;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
