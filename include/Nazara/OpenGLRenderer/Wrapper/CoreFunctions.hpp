// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef	NAZARA_OPENGLRENDERER_COREFUNCTIONS_HPP
#define NAZARA_OPENGLRENDERER_COREFUNCTIONS_HPP

#define GL_GLES_PROTOTYPES 0
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

// Define some OpenGL (not ES) extensions
#define GL_SHADER_BINARY_FORMAT_SPIR_V_ARB 0x9551
#define GL_SPIR_V_BINARY_ARB               0x9552
typedef void (GL_APIENTRYP PFNGLSPECIALIZESHADERARBPROC) (GLuint shader, const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue);

// OpenGL core
#define NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(cb, extCb) \
	cb(glActiveTexture, PFNGLACTIVETEXTUREPROC) \
	cb(glAttachShader, PFNGLATTACHSHADERPROC) \
	cb(glBeginQuery, PFNGLBEGINQUERYPROC) \
	cb(glBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC) \
	cb(glBindBuffer, PFNGLBINDBUFFERPROC) \
	cb(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC) \
	cb(glBindRenderbuffer, PFNGLBINDRENDERBUFFERPROC) \
	cb(glBindSampler, PFNGLBINDSAMPLERPROC) \
	cb(glBindTexture, PFNGLBINDTEXTUREPROC) \
	cb(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC) \
	cb(glBlendFunc, PFNGLBLENDFUNCPROC) \
	cb(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC) \
	cb(glBlitFramebuffer, PFNGLBLITFRAMEBUFFERPROC) \
	cb(glBufferData, PFNGLBUFFERDATAPROC) \
	cb(glBufferSubData, PFNGLBUFFERSUBDATAPROC) \
	cb(glClear, PFNGLCLEARPROC) \
	cb(glClearColor, PFNGLCLEARCOLORPROC) \
	cb(glClearDepthf, PFNGLCLEARDEPTHFPROC) \
	cb(glClearStencil, PFNGLCLEARSTENCILPROC) \
	cb(glCreateProgram, PFNGLCREATEPROGRAMPROC) \
	cb(glCreateShader, PFNGLCREATESHADERPROC) \
	cb(glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC) \
	cb(glColorMask, PFNGLCOLORMASKPROC) \
	cb(glCompressedTexSubImage2D, PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) \
	cb(glCompressedTexSubImage3D, PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) \
	cb(glCullFace, PFNGLCULLFACEPROC) \
	cb(glCompileShader, PFNGLCOMPILESHADERPROC) \
	cb(glCopyTexSubImage2D, PFNGLCOPYTEXSUBIMAGE2DPROC) \
	cb(glDeleteBuffers, PFNGLDELETEBUFFERSPROC) \
	cb(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC) \
	cb(glDeleteProgram, PFNGLDELETEPROGRAMPROC) \
	cb(glDeleteQueries, PFNGLDELETEQUERIESPROC) \
	cb(glDeleteRenderbuffers, PFNGLDELETERENDERBUFFERSPROC) \
	cb(glDeleteSamplers, PFNGLDELETESAMPLERSPROC) \
	cb(glDeleteShader, PFNGLDELETESHADERPROC) \
	cb(glDeleteTextures, PFNGLDELETETEXTURESPROC) \
	cb(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC) \
	cb(glDepthFunc, PFNGLDEPTHFUNCPROC) \
	cb(glDepthMask, PFNGLDEPTHMASKPROC) \
	cb(glDisable, PFNGLDISABLEPROC) \
	cb(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC) \
	cb(glDrawArrays, PFNGLDRAWARRAYSPROC) \
	cb(glDrawArraysInstanced, PFNGLDRAWARRAYSINSTANCEDPROC) \
	cb(glDrawBuffers, PFNGLDRAWBUFFERSPROC) \
	cb(glDrawElements, PFNGLDRAWELEMENTSPROC) \
	cb(glDrawElementsInstanced, PFNGLDRAWELEMENTSINSTANCEDPROC) \
	cb(glEnable, PFNGLENABLEPROC) \
	cb(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC) \
	cb(glEndQuery, PFNGLENDQUERYPROC) \
	cb(glFlush, PFNGLFLUSHPROC) \
	cb(glFramebufferRenderbuffer, PFNGLFRAMEBUFFERRENDERBUFFERPROC) \
	cb(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC) \
	cb(glFramebufferTextureLayer, PFNGLFRAMEBUFFERTEXTURELAYERPROC) \
	cb(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC) \
	cb(glGenBuffers, PFNGLGENBUFFERSPROC) \
	cb(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC) \
	cb(glGenRenderbuffers, PFNGLGENRENDERBUFFERSPROC) \
	cb(glGenQueries, PFNGLGENQUERIESPROC) \
	cb(glGenSamplers, PFNGLGENSAMPLERSPROC) \
	cb(glGenTextures, PFNGLGENTEXTURESPROC) \
	cb(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC) \
	cb(glGetActiveUniform, PFNGLGETACTIVEUNIFORMPROC) \
	cb(glGetBooleanv, PFNGLGETBOOLEANVPROC) \
	cb(glGetBufferParameteriv, PFNGLGETBUFFERPARAMETERIVPROC) \
	cb(glGetError, PFNGLGETERRORPROC) \
	cb(glGetFloatv, PFNGLGETFLOATVPROC) \
	cb(glGetIntegerv, PFNGLGETINTEGERVPROC) \
	cb(glGetProgramBinary, PFNGLGETPROGRAMBINARYPROC) \
	cb(glGetProgramiv, PFNGLGETPROGRAMIVPROC) \
	cb(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC) \
	cb(glGetQueryiv, PFNGLGETQUERYIVPROC) \
	cb(glGetQueryObjectuiv, PFNGLGETQUERYOBJECTUIVPROC) \
	cb(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC) \
	cb(glGetShaderiv, PFNGLGETSHADERIVPROC) \
	cb(glGetShaderSource, PFNGLGETSHADERSOURCEPROC) \
	cb(glGetString, PFNGLGETSTRINGPROC) \
	cb(glGetStringi, PFNGLGETSTRINGIPROC) \
	cb(glGetTexParameterfv, PFNGLGETTEXPARAMETERFVPROC) \
	cb(glGetTexParameteriv, PFNGLGETTEXPARAMETERIVPROC) \
	cb(glGetUniformfv, PFNGLGETUNIFORMFVPROC) \
	cb(glGetUniformiv, PFNGLGETUNIFORMIVPROC) \
	cb(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC) \
	cb(glIsEnabled, PFNGLISENABLEDPROC) \
	cb(glLineWidth, PFNGLLINEWIDTHPROC) \
	cb(glLinkProgram, PFNGLLINKPROGRAMPROC) \
	cb(glMapBufferRange, PFNGLMAPBUFFERRANGEPROC) \
	cb(glPixelStorei, PFNGLPIXELSTOREIPROC) \
	cb(glProgramBinary, PFNGLPROGRAMBINARYPROC) \
	cb(glProgramParameteri, PFNGLPROGRAMPARAMETERIPROC) \
	cb(glReadPixels, PFNGLREADPIXELSPROC) \
	cb(glRenderbufferStorage, PFNGLRENDERBUFFERSTORAGEPROC) \
	cb(glSamplerParameterf, PFNGLSAMPLERPARAMETERFPROC) \
	cb(glSamplerParameteri, PFNGLSAMPLERPARAMETERIPROC) \
	cb(glScissor, PFNGLSCISSORPROC) \
	cb(glShaderBinary, PFNGLSHADERBINARYPROC) \
	cb(glShaderSource, PFNGLSHADERSOURCEPROC) \
	cb(glStencilFunc, PFNGLSTENCILFUNCPROC) \
	cb(glStencilFuncSeparate, PFNGLSTENCILFUNCSEPARATEPROC) \
	cb(glStencilOp, PFNGLSTENCILOPPROC) \
	cb(glStencilOpSeparate, PFNGLSTENCILOPSEPARATEPROC) \
	cb(glTexImage2D, PFNGLTEXIMAGE2DPROC) \
	cb(glTexImage3D, PFNGLTEXIMAGE3DPROC) \
	cb(glTexParameterf, PFNGLTEXPARAMETERFPROC) \
	cb(glTexParameteri, PFNGLTEXPARAMETERIPROC) \
	cb(glTexStorage2D, PFNGLTEXSTORAGE2DPROC) \
	cb(glTexStorage3D, PFNGLTEXSTORAGE3DPROC) \
	cb(glTexSubImage2D, PFNGLTEXSUBIMAGE2DPROC) \
	cb(glTexSubImage3D, PFNGLTEXSUBIMAGE3DPROC) \
	cb(glUniform1f, PFNGLUNIFORM1FPROC) \
	cb(glUniform1i, PFNGLUNIFORM1IPROC) \
	cb(glUniform1fv, PFNGLUNIFORM1FVPROC) \
	cb(glUniform1iv, PFNGLUNIFORM1IVPROC) \
	cb(glUniform2fv, PFNGLUNIFORM2FVPROC) \
	cb(glUniform2iv, PFNGLUNIFORM2IVPROC) \
	cb(glUniform3fv, PFNGLUNIFORM3FVPROC) \
	cb(glUniform3iv, PFNGLUNIFORM3IVPROC) \
	cb(glUniform4fv, PFNGLUNIFORM4FVPROC) \
	cb(glUniform4iv, PFNGLUNIFORM4IVPROC) \
	cb(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC) \
	cb(glUnmapBuffer, PFNGLUNMAPBUFFERPROC) \
	cb(glUseProgram, PFNGLUSEPROGRAMPROC) \
	cb(glValidateProgram, PFNGLVALIDATEPROGRAMPROC) \
	cb(glVertexAttrib4f, PFNGLVERTEXATTRIB4FPROC) \
	cb(glVertexAttribDivisor, PFNGLVERTEXATTRIBDIVISORPROC) \
	cb(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC) \
	cb(glVertexAttribIPointer, PFNGLVERTEXATTRIBIPOINTERPROC) \
	cb(glViewport, PFNGLVIEWPORTPROC) \
	\
	extCb(glDebugMessageCallback, PFNGLDEBUGMESSAGECALLBACKPROC) \
	\
	extCb(glSpecializeShaderARB, PFNGLSPECIALIZESHADERARBPROC) \

#endif
