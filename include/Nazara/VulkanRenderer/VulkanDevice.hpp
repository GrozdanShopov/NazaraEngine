// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANDEVICE_HPP
#define NAZARA_VULKANRENDERER_VULKANDEVICE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanDevice : public RenderDevice, public Vk::Device
	{
		public:
			using Device::Device;
			VulkanDevice(const VulkanDevice&) = delete;
			VulkanDevice(VulkanDevice&&) = delete; ///TODO?
			~VulkanDevice();

			std::shared_ptr<AbstractBuffer> InstantiateBuffer(BufferType type) override;
			std::shared_ptr<CommandPool> InstantiateCommandPool(QueueType queueType) override;
			std::shared_ptr<RenderPipeline> InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo) override;
			std::shared_ptr<RenderPipelineLayout> InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo) override;
			std::shared_ptr<ShaderStageImpl> InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize) override;
			std::shared_ptr<Texture> InstantiateTexture(const TextureInfo& params) override;
			std::shared_ptr<TextureSampler> InstantiateTextureSampler(const TextureSamplerInfo& params) override;

			VulkanDevice& operator=(const VulkanDevice&) = delete;
			VulkanDevice& operator=(VulkanDevice&&) = delete; ///TODO?
	};
}

#include <Nazara/VulkanRenderer/VulkanDevice.inl>

#endif // NAZARA_VULKANRENDERER_VULKANDEVICE_HPP
