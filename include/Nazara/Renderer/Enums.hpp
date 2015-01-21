// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_RENDERER_HPP
#define NAZARA_ENUMS_RENDERER_HPP

enum nzAttachmentPoint
{
	nzAttachmentPoint_Color,
	nzAttachmentPoint_Depth,
	nzAttachmentPoint_DepthStencil,
	nzAttachmentPoint_Stencil,

	nzAttachmentPoint_Max = nzAttachmentPoint_Stencil
};

enum nzBlendFunc
{
	nzBlendFunc_DestAlpha,
	nzBlendFunc_DestColor,
	nzBlendFunc_SrcAlpha,
	nzBlendFunc_SrcColor,
	nzBlendFunc_InvDestAlpha,
	nzBlendFunc_InvDestColor,
	nzBlendFunc_InvSrcAlpha,
	nzBlendFunc_InvSrcColor,
	nzBlendFunc_One,
	nzBlendFunc_Zero,

	nzBlendFunc_Max = nzBlendFunc_Zero
};

enum nzFaceFilling
{
	nzFaceFilling_Fill,
	nzFaceFilling_Line,
	nzFaceFilling_Point,

	nzFaceFilling_Max = nzFaceFilling_Point
};

enum nzFaceSide
{
	nzFaceSide_Back,
	nzFaceSide_Front,
	nzFaceSide_FrontAndBack,

	nzFaceSide_Max = nzFaceSide_FrontAndBack
};

enum nzGpuQueryCondition
{
	nzGpuQueryCondition_Region_NoWait,
	nzGpuQueryCondition_Region_Wait,
	nzGpuQueryCondition_NoWait,
	nzGpuQueryCondition_Wait,

	nzGpuQueryCondition_Max = nzGpuQueryCondition_Wait
};

enum nzGpuQueryMode
{
	nzGpuQueryMode_AnySamplesPassed,
	nzGpuQueryMode_AnySamplesPassedConservative,
	nzGpuQueryMode_PrimitiveGenerated,
	nzGpuQueryMode_SamplesPassed,
	nzGpuQueryMode_TimeElapsed,
	nzGpuQueryMode_TransformFeedbackPrimitivesWritten,

	nzGpuQueryMode_Max = nzGpuQueryMode_TransformFeedbackPrimitivesWritten
};

enum nzMatrixType
{
	// Matrices de base
	nzMatrixType_Projection,
	nzMatrixType_View,
	nzMatrixType_World,

	// Matrices combinées
	nzMatrixType_ViewProj,
	nzMatrixType_WorldView,
	nzMatrixType_WorldViewProj,

	// Matrice inversées
	nzMatrixType_InvProjection,
	nzMatrixType_InvView,
	nzMatrixType_InvViewProj,
	nzMatrixType_InvWorld,
	nzMatrixType_InvWorldView,
	nzMatrixType_InvWorldViewProj,

	nzMatrixType_Max = nzMatrixType_InvWorldViewProj
};

enum nzPixelBufferType
{
	nzPixelBufferType_Pack,
	nzPixelBufferType_Unpack,

	nzPixelBufferType_Max = nzPixelBufferType_Unpack
};

enum nzRendererCap
{
	nzRendererCap_AnisotropicFilter,
	nzRendererCap_ConditionalRendering,
	nzRendererCap_FP64,
	nzRendererCap_HardwareBuffer,
	nzRendererCap_Instancing,
	nzRendererCap_MultipleRenderTargets,
	nzRendererCap_OcclusionQuery,
	nzRendererCap_PixelBufferObject,
	nzRendererCap_RenderTexture,
	nzRendererCap_Texture3D,
	nzRendererCap_TextureCubemap,
	nzRendererCap_TextureMulti,
	nzRendererCap_TextureNPOT,

	nzRendererCap_Max = nzRendererCap_TextureNPOT
};

enum nzRendererBufferFlags
{
	nzRendererBuffer_Color   = 0x1,
	nzRendererBuffer_Depth   = 0x2,
	nzRendererBuffer_Stencil = 0x4,

	nzRendererBuffer_Max = nzRendererBuffer_Stencil*2-1
};

enum nzRendererComparison
{
	nzRendererComparison_Always,
	nzRendererComparison_Equal,
	nzRendererComparison_Greater,
	nzRendererComparison_GreaterOrEqual,
	nzRendererComparison_Less,
	nzRendererComparison_LessOrEqual,
	nzRendererComparison_Never,
	nzRendererComparison_NotEqual,

	nzRendererComparison_Max = nzRendererComparison_NotEqual
};

enum nzRendererParameter
{
	nzRendererParameter_Blend,
	nzRendererParameter_ColorWrite,
	nzRendererParameter_DepthBuffer,
	nzRendererParameter_DepthWrite,
	nzRendererParameter_FaceCulling,
	nzRendererParameter_ScissorTest,
	nzRendererParameter_StencilTest,

	nzRendererParameter_Max = nzRendererParameter_StencilTest
};

enum nzSamplerFilter
{
	nzSamplerFilter_Unknown = -1,

	nzSamplerFilter_Bilinear,
	nzSamplerFilter_Nearest,
	nzSamplerFilter_Trilinear,

	nzSamplerFilter_Default,

	nzSamplerFilter_Max = nzSamplerFilter_Default
};

enum nzSamplerWrap
{
	nzSamplerWrap_Unknown = -1,

	nzSamplerWrap_Clamp,
	nzSamplerWrap_MirroredRepeat,
	nzSamplerWrap_Repeat,

	nzSamplerWrap_Default,

	nzSamplerWrap_Max = nzSamplerWrap_Repeat
};

enum nzShaderUniform
{
	nzShaderUniform_InvProjMatrix,
	nzShaderUniform_InvTargetSize,
	nzShaderUniform_InvViewMatrix,
	nzShaderUniform_InvViewProjMatrix,
	nzShaderUniform_InvWorldMatrix,
	nzShaderUniform_InvWorldViewMatrix,
	nzShaderUniform_InvWorldViewProjMatrix,
	nzShaderUniform_ProjMatrix,
	nzShaderUniform_TargetSize,
	nzShaderUniform_ViewMatrix,
	nzShaderUniform_ViewProjMatrix,
	nzShaderUniform_WorldMatrix,
	nzShaderUniform_WorldViewMatrix,
	nzShaderUniform_WorldViewProjMatrix,

	nzShaderUniform_Max = nzShaderUniform_WorldViewProjMatrix
};

enum nzShaderStage
{
	nzShaderStage_Fragment,
	nzShaderStage_Geometry,
	nzShaderStage_Vertex,

	nzShaderStage_Max = nzShaderStage_Vertex
};

enum nzStencilOperation
{
	nzStencilOperation_Decrement,
	nzStencilOperation_DecrementNoClamp,
	nzStencilOperation_Increment,
	nzStencilOperation_IncrementNoClamp,
	nzStencilOperation_Invert,
	nzStencilOperation_Keep,
	nzStencilOperation_Replace,
	nzStencilOperation_Zero,

	nzStencilOperation_Max = nzStencilOperation_Zero
};

#endif // NAZARA_ENUMS_RENDERER_HPP
