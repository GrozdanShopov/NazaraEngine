// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzRenderTarget::~NzRenderTarget() = default;

bool NzRenderTarget::IsActive() const
{
	return NzRenderer::GetTarget() == this;
}

bool NzRenderTarget::SetActive(bool active)
{
	if (active)
		return NzRenderer::SetTarget(this);
	else if (NzRenderer::GetTarget() == this)
		return NzRenderer::SetTarget(nullptr);

	return true;
}

void NzRenderTarget::Desactivate()
{
	// Seuls les target sans contextes (ex: NzRenderTexture) n�cessitent une d�sactivation
}
