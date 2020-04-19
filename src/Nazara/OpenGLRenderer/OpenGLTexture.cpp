// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <stdexcept>
#include <vma/vk_mem_alloc.h>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLTexture::OpenGLTexture(OpenGLDevice& device, const TextureInfo& params) :
	m_params(params)
	{
		if (!m_texture.Create(device))
			throw std::runtime_error("failed to create texture object");

		GLint internalFormat;
		switch (params.pixelFormat)
		{
			case PixelFormat_RGB8:
			{
				internalFormat = GL_RGB8;
				break;
			}

			case PixelFormat_RGBA8:
			{
				internalFormat = GL_RGBA8;
				break;
			}
		}

		switch (params.type)
		{
			case ImageType_1D:
				break;

			case ImageType_1D_Array:
				break;

			case ImageType_2D:
				for (unsigned int level = 0; level < m_params.mipmapLevel; ++level)
					m_texture.TexImage2D(0, internalFormat, GetLevelSize(params.width, level), GetLevelSize(params.height, level), 0);
				break;

			case ImageType_2D_Array:
				break;

			case ImageType_3D:
				break;

			case ImageType_Cubemap:
				break;

			default:
				break;
		}
	}

	PixelFormat OpenGLTexture::GetFormat() const
	{
		return m_params.pixelFormat;
	}

	UInt8 OpenGLTexture::GetLevelCount() const
	{
		return m_params.mipmapLevel;
	}

	Vector3ui OpenGLTexture::GetSize(UInt8 level) const
	{
		return Vector3ui(GetLevelSize(m_params.width, level), GetLevelSize(m_params.height, level), GetLevelSize(m_params.depth, level));
	}

	ImageType OpenGLTexture::GetType() const
	{
		return m_params.type;
	}

	bool OpenGLTexture::Update(const void* ptr)
	{
		GLint format;
		GLint type;
		switch (m_params.pixelFormat)
		{
			case PixelFormat_RGB8:
			{
				format = GL_RGB;
				type = GL_UNSIGNED_BYTE;
				break;
			}

			case PixelFormat_RGBA8:
			{
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				break;
			}
		}
		
		switch (m_params.type)
		{
			case ImageType_1D:
				break;

			case ImageType_1D_Array:
				break;

			case ImageType_2D:
				m_texture.TexSubImage2D(0, 0, 0, m_params.width, m_params.height, format, type, ptr);
				break;

			case ImageType_2D_Array:
				break;

			case ImageType_3D:
				break;

			case ImageType_Cubemap:
				break;

			default:
				break;
		}

		return true;
	}
}
