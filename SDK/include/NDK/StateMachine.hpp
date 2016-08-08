// Copyright (C) 2015 J�r�me Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_STATEMACHINE_HPP
#define NDK_STATEMACHINE_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/State.hpp>
#include <memory>

namespace Ndk
{
	class StateMachine
	{
		public:
			inline StateMachine(std::shared_ptr<State> originalState);
			StateMachine(const StateMachine&) = delete;
			inline StateMachine(StateMachine&& fsm) = default;
			inline ~StateMachine();

			inline void ChangeState(std::shared_ptr<State> state);

			inline const std::shared_ptr<State>& GetCurrentState() const;

			inline bool Update(float elapsedTime);

			inline StateMachine& operator=(StateMachine&& fsm) = default;
			StateMachine& operator=(const StateMachine&) = delete;

		private:
			std::shared_ptr<State> m_currentState;
			std::shared_ptr<State> m_nextState;
	};
}

#include <NDK/StateMachine.inl>

#endif // NDK_STATEMACHINE_HPP