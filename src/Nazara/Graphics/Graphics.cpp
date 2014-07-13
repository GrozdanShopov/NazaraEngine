// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Graphics/Loaders/Mesh.hpp>
#include <Nazara/Graphics/Loaders/OBJ.hpp>
#include <Nazara/Graphics/Loaders/Texture.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

bool NzGraphics::Initialize()
{
	if (s_moduleReferenceCounter > 0)
	{
		s_moduleReferenceCounter++;
		return true; // Déjà initialisé
	}

	// Initialisation des dépendances
	if (!NzRenderer::Initialize())
	{
		NazaraError("Failed to initialize Renderer module");
		return false;
	}

	s_moduleReferenceCounter++;

	// Initialisation du module
	NzCallOnExit onExit(NzGraphics::Uninitialize);

	if (!NzMaterial::Initialize())
	{
		NazaraError("Failed to initialize materials");
		return false;
	}

	if (!NzSkinningManager::Initialize())
	{
		NazaraError("Failed to initialize skinning manager");
		return false;
	}

	// Loaders
	NzLoaders_OBJ_Register();

	// Loaders génériques
	NzLoaders_Mesh_Register();
	NzLoaders_Texture_Register();

	// RenderTechniques
	NzRenderTechniques::Register(NzRenderTechniques::ToString(nzRenderTechniqueType_BasicForward), 0, []() -> NzAbstractRenderTechnique* { return new NzForwardRenderTechnique; });

	if (NzDeferredRenderTechnique::IsSupported())
	{
		NzDeferredRenderTechnique::Initialize();
		NzRenderTechniques::Register(NzRenderTechniques::ToString(nzRenderTechniqueType_DeferredShading), 20, []() -> NzAbstractRenderTechnique* { return new NzDeferredRenderTechnique; });
	}

	onExit.Reset();

	NazaraNotice("Initialized: Graphics module");
	return true;
}

bool NzGraphics::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzGraphics::Uninitialize()
{
	if (s_moduleReferenceCounter != 1)
	{
		// Le module est soit encore utilisé, soit pas initialisé
		if (s_moduleReferenceCounter > 1)
			s_moduleReferenceCounter--;

		return;
	}

	// Libération du module
	s_moduleReferenceCounter = 0;

	// Loaders
	NzLoaders_Mesh_Unregister();
	NzLoaders_OBJ_Unregister();
	NzLoaders_Texture_Unregister();

	NzDeferredRenderTechnique::Uninitialize();
	NzMaterial::Uninitialize();
	NzSkinningManager::Uninitialize();

	NazaraNotice("Uninitialized: Graphics module");

	// Libération des dépendances
	NzRenderer::Uninitialize();
}

unsigned int NzGraphics::s_moduleReferenceCounter = 0;
