// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Material.hpp>

bool NzMaterialParams::IsValid() const
{
	return true;
}

NzMaterial::NzMaterial() :
m_diffuseMap(nullptr),
m_specularMap(nullptr)
{
	Reset();
}

NzMaterial::~NzMaterial()
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();
}

void NzMaterial::EnableAlphaBlending(bool alphaBlending)
{
	m_alphaBlendingEnabled = alphaBlending;
}

void NzMaterial::EnableZTest(bool zTest)
{
	m_zTestEnabled = zTest;
}

void NzMaterial::EnableZWrite(bool zWrite)
{
	m_zWriteEnabled = zWrite;
}

NzColor NzMaterial::GetAmbientColor() const
{
	return m_ambientColor;
}

NzColor NzMaterial::GetDiffuseColor() const
{
	return m_diffuseColor;
}

const NzTexture* NzMaterial::GetDiffuseMap() const
{
	return m_diffuseMap;
}

nzBlendFunc NzMaterial::GetDstBlend() const
{
	return m_dstBlend;
}

nzFaceCulling NzMaterial::GetFaceCulling() const
{
	return m_faceCulling;
}

nzFaceFilling NzMaterial::GetFaceFilling() const
{
	return m_faceFilling;
}

float NzMaterial::GetShininess() const
{
	return m_shininess;
}

NzColor NzMaterial::GetSpecularColor() const
{
	return m_specularColor;
}

const NzTexture* NzMaterial::GetSpecularMap() const
{
	return m_specularMap;
}

nzBlendFunc NzMaterial::GetSrcBlend() const
{
	return m_srcBlend;
}

nzRendererComparison NzMaterial::GetZTestCompare() const
{
	return m_zTestCompareFunc;
}

bool NzMaterial::IsAlphaBlendingEnabled() const
{
	return m_alphaBlendingEnabled;
}

bool NzMaterial::IsZTestEnabled() const
{
	return m_zTestEnabled;
}

bool NzMaterial::IsZWriteEnabled() const
{
	return m_zWriteEnabled;
}

bool NzMaterial::LoadFromFile(const NzString& filePath, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromFile(this, filePath, params);
}

bool NzMaterial::LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromMemory(this, data, size, params);
}

bool NzMaterial::LoadFromStream(NzInputStream& stream, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromStream(this, stream, params);
}

void NzMaterial::Reset()
{
	if (m_diffuseMap)
	{
		m_diffuseMap->RemoveResourceReference();
		m_diffuseMap = nullptr;
	}

	if (m_specularMap)
	{
		m_specularMap->RemoveResourceReference();
		m_specularMap = nullptr;
	}

	m_alphaBlendingEnabled = false;
	m_ambientColor = NzColor::Black;
	m_diffuseColor = NzColor::White;
	m_dstBlend = nzBlendFunc_Zero;
	m_faceCulling = nzFaceCulling_Back;
	m_faceFilling = nzFaceFilling_Fill;
	m_shininess = 0;
	m_specularColor = NzColor::White;
	m_srcBlend = nzBlendFunc_One;
	m_textureFilter = nzTextureFilter_Default;
	m_textureWrap = nzTextureWrap_Repeat;
	m_zTestCompareFunc = nzRendererComparison_LessOrEqual;
	m_zTestEnabled = true;
	m_zWriteEnabled = true;
}

void NzMaterial::SetAmbientColor(const NzColor& ambient)
{
	m_ambientColor = ambient;
}

void NzMaterial::SetDiffuseColor(const NzColor& diffuse)
{
	m_diffuseColor = diffuse;
}

void NzMaterial::SetDiffuseMap(const NzTexture* map)
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	m_diffuseMap = map;
	if (m_diffuseMap)
		m_diffuseMap->AddResourceReference();
}

void NzMaterial::SetDstBlend(nzBlendFunc func)
{
	m_dstBlend = func;
}

void NzMaterial::SetFaceCulling(nzFaceCulling culling)
{
	m_faceCulling = culling;
}

void NzMaterial::SetFaceFilling(nzFaceFilling filling)
{
	m_faceFilling = filling;
}

void NzMaterial::SetShininess(float shininess)
{
	m_shininess = shininess;
}

void NzMaterial::SetSpecularColor(const NzColor& specular)
{
	m_specularColor = specular;
}

void NzMaterial::SetSpecularMap(const NzTexture* map)
{
	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	m_specularMap = map;
	if (m_specularMap)
		m_specularMap->AddResourceReference();
}

void NzMaterial::SetSrcBlend(nzBlendFunc func)
{
	m_srcBlend = func;
}

void NzMaterial::SetZTestCompare(nzRendererComparison compareFunc)
{
	m_zTestEnabled = compareFunc;
}

const NzMaterial* NzMaterial::GetDefault()
{
	static NzMaterial defaultMaterial;
	static bool initialized = false;

	if (!initialized)
	{
		defaultMaterial.SetFaceCulling(nzFaceCulling_FrontAndBack);
		defaultMaterial.SetFaceFilling(nzFaceFilling_Line);
		defaultMaterial.SetDiffuseColor(NzColor::White);

		initialized = true;
	}

	return &defaultMaterial;
}

NzMaterialLoader::LoaderList NzMaterial::s_loaders;
