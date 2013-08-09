// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERTEXTURE_HPP
#define NAZARA_RENDERTEXTURE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Texture.hpp>

///TODO: Faire fonctionner les RenderTexture indépendamment du contexte (un FBO par classe et par contexte l'utilisant)

struct NzRenderTextureImpl;

class NAZARA_API NzRenderTexture : public NzRenderTarget, NzResourceListener, NzNonCopyable
{
	public:
		NzRenderTexture() = default;
		~NzRenderTexture();

		bool AttachBuffer(nzAttachmentPoint attachmentPoint, nzUInt8 index, nzPixelFormat format);
		bool AttachTexture(nzAttachmentPoint attachmentPoint, nzUInt8 index, NzTexture* texture, unsigned int z = 0);

		bool Create(unsigned int width, unsigned int height, bool lock = false);
		void Destroy();

		void Detach(nzAttachmentPoint attachmentPoint, nzUInt8 index);

		unsigned int GetHeight() const;
		NzRenderTargetParameters GetParameters() const;
		unsigned int GetWidth() const;

		bool IsComplete() const;
		bool IsRenderable() const;
		bool IsValid() const;

		bool Lock() const;
		void Unlock() const;

		// Fonctions OpenGL
		bool HasContext() const;

		static bool IsSupported();

	protected:
		bool Activate() const override;
		void Desactivate() const override;

	private:
		bool OnResourceDestroy(const NzResource* resource, int index) override;

		NzRenderTextureImpl* m_impl = nullptr;
};

#endif // NAZARA_RENDERTEXTURE_HPP
