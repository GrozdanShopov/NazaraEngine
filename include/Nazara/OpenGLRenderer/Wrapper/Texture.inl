// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/PipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline bool PipelineLayout::Create(Device& device, VkDescriptorSetLayout layout, VkPipelineLayoutCreateFlags flags)
		{
			return Create(device, 1U, &layout, flags);
		}

		inline bool PipelineLayout::Create(Device& device, UInt32 layoutCount, const VkDescriptorSetLayout* layouts, VkPipelineLayoutCreateFlags flags)
		{
			VkPipelineLayoutCreateInfo createInfo = {
				VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				nullptr,
				flags,
				layoutCount,
				layouts,
				0U,
				nullptr
			};

			return Create(device, createInfo);
		}

		inline VkResult PipelineLayout::CreateHelper(Device& device, const VkPipelineLayoutCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkPipelineLayout* handle)
		{
			return device.vkCreatePipelineLayout(device, createInfo, allocator, handle);
		}

		inline void PipelineLayout::DestroyHelper(Device& device, VkPipelineLayout handle, const VkAllocationCallbacks* allocator)
		{
			return device.vkDestroyPipelineLayout(device, handle, allocator);
		}
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
