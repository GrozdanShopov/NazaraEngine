// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Material
	* \brief Graphics class that represents a material
	*/

	/*!
	* \brief Checks whether the parameters for the material are correct
	* \return true If parameters are valid
	*/
	bool MaterialParams::IsValid() const
	{
		if (!UberShaderLibrary::Has(shaderName))
			return false;

		return true;
	}

	/*!
	* \brief Applies shader to the material
	*
	* \param instance Pipeline instance to update
	* \param textureUnit Unit for the texture GL_TEXTURE"i"
	* \param lastUsedUnit Optional argument to get the last texture unit
	*/
	void Material::Apply(const MaterialPipeline::Instance& instance) const
	{
		const Shader* shader = instance.renderPipeline.GetInfo().shader;

		if (instance.uniforms[MaterialUniform_AlphaThreshold] != -1)
			shader->SendFloat(instance.uniforms[MaterialUniform_AlphaThreshold], m_alphaThreshold);

		if (instance.uniforms[MaterialUniform_Ambient] != -1)
			shader->SendColor(instance.uniforms[MaterialUniform_Ambient], m_ambientColor);

		if (instance.uniforms[MaterialUniform_Diffuse] != -1)
			shader->SendColor(instance.uniforms[MaterialUniform_Diffuse], m_diffuseColor);

		if (instance.uniforms[MaterialUniform_Shininess] != -1)
			shader->SendFloat(instance.uniforms[MaterialUniform_Shininess], m_shininess);

		if (instance.uniforms[MaterialUniform_Specular] != -1)
			shader->SendColor(instance.uniforms[MaterialUniform_Specular], m_specularColor);

		if (m_alphaMap && instance.uniforms[MaterialUniform_AlphaMap] != -1)
		{
			unsigned int textureUnit = s_textureUnits[TextureMap_Alpha];

			Renderer::SetTexture(textureUnit, m_alphaMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		}

		if (m_diffuseMap && instance.uniforms[MaterialUniform_DiffuseMap] != -1)
		{
			unsigned int textureUnit = s_textureUnits[TextureMap_Diffuse];

			Renderer::SetTexture(textureUnit, m_diffuseMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		}

		if (m_emissiveMap && instance.uniforms[MaterialUniform_EmissiveMap] != -1)
		{
			unsigned int textureUnit = s_textureUnits[TextureMap_Emissive];

			Renderer::SetTexture(textureUnit, m_emissiveMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		}

		if (m_heightMap && instance.uniforms[MaterialUniform_HeightMap] != -1)
		{
			unsigned int textureUnit = s_textureUnits[TextureMap_Height];

			Renderer::SetTexture(textureUnit, m_heightMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		}

		if (m_normalMap && instance.uniforms[MaterialUniform_NormalMap] != -1)
		{
			unsigned int textureUnit = s_textureUnits[TextureMap_Normal];

			Renderer::SetTexture(textureUnit, m_normalMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		}

		if (m_specularMap && instance.uniforms[MaterialUniform_SpecularMap] != -1)
		{
			unsigned int textureUnit = s_textureUnits[TextureMap_Specular];

			Renderer::SetTexture(textureUnit, m_specularMap);
			Renderer::SetTextureSampler(textureUnit, m_specularSampler);
		}
	}

	/*!
	* \brief Builds the material from a parameter list
	*
	* \param matData Data information for the material
	* \param matParams Additional parameters for the material
	*/
	void Material::BuildFromParameters(const ParameterList& matData, const MaterialParams& matParams)
	{
		Color color;
		bool isEnabled;
		double dValue;
		long long iValue;
		Nz::String path;

		ErrorFlags errFlags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled, true);

		if (matData.GetDoubleParameter(MaterialData::AlphaThreshold, &dValue))
			SetAlphaThreshold(float(dValue));

		if (matData.GetBooleanParameter(MaterialData::AlphaTest, &isEnabled))
			EnableAlphaTest(isEnabled);

		if (matData.GetColorParameter(MaterialData::AmbientColor, &color))
			SetAmbientColor(color);

		if (matData.GetIntegerParameter(MaterialData::CullingSide, &iValue))
			SetFaceCulling(static_cast<FaceSide>(iValue));

		if (matData.GetIntegerParameter(MaterialData::DepthFunc, &iValue))
			SetDepthFunc(static_cast<RendererComparison>(iValue));

		if (matData.GetBooleanParameter(MaterialData::DepthSorting, &isEnabled))
			EnableDepthSorting(isEnabled);

		if (matData.GetColorParameter(MaterialData::DiffuseColor, &color))
			SetDiffuseColor(color);

		if (matData.GetIntegerParameter(MaterialData::DstBlend, &iValue))
			SetDstBlend(static_cast<BlendFunc>(iValue));

		if (matData.GetIntegerParameter(MaterialData::FaceFilling, &iValue))
			SetFaceFilling(static_cast<FaceFilling>(iValue));

		if (matData.GetDoubleParameter(MaterialData::LineWidth, &dValue))
			SetLineWidth(float(dValue));

		if (matData.GetDoubleParameter(MaterialData::PointSize, &dValue))
			SetPointSize(float(dValue));

		if (matData.GetColorParameter(MaterialData::SpecularColor, &color))
			SetSpecularColor(color);

		if (matData.GetDoubleParameter(MaterialData::Shininess, &dValue))
			SetShininess(float(dValue));

		if (matData.GetIntegerParameter(MaterialData::SrcBlend, &iValue))
			SetSrcBlend(static_cast<BlendFunc>(iValue));

		// RendererParameter
		if (matData.GetBooleanParameter(MaterialData::Blending, &isEnabled))
			EnableBlending(isEnabled);

		if (matData.GetBooleanParameter(MaterialData::ColorWrite, &isEnabled))
			EnableColorWrite(isEnabled);

		if (matData.GetBooleanParameter(MaterialData::DepthBuffer, &isEnabled))
			EnableDepthBuffer(isEnabled);

		if (matData.GetBooleanParameter(MaterialData::DepthWrite, &isEnabled))
			EnableDepthWrite(isEnabled);

		if (matData.GetBooleanParameter(MaterialData::FaceCulling, &isEnabled))
			EnableFaceCulling(isEnabled);

		if (matData.GetBooleanParameter(MaterialData::ScissorTest, &isEnabled))
			EnableScissorTest(isEnabled);

		if (matData.GetBooleanParameter(MaterialData::StencilTest, &isEnabled))
			EnableStencilTest(isEnabled);

		if (matData.GetBooleanParameter(MaterialData::VertexColor, &isEnabled))
			EnableVertexColor(isEnabled);

		// Samplers
		if (matData.GetIntegerParameter(MaterialData::DiffuseAnisotropyLevel, &iValue))
			m_diffuseSampler.SetAnisotropyLevel(static_cast<UInt8>(iValue));

		if (matData.GetIntegerParameter(MaterialData::DiffuseFilter, &iValue))
			m_diffuseSampler.SetFilterMode(static_cast<SamplerFilter>(iValue));

		if (matData.GetIntegerParameter(MaterialData::DiffuseWrap, &iValue))
			m_diffuseSampler.SetWrapMode(static_cast<SamplerWrap>(iValue));

		if (matData.GetIntegerParameter(MaterialData::SpecularAnisotropyLevel, &iValue))
			m_specularSampler.SetAnisotropyLevel(static_cast<UInt8>(iValue));

		if (matData.GetIntegerParameter(MaterialData::SpecularFilter, &iValue))
			m_specularSampler.SetFilterMode(static_cast<SamplerFilter>(iValue));

		if (matData.GetIntegerParameter(MaterialData::SpecularWrap, &iValue))
			m_specularSampler.SetWrapMode(static_cast<SamplerWrap>(iValue));

		// Stencil
		if (matData.GetIntegerParameter(MaterialData::StencilCompare, &iValue))
			m_pipelineInfo.stencilCompare.front = static_cast<RendererComparison>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilFail, &iValue))
			m_pipelineInfo.stencilFail.front = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilPass, &iValue))
			m_pipelineInfo.stencilPass.front = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilZFail, &iValue))
			m_pipelineInfo.stencilDepthFail.front = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilMask, &iValue))
			m_pipelineInfo.stencilWriteMask.front = static_cast<UInt32>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilReference, &iValue))
			m_pipelineInfo.stencilReference.front = static_cast<unsigned int>(iValue);

		// Stencil (back)
		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilCompare, &iValue))
			m_pipelineInfo.stencilCompare.back = static_cast<RendererComparison>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilFail, &iValue))
			m_pipelineInfo.stencilFail.back = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilPass, &iValue))
			m_pipelineInfo.stencilPass.back = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilZFail, &iValue))
			m_pipelineInfo.stencilDepthFail.back = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilMask, &iValue))
			m_pipelineInfo.stencilWriteMask.back = static_cast<UInt32>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilReference, &iValue))
			m_pipelineInfo.stencilReference.back = static_cast<unsigned int>(iValue);

		InvalidatePipeline();

		// Textures
		if (matParams.loadAlphaMap && matData.GetStringParameter(MaterialData::AlphaTexturePath, &path))
			SetAlphaMap(path.ToStdString());

		if (matParams.loadDiffuseMap && matData.GetStringParameter(MaterialData::DiffuseTexturePath, &path))
			SetDiffuseMap(path.ToStdString());

		if (matParams.loadEmissiveMap && matData.GetStringParameter(MaterialData::EmissiveTexturePath, &path))
			SetEmissiveMap(path.ToStdString());

		if (matParams.loadHeightMap && matData.GetStringParameter(MaterialData::HeightTexturePath, &path))
			SetHeightMap(path.ToStdString());

		if (matParams.loadNormalMap && matData.GetStringParameter(MaterialData::NormalTexturePath, &path))
			SetNormalMap(path.ToStdString());

		if (matParams.loadSpecularMap && matData.GetStringParameter(MaterialData::SpecularTexturePath, &path))
			SetSpecularMap(path.ToStdString());

		SetShader(matParams.shaderName);
	}

	/*!
	* \brief Builds a ParameterList with material data
	*
	* \param matData Destination parameter list which will receive material data
	*/
	void Material::SaveToParameters(ParameterList* matData)
	{
		NazaraAssert(matData, "Invalid ParameterList");

		matData->SetParameter(MaterialData::AlphaTest, IsAlphaTestEnabled());
		matData->SetParameter(MaterialData::AlphaThreshold, GetAlphaThreshold());
		matData->SetParameter(MaterialData::AmbientColor, GetAmbientColor());
		matData->SetParameter(MaterialData::CullingSide, static_cast<long long>(GetFaceCulling()));
		matData->SetParameter(MaterialData::DepthFunc, static_cast<long long>(GetDepthFunc()));
		matData->SetParameter(MaterialData::DepthSorting, IsDepthSortingEnabled());
		matData->SetParameter(MaterialData::DiffuseColor, GetDiffuseColor());
		matData->SetParameter(MaterialData::DstBlend, static_cast<long long>(GetDstBlend()));
		matData->SetParameter(MaterialData::FaceFilling, static_cast<long long>(GetFaceFilling()));
		matData->SetParameter(MaterialData::LineWidth, GetLineWidth());
		matData->SetParameter(MaterialData::PointSize, GetPointSize());
		matData->SetParameter(MaterialData::Shininess, GetShininess());
		matData->SetParameter(MaterialData::SpecularColor, GetSpecularColor());
		matData->SetParameter(MaterialData::SrcBlend, static_cast<long long>(GetSrcBlend()));

		// RendererParameter
		matData->SetParameter(MaterialData::Blending, IsBlendingEnabled());
		matData->SetParameter(MaterialData::ColorWrite, IsColorWriteEnabled());
		matData->SetParameter(MaterialData::DepthBuffer, IsDepthBufferEnabled());
		matData->SetParameter(MaterialData::DepthWrite, IsDepthWriteEnabled());
		matData->SetParameter(MaterialData::FaceCulling, IsFaceCullingEnabled());
		matData->SetParameter(MaterialData::ScissorTest, IsScissorTestEnabled());
		matData->SetParameter(MaterialData::StencilTest, IsStencilTestEnabled());
		matData->SetParameter(MaterialData::VertexColor, HasVertexColor());

		// Samplers
		matData->SetParameter(MaterialData::DiffuseAnisotropyLevel, static_cast<long long>(GetDiffuseSampler().GetAnisotropicLevel()));
		matData->SetParameter(MaterialData::DiffuseFilter, static_cast<long long>(GetDiffuseSampler().GetFilterMode()));
		matData->SetParameter(MaterialData::DiffuseWrap, static_cast<long long>(GetDiffuseSampler().GetWrapMode()));

		matData->SetParameter(MaterialData::SpecularAnisotropyLevel, static_cast<long long>(GetSpecularSampler().GetAnisotropicLevel()));
		matData->SetParameter(MaterialData::SpecularFilter, static_cast<long long>(GetSpecularSampler().GetFilterMode()));
		matData->SetParameter(MaterialData::SpecularWrap, static_cast<long long>(GetSpecularSampler().GetWrapMode()));

		// Stencil
		matData->SetParameter(MaterialData::StencilCompare,   static_cast<long long>(GetPipelineInfo().stencilCompare.front));
		matData->SetParameter(MaterialData::StencilFail,      static_cast<long long>(GetPipelineInfo().stencilFail.front));
		matData->SetParameter(MaterialData::StencilPass,      static_cast<long long>(GetPipelineInfo().stencilPass.front));
		matData->SetParameter(MaterialData::StencilZFail,     static_cast<long long>(GetPipelineInfo().stencilDepthFail.front));
		matData->SetParameter(MaterialData::StencilMask,      static_cast<long long>(GetPipelineInfo().stencilWriteMask.front));
		matData->SetParameter(MaterialData::StencilReference, static_cast<long long>(GetPipelineInfo().stencilReference.front));

		// Stencil (back)
		matData->SetParameter(MaterialData::BackFaceStencilCompare,   static_cast<long long>(GetPipelineInfo().stencilCompare.back));
		matData->SetParameter(MaterialData::BackFaceStencilFail,      static_cast<long long>(GetPipelineInfo().stencilFail.back));
		matData->SetParameter(MaterialData::BackFaceStencilPass,      static_cast<long long>(GetPipelineInfo().stencilPass.back));
		matData->SetParameter(MaterialData::BackFaceStencilZFail,     static_cast<long long>(GetPipelineInfo().stencilDepthFail.back));
		matData->SetParameter(MaterialData::BackFaceStencilMask,      static_cast<long long>(GetPipelineInfo().stencilWriteMask.back));
		matData->SetParameter(MaterialData::BackFaceStencilReference, static_cast<long long>(GetPipelineInfo().stencilReference.back));

		// Textures
		if (HasAlphaMap())
		{
			const std::filesystem::path& path = GetAlphaMap()->GetFilePath();
			if (!path.empty())
				matData->SetParameter(MaterialData::AlphaTexturePath, path.generic_u8string());
		}

		if (HasDiffuseMap())
		{
			const std::filesystem::path& path = GetDiffuseMap()->GetFilePath();
			if (!path.empty())
				matData->SetParameter(MaterialData::DiffuseTexturePath, path.generic_u8string());
		}

		if (HasEmissiveMap())
		{
			const std::filesystem::path& path = GetEmissiveMap()->GetFilePath();
			if (!path.empty())
				matData->SetParameter(MaterialData::EmissiveTexturePath, path.generic_u8string());
		}

		if (HasHeightMap())
		{
			const std::filesystem::path& path = GetHeightMap()->GetFilePath();
			if (!path.empty())
				matData->SetParameter(MaterialData::HeightTexturePath, path.generic_u8string());
		}

		if (HasNormalMap())
		{
			const std::filesystem::path& path = GetNormalMap()->GetFilePath();
			if (!path.empty())
				matData->SetParameter(MaterialData::NormalTexturePath, path.generic_u8string());
		}

		if (HasSpecularMap())
		{
			const std::filesystem::path& path = GetSpecularMap()->GetFilePath();
			if (!path.empty())
				matData->SetParameter(MaterialData::SpecularTexturePath, path.generic_u8string());
		}
	}

	/*!
	* \brief Resets the material, cleans everything
	*
	* \remark Invalidates the pipeline
	*/
	void Material::Reset()
	{
		OnMaterialReset(this);

		m_alphaMap.Reset();
		m_depthMaterial.Reset();
		m_diffuseMap.Reset();
		m_emissiveMap.Reset();
		m_heightMap.Reset();
		m_normalMap.Reset();
		m_specularMap.Reset();

		m_alphaThreshold = 0.2f;
		m_ambientColor = Color(128, 128, 128);
		m_diffuseColor = Color::White;
		m_diffuseSampler = TextureSampler();
		m_reflectionMode = ReflectionMode_Skybox;
		m_shadowCastingEnabled = true;
		m_shininess = 50.f;
		m_specularColor = Color::White;
		m_specularSampler = TextureSampler();
		m_pipelineInfo = MaterialPipelineInfo();
		m_pipelineInfo.depthBuffer = true;
		m_pipelineInfo.faceCulling = true;
		m_reflectionSize = 256;

		SetShader("Basic");

		InvalidatePipeline();
	}

	/*!
	* \brief Copies the other material
	*
	* \param material Material to copy into this
	*/
	void Material::Copy(const Material& material)
	{
		// Copy of base states
		m_alphaThreshold       = material.m_alphaThreshold;
		m_ambientColor         = material.m_ambientColor;
		m_diffuseColor         = material.m_diffuseColor;
		m_diffuseSampler       = material.m_diffuseSampler;
		m_pipelineInfo         = material.m_pipelineInfo;
		m_shininess            = material.m_shininess;
		m_shadowCastingEnabled = material.m_shadowCastingEnabled;
		m_specularColor        = material.m_specularColor;
		m_specularSampler      = material.m_specularSampler;

		// Copy of reference to the textures
		m_alphaMap      = material.m_alphaMap;
		m_depthMaterial = material.m_depthMaterial;
		m_diffuseMap    = material.m_diffuseMap;
		m_emissiveMap   = material.m_emissiveMap;
		m_heightMap     = material.m_heightMap;
		m_normalMap     = material.m_normalMap;
		m_specularMap   = material.m_specularMap;

		SetReflectionMode(material.GetReflectionMode());

		InvalidatePipeline();
	}

	/*!
	* \brief Initializes the material librairies
	* \return true If successful
	*
	* \remark Produces a NazaraError if the material library failed to be initialized
	*/
	bool Material::Initialize()
	{
		if (!MaterialLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		if (!MaterialManager::Initialize())
		{
			NazaraError("Failed to initialise manager");
			return false;
		}

		s_defaultMaterial = New();
		s_defaultMaterial->EnableFaceCulling(false);
		s_defaultMaterial->SetFaceFilling(FaceFilling_Line);
		MaterialLibrary::Register("Default", s_defaultMaterial);

		unsigned int textureUnit = 0;

		s_textureUnits[TextureMap_Diffuse]        = textureUnit++;
		s_textureUnits[TextureMap_Alpha]          = textureUnit++;
		s_textureUnits[TextureMap_Specular]       = textureUnit++;
		s_textureUnits[TextureMap_Normal]         = textureUnit++;
		s_textureUnits[TextureMap_Emissive]       = textureUnit++;
		s_textureUnits[TextureMap_Overlay]        = textureUnit++;
		s_textureUnits[TextureMap_ReflectionCube] = textureUnit++;
		s_textureUnits[TextureMap_Height]         = textureUnit++;
		s_textureUnits[TextureMap_Shadow2D_1]     = textureUnit++;
		s_textureUnits[TextureMap_ShadowCube_1]   = textureUnit++;
		s_textureUnits[TextureMap_Shadow2D_2]     = textureUnit++;
		s_textureUnits[TextureMap_ShadowCube_2]   = textureUnit++;
		s_textureUnits[TextureMap_Shadow2D_3]     = textureUnit++;
		s_textureUnits[TextureMap_ShadowCube_3]   = textureUnit++;

		return true;
	}

	/*!
	* \brief Uninitializes the material librairies
	*/
	void Material::Uninitialize()
	{
		s_defaultMaterial.Reset();

		MaterialManager::Uninitialize();
		MaterialLibrary::Uninitialize();
	}

	std::array<int, TextureMap_Max + 1> Material::s_textureUnits;
	MaterialLibrary::LibraryMap Material::s_library;
	MaterialLoader::LoaderList Material::s_loaders;
	MaterialManager::ManagerMap Material::s_managerMap;
	MaterialManager::ManagerParams Material::s_managerParameters;
	MaterialRef Material::s_defaultMaterial = nullptr;
}
