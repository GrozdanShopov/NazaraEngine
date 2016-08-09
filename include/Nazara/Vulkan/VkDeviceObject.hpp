// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKDEVICEOBJECT_HPP
#define NAZARA_VULKAN_VKDEVICEOBJECT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDevice.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		template<typename C, typename VkType, typename CreateInfo>
		class DeviceObject
		{
			public:
				inline DeviceObject(Device& instance);
				DeviceObject(const DeviceObject&) = delete;
				DeviceObject(DeviceObject&&);
				inline ~DeviceObject();

				inline bool Create(const CreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline Device& GetDevice();
				inline const Device& GetDevice() const;
				inline VkResult GetLastErrorCode() const;

				DeviceObject& operator=(const DeviceObject&) = delete;
				DeviceObject& operator=(DeviceObject&&) = delete;

				inline operator VkType();

			protected:
				Device& m_device;
				VkAllocationCallbacks m_allocator;
				VkType m_handle;
				VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/Vulkan/VkDeviceObject.inl>

#endif // NAZARA_VULKAN_VKDEVICEOBJECT_HPP
