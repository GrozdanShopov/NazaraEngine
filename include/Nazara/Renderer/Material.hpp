// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_HPP
#define NAZARA_MATERIAL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Texture.hpp>

struct NAZARA_API NzMaterialParams
{
	bool loadDiffuseMap = true;
	bool loadSpecularMap = true;

	bool IsValid() const;
};

class NzMaterial;

using NzMaterialLoader = NzResourceLoader<NzMaterial, NzMaterialParams>;

class NAZARA_API NzMaterial : public NzResource
{
	friend NzMaterialLoader;

	public:
		NzMaterial();
		~NzMaterial();

		void EnableAlphaBlending(bool alphaBlending);
		void EnableZTest(bool zTest);
		void EnableZWrite(bool zWrite);

		NzColor GetAmbientColor() const;
		NzColor GetDiffuseColor() const;
		const NzTexture* GetDiffuseMap() const;
		nzBlendFunc GetDstBlend() const;
		nzFaceCulling GetFaceCulling() const;
		nzFaceFilling GetFaceFilling() const;
		float GetShininess() const;
		NzColor GetSpecularColor() const;
		const NzTexture* GetSpecularMap() const;
		nzBlendFunc GetSrcBlend() const;
		nzTextureFilter GetTextureFilter() const;
		nzRendererComparison GetZTestCompare() const;

		bool IsAlphaBlendingEnabled() const;
		bool IsZTestEnabled() const;
		bool IsZWriteEnabled() const;

		bool LoadFromFile(const NzString& filePath, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params = NzMaterialParams());
		bool LoadFromStream(NzInputStream& stream, const NzMaterialParams& params = NzMaterialParams());

		void Reset();

		void SetAmbientColor(const NzColor& ambient);
		void SetDiffuseColor(const NzColor& diffuse);
		void SetDiffuseMap(const NzTexture* map);
		void SetDstBlend(nzBlendFunc func);
		void SetFaceCulling(nzFaceCulling culling);
		void SetFaceFilling(nzFaceFilling filling);
		void SetShininess(float shininess);
		void SetSpecularColor(const NzColor& specular);
		void SetSpecularMap(const NzTexture* map);
		void SetSrcBlend(nzBlendFunc func);
		void SetTextureFilter(nzTextureFilter filter);
		void SetTextureWrap(nzTextureWrap wrapMode);
		void SetZTestCompare(nzRendererComparison compareFunc);

		static const NzMaterial* GetDefault();

	private:
		nzBlendFunc m_dstBlend;
		nzBlendFunc m_srcBlend;
		nzFaceCulling m_faceCulling;
		nzFaceFilling m_faceFilling;
		nzRendererComparison m_zTestCompareFunc;
		nzTextureFilter m_textureFilter;
		nzTextureWrap m_textureWrap;
		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
		const NzTexture* m_diffuseMap;
		const NzTexture* m_specularMap;
		bool m_alphaBlendingEnabled;
		bool m_zTestEnabled;
		bool m_zWriteEnabled;
		float m_shininess;

		static NzMaterialLoader::LoaderList s_loaders;
};

#endif // NAZARA_MATERIAL_HPP
