// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanSurface.hpp>
#include <Nazara/VulkanRenderer/Vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include <Nazara/VulkanRenderer/Debug.hpp>

#ifdef VK_USE_PLATFORM_METAL_EXT
#include <objc/runtime.h>
#include <vulkan/vulkan_metal.h>
#endif

namespace Nz
{
	#ifdef VK_USE_PLATFORM_METAL_EXT
	id CreateAndAttachMetalLayer(void* window);
	#endif

	VulkanSurface::VulkanSurface() :
	m_surface(Vulkan::GetInstance())
	{
	}

	bool VulkanSurface::Create(WindowHandle handle)
	{
		bool success = false;
		#if defined(NAZARA_PLATFORM_WINDOWS)
		{
			NazaraAssert(handle.type == WindowBackend::Windows, "expected Windows window");

			HWND winHandle = reinterpret_cast<HWND>(handle.windows.window);
			HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(winHandle, GWLP_HINSTANCE));

			success = m_surface.Create(instance, winHandle);
		}
		#elif defined(NAZARA_PLATFORM_LINUX)
		{
			switch (handle.type)
			{
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
			case WindowBackend::Wayland:
				{
					wl_display* display = static_cast<wl_display*>(handle.wayland.display);
					wl_surface* surface = static_cast<wl_surface*>(handle.wayland.surface);

					success = m_surface.Create(display, surface);
					break;
				}
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
				case WindowBackend::X11:
				{
					Display* display = static_cast<Display*>(handle.x11.display);
					::Window window = static_cast<::Window>(handle.x11.window);

					success = m_surface.Create(display, window);
					break;
				}
#endif

				default:
				{
					NazaraError("unhandled window type");
					return false;
				}
			}
		}
		#elif defined(NAZARA_PLATFORM_MACOS)
		{
			NazaraAssert(handle.type == WindowBackend::Cocoa, "expected cocoa window");
			id layer = CreateAndAttachMetalLayer(handle.cocoa.window);
			success = m_surface.Create(layer);
		}
		#else
		#error This OS is not supported by Vulkan
		#endif

		if (!success)
		{
			NazaraError("Failed to create Vulkan surface: " + TranslateVulkanError(m_surface.GetLastErrorCode()));
			return false;
		}

		return true;
	}

	void VulkanSurface::Destroy()
	{
		m_surface.Destroy();
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
