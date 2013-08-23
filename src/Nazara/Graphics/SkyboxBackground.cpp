// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_RENDERER_OPENGL
#define NAZARA_RENDERER_OPENGL // Nécessaire pour inclure les headers OpenGL
#endif

#include <Nazara/Graphics/SkyboxBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	NzIndexBuffer* BuildIndexBuffer()
	{
		std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(false, 36, nzBufferStorage_Hardware, nzBufferUsage_Static));
		indexBuffer->SetPersistent(false);

		nzUInt16 indices[6*6] =
		{
			0, 1, 2, 0, 2, 3,
			3, 2, 6, 3, 6, 7,
			7, 6, 5, 7, 5, 4,
			4, 5, 1, 4, 1, 0,
			0, 3, 7, 0, 7, 4,
			1, 6, 2, 1, 5, 6
		};

		if (!indexBuffer->Fill(indices, 0, 36))
		{
			NazaraError("Failed to create index buffer");
			return nullptr;
		}

		return indexBuffer.release();
	}

	NzShaderProgram* BuildProgram()
	{
		const char* fragmentSource110 =
		"#version 110\n"
		"varying vec3 vTexCoord;\n"
		"uniform samplerCube Skybox;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = textureCube(Skybox, vTexCoord);\n"
		"}\n";

		const char* fragmentSource140 =
		"#version 140\n"
		"in vec3 vTexCoord;\n"
		"out vec4 RenderTarget0;\n"
		"uniform samplerCube Skybox;\n"
		"void main()\n"
		"{\n"
		"	RenderTarget0 = texture(Skybox, vTexCoord);\n"
		"}\n";

		const char* vertexSource110 =
		"#version 110\n"
		"attribute vec3 VertexPosition;\n"
		"varying vec3 vTexCoord;\n"
		"uniform mat4 WorldViewProjMatrix;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n"
		"    vTexCoord = vec3(VertexPosition.x, -VertexPosition.y, -VertexPosition.z);\n"
		"}\n";

		const char* vertexSource140 =
		"#version 140\n"
		"in vec3 VertexPosition;\n"
		"out vec3 vTexCoord;\n"
		"uniform mat4 WorldViewProjMatrix;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n"
		"    vTexCoord = vec3(VertexPosition.x, -VertexPosition.y, -VertexPosition.z);\n"
		"}\n";

		///TODO: Remplacer ça par des ShaderNode
		std::unique_ptr<NzShaderProgram> program(new NzShaderProgram(nzShaderLanguage_GLSL));
		program->SetPersistent(false);

		bool useGLSL140 = (NzOpenGL::GetVersion() >= 310);

		if (!program->LoadShader(nzShaderType_Fragment, (useGLSL140) ? fragmentSource140 : fragmentSource110))
		{
			NazaraError("Failed to load fragment shader");
			return nullptr;
		}

		if (!program->LoadShader(nzShaderType_Vertex, (useGLSL140) ? vertexSource140 : vertexSource110))
		{
			NazaraError("Failed to load vertex shader");
			return nullptr;
		}

		if (!program->Compile())
		{
			NazaraError("Failed to compile program");
			return nullptr;
		}

		return program.release();
	}

	NzRenderStates BuildRenderStates()
	{
		NzRenderStates states;
		states.parameters[nzRendererParameter_DepthBuffer] = false;
		states.parameters[nzRendererParameter_FaceCulling] = true;
		states.faceCulling = nzFaceCulling_Front;

		return states;
	}

	NzVertexBuffer* BuildVertexBuffer()
	{
		std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(NzVertexDeclaration::Get(nzVertexLayout_XYZ), 8, nzBufferStorage_Hardware, nzBufferUsage_Static));
		vertexBuffer->SetPersistent(false);

		float vertices[8*(sizeof(float)*3)] =
		{
			-1.0,  1.0,  1.0,
			-1.0, -1.0,  1.0,
			 1.0, -1.0,  1.0,
			 1.0,  1.0,  1.0,
			-1.0,  1.0, -1.0,
			-1.0, -1.0, -1.0,
			 1.0, -1.0, -1.0,
			 1.0,  1.0, -1.0,
		};

		if (!vertexBuffer->Fill(vertices, 0, 8))
		{
			NazaraError("Failed to create vertex buffer");
			return nullptr;
		}

		return vertexBuffer.release();
	}

	static NzIndexBuffer* s_indexBuffer = nullptr;
	static NzShaderProgram* s_program = nullptr;
	static NzVertexBuffer* s_vertexBuffer = nullptr;
	static unsigned int s_skyboxLocation;
}

NzSkyboxBackground::NzSkyboxBackground()
{
	if (!s_indexBuffer)
		s_indexBuffer = BuildIndexBuffer();

	if (!s_program)
	{
		s_program = BuildProgram();
		s_skyboxLocation = s_program->GetUniformLocation("Skybox");
	}

	if (!s_vertexBuffer)
		s_vertexBuffer = BuildVertexBuffer();

	m_indexBuffer = s_indexBuffer;
	m_program = s_program;
	m_sampler.SetWrapMode(nzSamplerWrap_Clamp); // Nécessaire pour ne pas voir les côtés
	m_vertexBuffer = s_vertexBuffer;
}

NzSkyboxBackground::NzSkyboxBackground(NzTexture* cubemapTexture) :
NzSkyboxBackground()
{
	SetTexture(cubemapTexture);
}

NzSkyboxBackground::~NzSkyboxBackground()
{
	if (m_indexBuffer.Reset())
		s_indexBuffer = nullptr;

	if (m_program.Reset())
		s_program = nullptr;

	if (m_vertexBuffer.Reset())
		s_vertexBuffer = nullptr;
}

void NzSkyboxBackground::Draw(const NzScene* scene) const
{
	static NzRenderStates states(BuildRenderStates());

	s_program->SendInteger(s_skyboxLocation, 0);

	NzAbstractViewer* viewer = scene->GetViewer();

	NzMatrix4f skyboxMatrix(viewer->GetViewMatrix());
	skyboxMatrix.SetTranslation(NzVector3f::Zero());

	NzRenderer::SetIndexBuffer(m_indexBuffer);
	NzRenderer::SetMatrix(nzMatrixType_Projection, viewer->GetProjectionMatrix());
	NzRenderer::SetMatrix(nzMatrixType_View, skyboxMatrix);
	NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Scale(NzVector3f(viewer->GetZNear())));
	NzRenderer::SetRenderStates(states);
	NzRenderer::SetShaderProgram(s_program);
	NzRenderer::SetTexture(0, m_texture);
	NzRenderer::SetTextureSampler(0, m_sampler);
	NzRenderer::SetVertexBuffer(m_vertexBuffer);

	NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, 36);
}

nzBackgroundType NzSkyboxBackground::GetBackgroundType() const
{
	return nzBackgroundType_Skybox;
}

NzTexture* NzSkyboxBackground::GetTexture() const
{
	return m_texture;
}

void NzSkyboxBackground::SetTexture(NzTexture* cubemapTexture)
{
	#if NAZARA_GRAPHICS_SAFE
	if (cubemapTexture)
	{
		if (!cubemapTexture->IsValid())
		{
			NazaraError("Texture must be valid");
			return;
		}

		if (!cubemapTexture->IsCubemap())
		{
			NazaraError("Texture must be a cubemap");
			return;
		}
	}
	#endif

	m_texture = cubemapTexture;
}
