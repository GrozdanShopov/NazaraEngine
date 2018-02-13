// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_LUABINDING_BASE_HPP
#define NDK_LUABINDING_BASE_HPP

#include <Nazara/Lua/LuaClass.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <NDK/Prerequisites.hpp>

namespace Ndk
{
	class LuaBinding;

	class LuaBinding_Audio;
	class LuaBinding_Core;
	class LuaBinding_Graphics;
	class LuaBinding_Math;
	class LuaBinding_Network;
	class LuaBinding_Renderer;
	class LuaBinding_SDK;
	class LuaBinding_Utility;
	class LuaBinding_Platform;

	class NDK_API LuaBinding_Base
	{
		public:
			LuaBinding_Base(LuaBinding& binding);
			virtual ~LuaBinding_Base();

			virtual void Register(Nz::LuaState& state) = 0;

			// Implementation lies in the respective .cpp files (still searching for a cleaner way..)
			static std::unique_ptr<LuaBinding_Base> BindCore(LuaBinding& binding);
			static std::unique_ptr<LuaBinding_Base> BindMath(LuaBinding& binding);
			static std::unique_ptr<LuaBinding_Base> BindNetwork(LuaBinding& binding);
			static std::unique_ptr<LuaBinding_Base> BindSDK(LuaBinding& binding);
			static std::unique_ptr<LuaBinding_Base> BindUtility(LuaBinding& binding);

			#ifndef NDK_SERVER
			static std::unique_ptr<LuaBinding_Base> BindAudio(LuaBinding& binding);
			static std::unique_ptr<LuaBinding_Base> BindGraphics(LuaBinding& binding);
			static std::unique_ptr<LuaBinding_Base> BindRenderer(LuaBinding& binding);
			static std::unique_ptr<LuaBinding_Base> BindPlatform(LuaBinding& binding);
			#endif

		protected:
			LuaBinding& m_binding;
	};
}

#endif // NDK_LUABINDING_BASE_HPP
