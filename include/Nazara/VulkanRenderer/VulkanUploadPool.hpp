// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANUPLOADPOOL_HPP
#define NAZARA_VULKANRENDERER_VULKANUPLOADPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Buffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceMemory.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanUploadPool
	{
		public:
			struct AllocationData;

			inline VulkanUploadPool(Vk::Device& device, UInt64 blockSize);
			VulkanUploadPool(const VulkanUploadPool&) = delete;
			VulkanUploadPool(VulkanUploadPool&&) noexcept = default;
			~VulkanUploadPool() = default;

			std::optional<AllocationData> Allocate(UInt64 size);
			std::optional<AllocationData> Allocate(UInt64 size, UInt64 alignment);

			void Reset();

			struct AllocationData
			{
				VkBuffer buffer;
				void* mappedPtr;
				UInt64 offset;
				UInt64 size;
			};

			VulkanUploadPool& operator=(const VulkanUploadPool&) = delete;
			VulkanUploadPool& operator=(VulkanUploadPool&&) = delete;

		private:
			struct Block
			{
				Vk::DeviceMemory blockMemory;
				Vk::Buffer buffer;
				UInt64 freeOffset;
			};

			UInt64 m_blockSize;
			Vk::Device& m_device;
			std::vector<Block> m_blocks;
	};
}

#include <Nazara/VulkanRenderer/VulkanUploadPool.inl>

#endif // NAZARA_VULKANRENDERER_VULKANUPLOADPOOL_HPP
