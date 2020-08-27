// Copyright (C) 2020 Jérôme Leclercq, Arnaud Cadot
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	/*!
	* \brief Binds SDK module to Lua
	*/

	void LuaBinding::BindSDK()
	{
		/*********************************** Ndk::Application **********************************/

		#ifndef NDK_SERVER
		//application.SetMethod("AddWindow", &Application::AddWindow);

		application.BindMethod("EnableConsole", &Application::EnableConsole);
		application.BindMethod("EnableFPSCounter", &Application::EnableFPSCounter);

		application.BindMethod("IsConsoleEnabled", &Application::IsConsoleEnabled);
		application.BindMethod("IsFPSCounterEnabled", &Application::IsFPSCounterEnabled);
		#endif

		application.BindMethod("AddWorld", [] (Nz::LuaInstance& lua, Application* instance, std::size_t /*argumentCount*/) -> int
		{
			lua.Push(instance->AddWorld().CreateHandle());
			return 1;
		});

		application.BindMethod("GetUpdateTime", &Application::GetUpdateTime);
		application.BindMethod("Quit", &Application::Quit);

		/*********************************** Ndk::Console **********************************/
		#ifndef NDK_SERVER
		console.Inherit<Nz::Node>(node, [] (ConsoleHandle* handle) -> Nz::Node*
		{
			return handle->GetObject();
		});

		console.BindMethod("AddLine", &Console::AddLine, Nz::Color::White);
		console.BindMethod("Clear", &Console::Clear);
		console.BindMethod("GetCharacterSize", &Console::GetCharacterSize);
		console.BindMethod("GetHistory", &Console::GetHistory);
		console.BindMethod("GetHistoryBackground", &Console::GetHistoryBackground);
		console.BindMethod("GetInput", &Console::GetInput);
		console.BindMethod("GetInputBackground", &Console::GetInputBackground);
		console.BindMethod("GetSize", &Console::GetSize);
		console.BindMethod("GetTextFont", &Console::GetTextFont);

		console.BindMethod("IsVisible", &Console::IsVisible);

		console.BindMethod("SendCharacter", &Console::SendCharacter);
		//consoleClass.SetMethod("SendEvent", &Console::SendEvent);

		console.BindMethod("SetCharacterSize", &Console::SetCharacterSize);
		console.BindMethod("SetSize", &Console::SetSize);
		console.BindMethod("SetTextFont", &Console::SetTextFont);

		console.BindMethod("Show", &Console::Show, true);
		#endif

		/*********************************** Ndk::Entity **********************************/
		entity.BindMethod("Enable", &Entity::Enable, true);
		entity.BindMethod("GetId", &Entity::GetId);
		entity.BindMethod("GetWorld", &Entity::GetWorld);
		entity.BindMethod("Kill", &Entity::Kill);
		entity.BindMethod("IsEnabled", &Entity::IsEnabled);
		entity.BindMethod("IsValid", &Entity::IsValid);
		entity.BindMethod("RemoveAllComponents", &Entity::RemoveAllComponents);
		entity.BindMethod("__tostring", &EntityHandle::ToString);

		entity.BindMethod("AddComponent", [this] (Nz::LuaInstance& instance, EntityHandle& handle, std::size_t /*argumentCount*/) -> int
		{
			ComponentBinding* binding = QueryComponentIndex(instance);

			return binding->adder(instance, handle);
		});

		entity.BindMethod("GetComponent", [this] (Nz::LuaInstance& instance, EntityHandle& handle, std::size_t /*argumentCount*/) -> int
		{
			ComponentBinding* binding = QueryComponentIndex(instance);

			return binding->getter(instance, handle->GetComponent(binding->index));
		});

		entity.BindMethod("RemoveComponent", [this] (Nz::LuaInstance& instance, EntityHandle& handle, std::size_t /*argumentCount*/) -> int
		{
			ComponentBinding* binding = QueryComponentIndex(instance);

			handle->RemoveComponent(binding->index);
			return 0;
		});

		/*********************************** Ndk::NodeComponent **********************************/
		nodeComponent.Inherit<Nz::Node>(node, [] (NodeComponentHandle* handle) -> Nz::Node*
		{
			return handle->GetObject();
		});

		/*********************************** Ndk::VelocityComponent **********************************/
		velocityComponent.SetGetter([] (Nz::LuaInstance& lua, VelocityComponentHandle& instance)
		{
			std::size_t length;
			const char* member = lua.CheckString(2, &length);

			if (std::strcmp(member, "Linear") == 0)
			{
				lua.Push(instance->linearVelocity);
				return true;
			}

			return false;
		});

		velocityComponent.SetSetter([] (Nz::LuaInstance& lua, VelocityComponentHandle& instance)
		{
			std::size_t length;
			const char* member = lua.CheckString(2, &length);

			int argIndex = 3;
			if (std::strcmp(member, "Linear") == 0)
			{
				instance->linearVelocity = lua.Check<Nz::Vector3f>(&argIndex);
				return true;
			}

			return false;
		});

		/*********************************** Ndk::World **********************************/
		world.BindMethod("CreateEntity", &World::CreateEntity);
		world.BindMethod("CreateEntities", &World::CreateEntities);
		world.BindMethod("Clear", &World::Clear);


		#ifndef NDK_SERVER
		/*********************************** Ndk::CameraComponent **********************************/
		cameraComponent.Inherit<Nz::AbstractViewer>(abstractViewer, [] (CameraComponentHandle* handle) -> Nz::AbstractViewer*
		{
			return handle->GetObject();
		});

		cameraComponent.BindMethod("GetFOV", &Ndk::CameraComponent::GetFOV);
		cameraComponent.BindMethod("GetLayer", &Ndk::CameraComponent::GetLayer);

		cameraComponent.BindMethod("SetFOV", &Ndk::CameraComponent::SetFOV);
		cameraComponent.BindMethod("SetLayer", &Ndk::CameraComponent::SetLayer);
		cameraComponent.BindMethod("SetProjectionType", &Ndk::CameraComponent::SetProjectionType);
		cameraComponent.BindMethod("SetSize", (void(Ndk::CameraComponent::*)(const Nz::Vector2f&)) &Ndk::CameraComponent::SetSize);
		//cameraComponent.BindMethod("SetTarget", &Ndk::CameraComponent::SetTarget);
		cameraComponent.BindMethod("SetTargetRegion", &Ndk::CameraComponent::SetTargetRegion);
		cameraComponent.BindMethod("SetViewport", &Ndk::CameraComponent::SetViewport);
		cameraComponent.BindMethod("SetZFar", &Ndk::CameraComponent::SetZFar);
		cameraComponent.BindMethod("SetZNear", &Ndk::CameraComponent::SetZNear);

		/*********************************** Ndk::GraphicsComponent **********************************/
		graphicsComponent.BindMethod("Attach", [] (Nz::LuaInstance& lua, Ndk::GraphicsComponent* instance, std::size_t argumentCount) -> int
		{
			/*
			void Attach(Nz::InstancedRenderableRef renderable, int renderOrder = 0);
			void Attach(Nz::InstancedRenderableRef renderable, const Nz::Matrix4f& localMatrix, int renderOrder = 0);
			*/

			std::size_t argCount = std::min<std::size_t>(argumentCount, 3U);

			switch (argCount)
			{
				case 1:
				{
					int argIndex = 2;
					instance->Attach(lua.Check<Nz::InstancedRenderableRef>(&argIndex));
					return 0;
				}

				case 2:
				{
					int argIndex = 2;
					Nz::InstancedRenderableRef renderable = lua.Check<Nz::InstancedRenderableRef>(&argIndex);

					if (lua.IsOfType(argIndex, Nz::LuaType_Number))
					{
						int renderOrder = lua.Check<int>(&argIndex);

						instance->Attach(renderable, renderOrder);
					}
					else if (lua.IsOfType(argIndex, "Matrix4"))
					{
						Nz::Matrix4f localMatrix = lua.Check<Nz::Matrix4f>(&argIndex);

						instance->Attach(renderable, localMatrix);
					}
					else
						break;

					return 0;
				}

				case 3:
				{
					int argIndex = 2;
					Nz::InstancedRenderableRef renderable = lua.Check<Nz::InstancedRenderableRef>(&argIndex);
					Nz::Matrix4f localMatrix = lua.Check<Nz::Matrix4f>(&argIndex);
					int renderOrder = lua.Check<int>(&argIndex);

					instance->Attach(renderable, localMatrix, renderOrder);
					return 0;
				}
			}

			lua.Error("No matching overload for method GetMemoryUsage");
			return 0;
		});
		#endif


		// Components functions
		m_componentBinding.resize(BaseComponent::GetMaxComponentIndex());

		BindComponent<NodeComponent>("Node");
		BindComponent<VelocityComponent>("Velocity");

		#ifndef NDK_SERVER
		BindComponent<CameraComponent>("Camera");
		BindComponent<GraphicsComponent>("Graphics");
		#endif
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the SDK classes
	*/

	void LuaBinding::RegisterSDK(Nz::LuaInstance& instance)
	{
		// Classes
		application.Register(instance);
		entity.Register(instance);
		nodeComponent.Register(instance);
		velocityComponent.Register(instance);
		world.Register(instance);

		#ifndef NDK_SERVER
		cameraComponent.Register(instance);
		console.Register(instance);
		graphicsComponent.Register(instance);
		#endif

		// Enums

		// ComponentType (fake enumeration to expose component indexes)
		instance.PushTable(0, m_componentBinding.size());
		{
			for (const ComponentBinding& entry : m_componentBinding)
			{
				if (entry.name.IsEmpty())
					continue;

				instance.PushField(entry.name, entry.index);
			}
		}
		instance.SetGlobal("ComponentType");
	}

	/*!
	* \brief Gets the index of the component
	* \return A pointer to the binding linked to a component
	*
	* \param instance Lua instance that will interact with the component
	* \param argIndex Index of the component
	*/

	LuaBinding::ComponentBinding* LuaBinding::QueryComponentIndex(Nz::LuaInstance& instance, int argIndex)
	{
		switch (instance.GetType(argIndex))
		{
			case Nz::LuaType_Number:
			{
				ComponentIndex componentIndex = instance.Check<ComponentIndex>(&argIndex);
				if (componentIndex > m_componentBinding.size())
				{
					instance.Error("Invalid component index");
					return nullptr;
				}

				ComponentBinding& binding = m_componentBinding[componentIndex];
				if (binding.name.IsEmpty())
				{
					instance.Error("Invalid component index");
					return nullptr;
				}

				return &binding;
			}

			case Nz::LuaType_String:
			{
				const char* key = instance.CheckString(argIndex);
				auto it = m_componentBindingByName.find(key);
				if (it == m_componentBindingByName.end())
				{
					instance.Error("Invalid component name");
					return nullptr;
				}

				return &m_componentBinding[it->second];
			}

			default:
				break;
		}

		instance.Error("Invalid component index at #" + Nz::String::Number(argIndex));
		return nullptr;
	}
}
