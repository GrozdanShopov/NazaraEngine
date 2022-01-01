// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

#if !defined(NAZARA_VULKANRENDERER_DEVICE_FUNCTION) || !defined(NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION)
#error You must define NAZARA_VULKANRENDERER_DEVICE_FUNCTION and NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION before including this file
#endif

#ifndef NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN
#define NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(ext)
#endif

#ifndef NAZARA_VULKANRENDERER_DEVICE_EXT_END
#define NAZARA_VULKANRENDERER_DEVICE_EXT_END()
#endif

#ifndef NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN(ext)
#endif

#ifndef NAZARA_VULKANRENDERER_INSTANCE_EXT_END
#define NAZARA_VULKANRENDERER_INSTANCE_EXT_END()
#endif

// Vulkan core
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkAllocateCommandBuffers)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkAllocateDescriptorSets)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkAllocateMemory)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkBeginCommandBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkBindBufferMemory)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkBindImageMemory)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBeginQuery)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBeginRenderPass)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBindDescriptorSets)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBindIndexBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBindPipeline)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBindVertexBuffers)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBlitImage)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdClearAttachments)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdClearColorImage)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdClearDepthStencilImage)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyBufferToImage)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyImage)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyImageToBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyQueryPoolResults)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDispatch)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDispatchIndirect)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDraw)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDrawIndexed)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDrawIndexedIndirect)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDrawIndirect)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdEndQuery)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdEndRenderPass)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdExecuteCommands)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdFillBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdNextSubpass)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdPipelineBarrier)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdPushConstants)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdResetEvent)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdResetQueryPool)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdResolveImage)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetBlendConstants)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetDepthBias)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetDepthBounds)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetEvent)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetLineWidth)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetScissor)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetStencilCompareMask)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetStencilReference)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetStencilWriteMask)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetViewport)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdUpdateBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdWaitEvents)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdWriteTimestamp)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateBufferView)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateCommandPool)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateComputePipelines)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateDescriptorPool)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateDescriptorSetLayout)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateEvent)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateFence)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateFramebuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateGraphicsPipelines)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateImage)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateImageView)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreatePipelineCache)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreatePipelineLayout)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateRenderPass)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateSampler)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateSemaphore)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateShaderModule)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyBufferView)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyCommandPool)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyDescriptorPool)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyDevice)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyEvent)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyFence)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyFramebuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyImage)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyImageView)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyPipeline)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyPipelineCache)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyPipelineLayout)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyRenderPass)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroySampler)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroySemaphore)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyShaderModule)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDeviceWaitIdle)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkEndCommandBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkFreeCommandBuffers)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkFreeDescriptorSets)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkFreeMemory)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkFlushMappedMemoryRanges)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetBufferMemoryRequirements)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetDeviceMemoryCommitment)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetDeviceQueue)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetEventStatus)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetFenceStatus)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetImageMemoryRequirements)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetImageSparseMemoryRequirements)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetImageSubresourceLayout)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetRenderAreaGranularity)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkInvalidateMappedMemoryRanges)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkMapMemory)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkMergePipelineCaches)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueueSubmit)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueueWaitIdle)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetCommandBuffer)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetCommandPool)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetDescriptorPool)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetFences)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetEvent)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkSetEvent)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkUnmapMemory)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkUpdateDescriptorSets)
NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkWaitForFences)

NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(vkBindBufferMemory2, VK_API_VERSION_1_1, KHR, bind_memory2)
NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(vkBindImageMemory2, VK_API_VERSION_1_1, KHR, bind_memory2)

NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(vkGetBufferMemoryRequirements2, VK_API_VERSION_1_1, KHR, get_memory_requirements2)
NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION(vkGetImageMemoryRequirements2, VK_API_VERSION_1_1, KHR, get_memory_requirements2)

NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(VK_KHR_display_swapchain)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateSharedSwapchainsKHR)
NAZARA_VULKANRENDERER_DEVICE_EXT_END()

NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(VK_KHR_surface)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroySurfaceKHR)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
NAZARA_VULKANRENDERER_DEVICE_EXT_END()

NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN(VK_KHR_swapchain)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkAcquireNextImageKHR)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateSwapchainKHR)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroySwapchainKHR)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetSwapchainImagesKHR)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueuePresentKHR)
NAZARA_VULKANRENDERER_DEVICE_EXT_END()

NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN(VK_EXT_debug_utils)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBeginDebugUtilsLabelEXT)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdEndDebugUtilsLabelEXT)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdInsertDebugUtilsLabelEXT)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueueBeginDebugUtilsLabelEXT)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueueEndDebugUtilsLabelEXT)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueueInsertDebugUtilsLabelEXT)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkSetDebugUtilsObjectNameEXT)
	NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkSetDebugUtilsObjectTagEXT)
NAZARA_VULKANRENDERER_INSTANCE_EXT_END()

#undef NAZARA_VULKANRENDERER_DEVICE_CORE_EXT_FUNCTION
#undef NAZARA_VULKANRENDERER_DEVICE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_DEVICE_EXT_END
#undef NAZARA_VULKANRENDERER_DEVICE_FUNCTION
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_BEGIN
#undef NAZARA_VULKANRENDERER_INSTANCE_EXT_END
