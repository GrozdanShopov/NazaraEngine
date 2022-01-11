// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Destructs the object and calls OnMaterialRelease
	*
	* \see OnMaterialRelease
	*/
	inline MaterialPass::~MaterialPass()
	{
		OnMaterialRelease(this);
	}

	/*!
	* \brief Reset material pipeline state
	*
	* Sets the material pipeline
	*
	* \remark pipeline must be valid
	*
	* \see Configure
	*/
	inline void MaterialPass::Configure(std::shared_ptr<MaterialPipeline> pipeline)
	{
		NazaraAssert(pipeline, "Invalid material pipeline");

		m_pipeline = std::move(pipeline);
		m_pipelineInfo = m_pipeline->GetInfo();
		m_pipelineUpdated = true;
	}

	/*!
	* \brief Reset material pipeline state
	*
	* Sets the material pipeline using pipeline info
	*
	* \remark pipeline must be valid
	*
	* \see Configure
	*/
	inline void MaterialPass::Configure(const MaterialPipelineInfo& pipelineInfo)
	{
		m_pipelineInfo = pipelineInfo;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable blending for this material
	*
	* When enabled, all objects using this material will be rendered using blending, obeying the dstBlend and srcBlend parameters
	* This is useful with translucent objects, but will reduces performance as it prevents some optimizations (as deferred rendering)
	*
	* \param blending Defines if this material will use blending
	*
	* \remark Invalidates the pipeline
	*
	* \see IsBlendingEnabled
	* \see SetDstBlend
	* \see SetSrcBlend
	*/
	inline void MaterialPass::EnableBlending(bool blending)
	{
		m_pipelineInfo.blending = blending;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable color writing for this material
	*
	* \param colorWrite Defines if this material will use color writing
	*
	* \remark Invalidates the pipeline
	*
	* \see IsColorWritingEnabled
	*/
	inline void MaterialPass::EnableColorWrite(bool colorWrite)
	{
		m_pipelineInfo.colorWrite = colorWrite;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable depth buffer for this material
	*
	* When enabled, all objects using this material will be rendered using a depth buffer, if the RenderTarget has one.
	* This will enable Depth Test, preventing further fragments to render on top of closer ones.
	*
	* This parameter is required for depth writing.
	*
	* In order to enable depth writing without enabling depth test, set the depth comparison function to RendererComparison::Never
	*
	* \param depthBuffer Defines if this material will use depth buffer
	*
	* \remark Invalidates the pipeline
	*
	* \see EnableDepthWrite
	* \see IsDepthBufferEnabled
	* \see SetDepthFunc
	*/
	inline void MaterialPass::EnableDepthBuffer(bool depthBuffer)
	{
		m_pipelineInfo.depthBuffer = depthBuffer;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable depth clamp for this material
	*
	* When enabled, all fragments generated by this material will be clamped to znear or zfar
	* instead of being clipped out when outside this range.
	*
	* This can be useful to prevent clipping near or far primitives.
	*
	* \param depthClamp Defines if this material will use depth clamping
	*
	* \remark Invalidates the pipeline
	* \remark Depth clamping requires RenderDeviceFeatures::depthClamping to be true
	*
	* \see IsDepthClampEnabled
	*/
	inline void MaterialPass::EnableDepthClamp(bool depthClamp)
	{
		m_pipelineInfo.depthClamp = depthClamp;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable depth writing for this material
	*
	* When enabled, and if depth buffer is enabled and present, all fragments generated with this material will write
	* to the depth buffer if they pass depth test.
	*
	* This is usually disabled with translucent objects, as depth test is wanted to prevent them from rendering on top of opaque objects but
	* not depth writing (which could make other translucent fragments to fail depth test)
	*
	* \param depthBuffer Defines if this material will use depth write
	*
	* \remark Invalidates the pipeline
	*
	* \see EnableDepthBuffer
	* \see IsDepthWriteEnabled
	*/
	inline void MaterialPass::EnableDepthWrite(bool depthWrite)
	{
		m_pipelineInfo.depthWrite = depthWrite;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable face culling for this material
	*
	* When enabled, the material prevents front and/or back faces from rendering.
	* This is commonly used as an optimization to prevent processing of hidden faces by the rendering device.
	*
	* Use SetFaceCulling to control which side will be eliminated.
	*
	* \param faceCulling Defines if this material will use face culling
	*
	* \remark Invalidates the pipeline
	*
	* \see IsFaceCullingEnabled
	* \see SetFaceCulling
	*/
	inline void MaterialPass::EnableFaceCulling(bool faceCulling)
	{
		m_pipelineInfo.faceCulling = faceCulling;

		InvalidatePipeline();
	}

	inline void MaterialPass::EnableFlag(MaterialPassFlag flag, bool enable)
	{
		if (enable)
			m_flags |= flag;
		else
			m_flags &= ~flag;
	}

	/*!
	* \brief Enable/Disable scissor test for this material
	*
	* When enabled, the material prevents fragments out of the scissor box to be rendered.
	* This can be useful with GUI, where widgets must not be rendered outside of their parent rendering area.
	*
	* \param scissorTest Defines if this material will use scissor test
	*
	* \remark Invalidates the pipeline
	*
	* \see IsScissorTestEnabled
	*/
	inline void MaterialPass::EnableScissorTest(bool scissorTest)
	{
		m_pipelineInfo.scissorTest = scissorTest;

		InvalidatePipeline();
	}

	/*!
	* \brief Enable/Disable stencil test for this material
	*
	* When enabled, all fragments must pass the stencil test to be rendered.
	*
	* \param scissorTest Defines if this material will use stencil test
	*
	* \remark Invalidates the pipeline
	*
	* \see IsStencilTestEnabled
	*/
	inline void MaterialPass::EnableStencilTest(bool stencilTest)
	{
		m_pipelineInfo.stencilTest = stencilTest;

		InvalidatePipeline();
	}

	/*!
	* \brief Ensures the pipeline gets updated
	*
	* When the pipeline gets invalidated, it's not updated until required (per example by calling GetPipeline).
	* Using this function forces the pipeline update, making GetPipeline thread-safe as long as the pipeline does not get invalidated.
	*
	* \see GetPipeline
	*/
	inline void MaterialPass::EnsurePipelineUpdate() const
	{
		if (!m_pipelineUpdated)
			UpdatePipeline();
	}

	/*!
	* \brief Gets the function to compare depth
	*
	* \return Function comparing the depth of two materials
	*
	* \see EnableDepthTest
	* \see SetAmbientColor
	*/
	inline RendererComparison MaterialPass::GetDepthCompareFunc() const
	{
		return m_pipelineInfo.depthCompare;
	}

	inline BlendEquation MaterialPass::GetBlendAlphaModeEquation() const
	{
		return m_pipelineInfo.blend.modeAlpha;
	}

	inline BlendEquation MaterialPass::GetBlendColorModeEquation() const
	{
		return m_pipelineInfo.blend.modeColor;
	}

	inline BlendFunc MaterialPass::GetBlendDstAlphaFunc() const
	{
		return m_pipelineInfo.blend.dstAlpha;
	}

	inline BlendFunc MaterialPass::GetBlendDstColorFunc() const
	{
		return m_pipelineInfo.blend.dstColor;
	}

	inline BlendFunc MaterialPass::GetBlendSrcAlphaFunc() const
	{
		return m_pipelineInfo.blend.srcAlpha;
	}

	inline BlendFunc MaterialPass::GetBlendSrcColorFunc() const
	{
		return m_pipelineInfo.blend.srcColor;
	}

	/*!
	* \brief Gets the face culling
	*
	* \return Current face culling side
	*
	* \see SetFaceCulling
	*/
	inline FaceSide MaterialPass::GetFaceCulling() const
	{
		return m_pipelineInfo.cullingSide;
	}

	/*!
	* \brief Gets the face filling
	* \return Current face filling
	*/
	inline FaceFilling MaterialPass::GetFaceFilling() const
	{
		return m_pipelineInfo.faceFilling;
	}

	/*!
	* \brief Gets the line width of this material
	* \return Line width
	*/
	inline float MaterialPass::GetLineWidth() const
	{
		return m_pipelineInfo.lineWidth;
	}

	inline MaterialPassFlags MaterialPass::GetFlags() const
	{
		return m_flags;
	}

	inline const ShaderAst::ConstantValue& MaterialPass::GetOptionValue(std::size_t optionIndex) const
	{
		assert(optionIndex < m_optionValues.size());
		return m_optionValues[optionIndex];
	}

	/*!
	* \brief Gets the render states
	* \return Constant reference to the render states
	*/
	inline const std::shared_ptr<MaterialPipeline>& MaterialPass::GetPipeline() const
	{
		EnsurePipelineUpdate();

		return m_pipeline;
	}

	/*!
	* \brief Gets the pipeline informations
	* \return Constant reference to the pipeline info
	*/
	inline const MaterialPipelineInfo& MaterialPass::GetPipelineInfo() const
	{
		return m_pipelineInfo;
	}

	/*!
	* \brief Gets the point size of this material
	* \return Point size
	*/
	inline float MaterialPass::GetPointSize() const
	{
		return m_pipelineInfo.pointSize;
	}

	inline PrimitiveMode MaterialPass::GetPrimitiveMode() const
	{
		return m_pipelineInfo.primitiveMode;
	}

	inline const std::shared_ptr<const MaterialSettings>& MaterialPass::GetSettings() const
	{
		return m_settings;
	}

	/*!
	* \brief Gets the über-shader used by this material
	* \return Constant pointer to the über-shader used
	*/
	inline const std::shared_ptr<UberShader>& MaterialPass::GetShader(ShaderStageType shaderStage) const
	{
		return m_pipelineInfo.shaders[UnderlyingCast(shaderStage)].uberShader;
	}

	inline const std::shared_ptr<Texture>& MaterialPass::GetTexture(std::size_t textureIndex) const
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		return m_textures[textureIndex].texture;
	}

	inline const TextureSamplerInfo& MaterialPass::GetTextureSampler(std::size_t textureIndex) const
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		return m_textures[textureIndex].samplerInfo;
	}

	inline const std::shared_ptr<AbstractBuffer>& MaterialPass::GetUniformBuffer(std::size_t bufferIndex) const
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid uniform buffer index");
		return m_uniformBuffers[bufferIndex].buffer;
	}

	inline const std::vector<UInt8>& MaterialPass::GetUniformBufferConstData(std::size_t bufferIndex) const
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid uniform buffer index");
		return m_uniformBuffers[bufferIndex].data;
	}

	inline std::vector<UInt8>& MaterialPass::GetUniformBufferData(std::size_t bufferIndex)
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid uniform buffer index");
		UniformBuffer& uboEntry = m_uniformBuffers[bufferIndex];
		InvalidateUniformData(bufferIndex);

		return uboEntry.data;
	}

	inline bool MaterialPass::HasTexture(std::size_t textureIndex) const
	{
		return GetTexture(textureIndex) != nullptr;
	}

	/*!
	* \brief Checks whether this material has blending enabled
	* \return true If it is the case
	*/
	inline bool MaterialPass::IsBlendingEnabled() const
	{
		return m_pipelineInfo.blending;
	}

	/*!
	* \brief Checks whether this material has color write enabled
	* \return true If it is the case
	*/
	inline bool MaterialPass::IsColorWriteEnabled() const
	{
		return m_pipelineInfo.colorWrite;
	}

	/*!
	* \brief Checks whether this material has depth buffer enabled
	* \return true If it is the case
	*/
	inline bool MaterialPass::IsDepthBufferEnabled() const
	{
		return m_pipelineInfo.depthBuffer;
	}

	/*!
	* \brief Checks whether this material has depth clamping enabled
	* \return true If it is the case
	*/
	inline bool MaterialPass::IsDepthClampEnabled() const
	{
		return m_pipelineInfo.depthClamp;
	}

	/*!
	* \brief Checks whether this material has depth writing enabled
	* \return true If it is the case
	*/
	inline bool MaterialPass::IsDepthWriteEnabled() const
	{
		return m_pipelineInfo.depthWrite;
	}

	/*!
	* \brief Checks whether this material has face culling enabled
	* \return true If it is the case
	*/
	inline bool MaterialPass::IsFaceCullingEnabled() const
	{
		return m_pipelineInfo.faceCulling;
	}

	inline bool MaterialPass::IsFlagEnabled(MaterialPassFlag flag) const
	{
		return m_flags.Test(flag);
	}

	/*!
	* \brief Checks whether this material has scissor test enabled
	* \return true If it is the case
	*/
	inline bool MaterialPass::IsScissorTestEnabled() const
	{
		return m_pipelineInfo.scissorTest;
	}

	/*!
	* \brief Checks whether this material has stencil test enabled
	* \return true If it is the case
	*/
	inline bool MaterialPass::IsStencilTestEnabled() const
	{
		return m_pipelineInfo.stencilTest;
	}

	/*!
	* \brief Sets the depth functor
	*
	* \param depthFunc
	*
	* \remark Invalidates the pipeline
	*/
	inline void MaterialPass::SetDepthCompareFunc(RendererComparison depthFunc)
	{
		m_pipelineInfo.depthCompare = depthFunc;

		InvalidatePipeline();
	}

	inline void MaterialPass::SetBlendEquation(BlendEquation colorMode, BlendEquation alphaMode)
	{
		m_pipelineInfo.blend.modeAlpha = alphaMode;
		m_pipelineInfo.blend.modeColor = colorMode;

		InvalidatePipeline();
	}

	inline void MaterialPass::SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
	{
		m_pipelineInfo.blend.dstAlpha = dstAlpha;
		m_pipelineInfo.blend.dstColor = dstColor;
		m_pipelineInfo.blend.srcAlpha = srcAlpha;
		m_pipelineInfo.blend.srcColor = srcColor;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the face culling
	*
	* \param faceSide Face to cull
	*
	* \remark Invalidates the pipeline
	*/
	inline void MaterialPass::SetFaceCulling(FaceSide faceSide)
	{
		m_pipelineInfo.cullingSide = faceSide;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the face filling
	*
	* \param filling Face to fill
	*
	* \remark Invalidates the pipeline
	*/
	inline void MaterialPass::SetFaceFilling(FaceFilling filling)
	{
		m_pipelineInfo.faceFilling = filling;

		InvalidatePipeline();
	}

	/*!
	* \brief Sets the line width for this material
	*
	* This parameter is used when rendering lines, to define the width (in pixels) the line will take on the framebuffer
	*
	* \param lineWidth Width of the line
	*
	* \remark Invalidates the pipeline
	*
	* \see GetLineWidth
	*/
	inline void MaterialPass::SetLineWidth(float lineWidth)
	{
		m_pipelineInfo.lineWidth = lineWidth;

		InvalidatePipeline();
	}

	inline void MaterialPass::SetOptionValue(std::size_t optionIndex, ShaderAst::ConstantValue value)
	{
		assert(optionIndex < m_optionValues.size());
		if (m_optionValues[optionIndex] != value)
		{
			m_optionValues[optionIndex] = std::move(value);
			InvalidatePipeline();
		}
	}

	/*!
	* \brief Sets the point size for this material
	*
	* This parameter is used when rendering points, to define the size (in pixels) the point will take on the framebuffer
	*
	* \param pointSize Size of the point
	*
	* \remark Invalidates the pipeline
	*
	* \see GetPointSize
	*/
	inline void MaterialPass::SetPointSize(float pointSize)
	{
		m_pipelineInfo.pointSize = pointSize;

		InvalidatePipeline();
	}


	inline void MaterialPass::SetPrimitiveMode(PrimitiveMode mode)
	{
		m_pipelineInfo.primitiveMode = mode;

		InvalidatePipeline();
	}

	inline void MaterialPass::SetTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture)
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		if (m_textures[textureIndex].texture != texture)
		{
			m_textures[textureIndex].texture = std::move(texture);

			InvalidateCommandBuffer();
		}
	}

	inline void MaterialPass::SetTextureSampler(std::size_t textureIndex, TextureSamplerInfo samplerInfo)
	{
		NazaraAssert(textureIndex < m_textures.size(), "Invalid texture index");
		if (m_textures[textureIndex].samplerInfo != samplerInfo)
		{
			m_textures[textureIndex].samplerInfo = std::move(samplerInfo);

			InvalidateTextureSampler(textureIndex);
		}
	}

	inline void MaterialPass::SetUniformBuffer(std::size_t bufferIndex, std::shared_ptr<AbstractBuffer> uniformBuffer)
	{
		NazaraAssert(bufferIndex < m_uniformBuffers.size(), "Invalid shared uniform buffer index");
		if (m_uniformBuffers[bufferIndex].buffer != uniformBuffer)
		{
			m_uniformBuffers[bufferIndex].buffer = std::move(uniformBuffer);
			m_uniformBuffers[bufferIndex].dataInvalidated = true;

			InvalidateCommandBuffer();
		}
	}

	inline void MaterialPass::InvalidateCommandBuffer()
	{
		m_forceCommandBufferRegeneration = true;
		OnMaterialInvalidated(this);
	}

	inline void MaterialPass::InvalidatePipeline()
	{
		m_forceCommandBufferRegeneration = true;
		m_pipelineUpdated = false;

		OnMaterialInvalidated(this);
	}

	inline void MaterialPass::InvalidateTextureSampler(std::size_t textureIndex)
	{
		assert(textureIndex < m_textures.size());
		m_textures[textureIndex].sampler.reset();

		InvalidateCommandBuffer();
	}

	inline void MaterialPass::InvalidateUniformData(std::size_t uniformBufferIndex)
	{
		assert(uniformBufferIndex < m_uniformBuffers.size());
		UniformBuffer& uboEntry = m_uniformBuffers[uniformBufferIndex];
		uboEntry.dataInvalidated = true;

		OnMaterialInvalidated(this);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
