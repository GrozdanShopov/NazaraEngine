// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanMultipleFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanSingleFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/VulkanUploadPool.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	void VulkanCommandBufferBuilder::BeginDebugRegion(const std::string_view& regionName, const Nz::Color& color)
	{
		// Ensure \0 at the end of string
		StackArray<char> regionNameEOS = NazaraStackArrayNoInit(char, regionName.size() + 1);
		std::memcpy(regionNameEOS.data(), regionName.data(), regionName.size());
		regionNameEOS[regionName.size()] = '\0';

		m_commandBuffer.BeginDebugRegion(regionNameEOS.data(), color);
	}

	void VulkanCommandBufferBuilder::BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, Nz::Recti renderRect, std::initializer_list<ClearValues> clearValues)
	{
		const VulkanRenderPass& vkRenderPass = static_cast<const VulkanRenderPass&>(renderPass);

		const Vk::Framebuffer& vkFramebuffer = [&] () -> const Vk::Framebuffer&
		{
			const VulkanFramebuffer& vkFramebuffer = static_cast<const VulkanFramebuffer&>(framebuffer);
			switch (vkFramebuffer.GetType())
			{
				case VulkanFramebuffer::Type::Multiple:
				{
					const VulkanMultipleFramebuffer& vkMultipleFramebuffer = static_cast<const VulkanMultipleFramebuffer&>(vkFramebuffer);
					m_framebufferCount = std::max(m_framebufferCount, vkMultipleFramebuffer.GetFramebufferCount());
					return vkMultipleFramebuffer.GetFramebuffer(m_imageIndex);
				}

				case VulkanFramebuffer::Type::Single:
					return static_cast<const VulkanSingleFramebuffer&>(vkFramebuffer).GetFramebuffer();
			}

			throw std::runtime_error("Unhandled framebuffer type " + std::to_string(UnderlyingCast(vkFramebuffer.GetType())));
		}();

		StackArray<VkClearValue> vkClearValues = NazaraStackArray(VkClearValue, clearValues.size());

		std::size_t index = 0;
		for (const ClearValues& values : clearValues)
		{
			auto& vkValues = vkClearValues[index];

			if (PixelFormatInfo::GetContent(vkRenderPass.GetAttachmentFormat(index)) == PixelFormatContent_ColorRGBA)
			{
				vkValues.color.float32[0] = values.color.r / 255.f;
				vkValues.color.float32[1] = values.color.g / 255.f;
				vkValues.color.float32[2] = values.color.b / 255.f;
				vkValues.color.float32[3] = values.color.a / 255.f;
			}
			else
			{
				vkValues.depthStencil.depth = values.depth;
				vkValues.depthStencil.stencil = values.stencil;
			}

			index++;
		}

		VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		beginInfo.renderPass = vkRenderPass.GetRenderPass();
		beginInfo.framebuffer = vkFramebuffer;
		beginInfo.renderArea.offset.x = renderRect.x;
		beginInfo.renderArea.offset.y = renderRect.y;
		beginInfo.renderArea.extent.width = renderRect.width;
		beginInfo.renderArea.extent.height = renderRect.height;
		beginInfo.clearValueCount = vkClearValues.size();
		beginInfo.pClearValues = vkClearValues.data();

		m_commandBuffer.BeginRenderPass(beginInfo);

		m_currentRenderPass = &vkRenderPass;
	}

	void VulkanCommandBufferBuilder::BindIndexBuffer(Nz::AbstractBuffer* indexBuffer, UInt64 offset)
	{
		VulkanBuffer& vkBuffer = *static_cast<VulkanBuffer*>(indexBuffer);

		m_commandBuffer.BindIndexBuffer(vkBuffer.GetBuffer(), offset, VK_INDEX_TYPE_UINT16); //< Fuck me right?
	}

	void VulkanCommandBufferBuilder::BindPipeline(const RenderPipeline& pipeline)
	{
		if (!m_currentRenderPass)
			throw std::runtime_error("BindPipeline must be called in a RenderPass");

		const VulkanRenderPipeline& vkBinding = static_cast<const VulkanRenderPipeline&>(pipeline);

		m_commandBuffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, vkBinding.Get(m_currentRenderPass->GetRenderPass()));
	}

	void VulkanCommandBufferBuilder::BindShaderBinding(const ShaderBinding& binding)
	{
		const VulkanShaderBinding& vkBinding = static_cast<const VulkanShaderBinding&>(binding);

		const VulkanRenderPipelineLayout& pipelineLayout = vkBinding.GetOwner();

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.GetPipelineLayout(), 0U, vkBinding.GetDescriptorSet());
	}

	void VulkanCommandBufferBuilder::BindVertexBuffer(UInt32 binding, Nz::AbstractBuffer* vertexBuffer, UInt64 offset)
	{
		VulkanBuffer& vkBuffer = *static_cast<VulkanBuffer*>(vertexBuffer);

		m_commandBuffer.BindVertexBuffer(binding, vkBuffer.GetBuffer(), offset);
	}

	void VulkanCommandBufferBuilder::CopyBuffer(const RenderBufferView& source, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		VulkanBuffer& sourceBuffer = *static_cast<VulkanBuffer*>(source.GetBuffer());
		VulkanBuffer& targetBuffer = *static_cast<VulkanBuffer*>(target.GetBuffer());

		m_commandBuffer.CopyBuffer(sourceBuffer.GetBuffer(), targetBuffer.GetBuffer(), size, sourceOffset + source.GetOffset(), targetOffset + target.GetOffset());
	}

	void VulkanCommandBufferBuilder::CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		const auto& vkAllocation = static_cast<const VulkanUploadPool::VulkanAllocation&>(allocation);
		VulkanBuffer& targetBuffer = *static_cast<VulkanBuffer*>(target.GetBuffer());

		m_commandBuffer.CopyBuffer(vkAllocation.buffer, targetBuffer.GetBuffer(), size, vkAllocation.offset + sourceOffset, target.GetOffset() + targetOffset);
	}

	void VulkanCommandBufferBuilder::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		m_commandBuffer.Draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void VulkanCommandBufferBuilder::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		m_commandBuffer.DrawIndexed(indexCount, instanceCount, firstVertex, 0, firstInstance);
	}

	void VulkanCommandBufferBuilder::EndDebugRegion()
	{
		m_commandBuffer.EndDebugRegion();
	}

	void VulkanCommandBufferBuilder::EndRenderPass()
	{
		m_commandBuffer.EndRenderPass();
		m_currentRenderPass = nullptr;
	}

	void VulkanCommandBufferBuilder::PreTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0U, VK_ACCESS_TRANSFER_READ_BIT);
	}

	void VulkanCommandBufferBuilder::PostTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_UNIFORM_READ_BIT);
	}

	void VulkanCommandBufferBuilder::SetScissor(Nz::Recti scissorRegion)
	{
		m_commandBuffer.SetScissor(scissorRegion);
	}

	void VulkanCommandBufferBuilder::SetViewport(Nz::Recti viewportRegion)
	{
		m_commandBuffer.SetViewport(Nz::Rectf(viewportRegion), 0.f, 1.f);
	}
}
