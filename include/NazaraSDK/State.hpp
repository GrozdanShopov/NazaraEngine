// Copyright (C) 2020 J�r�me Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_STATE_HPP
#define NDK_STATE_HPP

#include <NazaraSDK/Prerequisites.hpp>

namespace Ndk
{
	class StateMachine;

	class NDK_API State
	{
		public:
			State() = default;
			virtual ~State();

			virtual void Enter(StateMachine& fsm) = 0;
			virtual void Leave(StateMachine& fsm) = 0;
			virtual bool Update(StateMachine& fsm, float elapsedTime) = 0;
	};
}

#endif // NDK_STATE_HPP