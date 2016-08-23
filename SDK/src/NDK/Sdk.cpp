// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Sdk.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Lua/Lua.hpp>
#include <Nazara/Noise/Noise.hpp>
#include <Nazara/Physics/Physics.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <NDK/Algorithm.hpp>
#include <NDK/BaseSystem.hpp>
#include <NDK/Components/CollisionComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent.hpp>
#include <NDK/Components/VelocityComponent.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>
#include <NDK/Systems/VelocitySystem.hpp>

#ifndef NDK_SERVER
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/LightComponent.hpp>
#include <NDK/Components/ListenerComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/ParticleEmitterComponent.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>
#include <NDK/Systems/ParticleSystem.hpp>
#include <NDK/Systems/ListenerSystem.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#endif

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::Sdk
	* \brief NDK class that represents the software development kit, a set of tools made to ease the conception of application
	*/

	/*!
	* \brief Initializes the Sdk module
	* \return true if initialization is successful
	*
	* \remark Produces a NazaraNotice
	*/

	bool Sdk::Initialize()
	{
		if (s_referenceCounter++ > 0)
			return true; // Already initialized

		try
		{
			Nz::ErrorFlags errFlags(Nz::ErrorFlag_ThrowException, true);

			// Initialize the engine first

			// Shared modules
			Nz::Lua::Initialize();
			Nz::Noise::Initialize();
			Nz::Physics::Initialize();
			Nz::Utility::Initialize();

			#ifndef NDK_SERVER
			// Client modules
			Nz::Audio::Initialize();
			Nz::Graphics::Initialize();
			#endif

			// SDK Initialization

			// Components
			BaseComponent::Initialize();

			// Shared components
			InitializeComponent<CollisionComponent>("NdkColli");
			InitializeComponent<NodeComponent>("NdkNode");
			InitializeComponent<PhysicsComponent>("NdkPhys");
			InitializeComponent<VelocityComponent>("NdkVeloc");

			#ifndef NDK_SERVER
			// Client components
			InitializeComponent<CameraComponent>("NdkCam");
			InitializeComponent<LightComponent>("NdkLight");
			InitializeComponent<ListenerComponent>("NdkList");
			InitializeComponent<GraphicsComponent>("NdkGfx");
			InitializeComponent<ParticleEmitterComponent>("NdkPaEmi");
			InitializeComponent<ParticleGroupComponent>("NdkPaGrp");
			#endif

			// Systems

			BaseSystem::Initialize();

			// Shared systems
			InitializeSystem<PhysicsSystem>();
			InitializeSystem<VelocitySystem>();

			#ifndef NDK_SERVER
			// Client systems
			InitializeSystem<ListenerSystem>();
			InitializeSystem<ParticleSystem>();
			InitializeSystem<RenderSystem>();
			#endif

			NazaraNotice("Initialized: SDK");
			return true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialize NDK: " + Nz::String(e.what()));

			return false;
		}
	}

	/*!
	* \brief Uninitializes the Sdk module
	*
	* \remark Produces a NazaraNotice
	*/

	void Sdk::Uninitialize()
	{
		if (s_referenceCounter != 1)
		{
			// Either the module is not initialized, either it was initialized multiple times
			if (s_referenceCounter > 1)
				s_referenceCounter--;

			return;
		}

		// Uninitialize the SDK
		s_referenceCounter = 0;

		// Components
		BaseComponent::Uninitialize();

		// Systems
		BaseSystem::Uninitialize();

		// Uninitialize the engine

		#ifndef NDK_SERVER
		// Client modules
		Nz::Audio::Uninitialize();
		Nz::Graphics::Uninitialize();
		#endif

		// Shared modules
		Nz::Lua::Uninitialize();
		Nz::Noise::Uninitialize();
		Nz::Physics::Uninitialize();
		Nz::Utility::Uninitialize();

		NazaraNotice("Uninitialized: SDK");
	}

	unsigned int Sdk::s_referenceCounter = 0;
}
