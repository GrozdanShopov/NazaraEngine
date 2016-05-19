// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKCOMMANDPOOL_HPP
#define NAZARA_VULKAN_VKCOMMANDPOOL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class CommandBuffer;
		class CommandPool;

		using CommandPoolHandle = ObjectHandle<CommandPool>;

		class NAZARA_VULKAN_API CommandPool : public DeviceObject<CommandPool, VkCommandPool, VkCommandPoolCreateInfo>, public HandledObject<CommandPool>
		{
			friend DeviceObject;

			public:
				inline CommandPool(Device& instance);
				CommandPool(const CommandPool&) = delete;
				CommandPool(CommandPool&&) = default;
				~CommandPool() = default;

				CommandBuffer AllocateCommandBuffer(VkCommandBufferLevel level);
				std::vector<CommandBuffer> AllocateCommandBuffers(UInt32 commandBufferCount, VkCommandBufferLevel level);

				using DeviceObject::Create;
				inline bool Create(UInt32 queueFamilyIndex, VkCommandPoolCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

				inline bool Reset(VkCommandPoolResetFlags flags);

				CommandPool& operator=(const CommandPool&) = delete;
				CommandPool& operator=(CommandPool&&) = delete;

			private:
				static inline VkResult CreateHelper(Device& device, const VkCommandPoolCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkCommandPool* handle);
				static inline void DestroyHelper(Device& device, VkCommandPool handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/Vulkan/VkCommandPool.inl>

#endif // NAZARA_VULKAN_VKCOMMANDPOOL_HPP
