// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_RENDERWINDOW_HPP
#define NAZARA_VULKANRENDERER_RENDERWINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VkRenderTarget.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceMemory.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Framebuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Image.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Surface.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Swapchain.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VkRenderWindow : public VkRenderTarget, public RenderWindowImpl
	{
		public:
			VkRenderWindow();
			VkRenderWindow(const VkRenderWindow&) = delete;
			VkRenderWindow(VkRenderWindow&&) = delete; ///TODO
			virtual ~VkRenderWindow();

			bool Acquire(UInt32* index, VkSemaphore signalSemaphore = VK_NULL_HANDLE, VkFence signalFence = VK_NULL_HANDLE) const override;

			bool Create(RendererImpl* renderer, RenderSurface* surface, const Vector2ui& size, const RenderWindowParameters& parameters) override;

			inline const Vk::Framebuffer& GetFrameBuffer(UInt32 imageIndex) const override;
			inline UInt32 GetFramebufferCount() const override;
			inline VulkanDevice& GetDevice();
			inline const VulkanDevice& GetDevice() const;
			inline UInt32 GetPresentableFamilyQueue() const;
			inline const Vk::Swapchain& GetSwapchain() const;

			std::shared_ptr<RenderDevice> GetRenderDevice() override;

			void Present(UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE) override;

			VkRenderWindow& operator=(const VkRenderWindow&) = delete;
			VkRenderWindow& operator=(VkRenderWindow&&) = delete; ///TODO

		private:
			bool SetupDepthBuffer(const Vector2ui& size);
			bool SetupRenderPass();
			bool SetupSwapchain(const Vk::PhysicalDevice& deviceInfo, Vk::Surface& surface, const Vector2ui& size);

			Clock m_clock;
			VkColorSpaceKHR m_colorSpace;
			VkFormat m_colorFormat;
			VkFormat m_depthStencilFormat;
			std::shared_ptr<VulkanDevice> m_device;
			std::vector<Vk::Framebuffer> m_frameBuffers;
			Vk::DeviceMemory m_depthBufferMemory;
			Vk::Image m_depthBuffer;
			Vk::ImageView m_depthBufferView;
			Vk::QueueHandle m_presentQueue;
			Vk::Swapchain m_swapchain;
			UInt32 m_presentableFamilyQueue;
	};
}

#include <Nazara/VulkanRenderer/VkRenderWindow.inl>

#endif // NAZARA_VULKANRENDERER_RENDERWINDOW_HPP
