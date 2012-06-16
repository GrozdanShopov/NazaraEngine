// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzRenderWindow::NzRenderWindow() :
m_context(nullptr)
{
}

NzRenderWindow::NzRenderWindow(NzVideoMode mode, const NzString& title, nzUInt32 style, const NzContextParameters& parameters) :
m_context(nullptr)
{
	Create(mode, title, style, parameters);

	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Failed to create render window");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzRenderWindow::NzRenderWindow(NzWindowHandle handle, const NzContextParameters& parameters) :
m_context(nullptr)
{
	Create(handle, parameters);

	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Failed to create render window");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzRenderWindow::~NzRenderWindow()
{
}

bool NzRenderWindow::CopyToImage(NzImage* image)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_context)
	{
		NazaraError("Window has not been created");
		return false;
	}

	if (!image)
	{
		NazaraError("Image must be valid");
		return false;
	}
	#endif

	if (!m_context->SetActive(true))
	{
		NazaraError("Failed to activate context");
		return false;
	}

	NzVector2ui size = GetSize();

	if (!image->Create(nzImageType_2D, nzPixelFormat_RGBA8, size.x, size.y, 1, 1))
	{
		NazaraError("Failed to create image");
		return false;
	}

	nzUInt8* pixels = image->GetPixels();
	glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	image->FlipVertically();

	return true;
}

bool NzRenderWindow::CopyToTexture(NzTexture* texture)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_context)
	{
		NazaraError("Window has not been created");
		return false;
	}

	if (!texture)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	if (!m_context->SetActive(true))
	{
		NazaraError("Failed to activate context");
		return false;
	}

	NzVector2ui size = GetSize();

	if (!texture->Create(nzImageType_2D, nzPixelFormat_RGBA8, size.x, size.y, 1, 1, true))
	{
		NazaraError("Failed to create texture");
		return false;
	}

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, size.x, size.y);

	texture->Unlock();

	return true;
}

bool NzRenderWindow::Create(NzVideoMode mode, const NzString& title, nzUInt32 style, const NzContextParameters& parameters)
{
    m_parameters = parameters;
    return NzWindow::Create(mode, title, style);
}

bool NzRenderWindow::Create(NzWindowHandle handle, const NzContextParameters& parameters)
{
    m_parameters = parameters;
    return NzWindow::Create(handle);
}

void NzRenderWindow::Display()
{
	if (m_context && m_parameters.doubleBuffered)
		m_context->SwapBuffers();
}

void NzRenderWindow::EnableVerticalSync(bool enabled)
{
    if (m_context)
    {
		#if defined(NAZARA_PLATFORM_WINDOWS)
        if (!m_context->SetActive(true))
		{
			NazaraError("Failed to activate context");
			return;
		}

        if (wglSwapInterval)
            wglSwapInterval(enabled ? 1 : 0);
        else
		#elif defined(NAZARA_PLATFORM_LINUX)
        if (!m_context->SetActive(true))
        {
            NazaraError("Failed to activate context");
            return;
        }

        if (glXSwapInterval)
            glXSwapInterval(enabled ? 1 : 0);
        else
		#else
			#error Vertical Sync is not supported on this platform
		#endif
            NazaraError("Vertical Sync is not supported on this platform");
    }
    else
        NazaraError("No context");
}

NzContextParameters NzRenderWindow::GetContextParameters() const
{
	if (m_context)
		return m_context->GetParameters();
	else
	{
		NazaraError("Window not created/context not initialized");
		return NzContextParameters();
	}
}

unsigned int NzRenderWindow::GetHeight() const
{
	return NzWindow::GetHeight();
}

NzRenderTargetParameters NzRenderWindow::GetParameters() const
{
	if (m_context)
	{
		const NzContextParameters& parameters = m_context->GetParameters();
		return NzRenderTargetParameters(parameters.antialiasingLevel, parameters.depthBits, parameters.stencilBits);
	}
	else
	{
		NazaraError("Window not created/context not initialized");
		return NzRenderTargetParameters();
	}
}

unsigned int NzRenderWindow::GetWidth() const
{
	return NzWindow::GetWidth();
}

bool NzRenderWindow::HasContext() const
{
	return true;
}

bool NzRenderWindow::IsValid() const
{
	return m_impl != nullptr && m_context != nullptr;
}

bool NzRenderWindow::Activate()
{
	if (m_context->SetActive(true))
	{
		glDrawBuffer((m_parameters.doubleBuffered) ? GL_BACK : GL_FRONT);
		return true;
	}
	else
	{
		NazaraError("Failed to activate window's context");
		return false;
	}
}

void NzRenderWindow::OnClose()
{
	delete m_context;
}

bool NzRenderWindow::OnCreate()
{
	m_parameters.doubleBuffered = true;
    m_parameters.window = GetHandle();

    m_context = new NzContext;
    if (!m_context->Create(m_parameters))
    {
        NazaraError("Failed not create context");
        delete m_context;

        return false;
    }

    EnableVerticalSync(false);

	#if NAZARA_RENDERER_ACTIVATE_RENDERWINDOW_ON_CREATION
	if (!SetActive(true)) // Les fen�tres s'activent � la cr�ation
		NazaraWarning("Failed to activate window");
	#endif

    return true;
}
