#include "WindowModificationState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NazaraSDK/StateMachine.hpp>

WindowModificationState::WindowModificationState(StateContext& context) :
BaseState(context)
{
}

void WindowModificationState::Enter(Ndk::StateMachine& fsm)
{
	BaseState::Enter(fsm);

	Nz::EventHandler& eventHandler = m_context.window.GetEventHandler();
	m_keyPressedSlot.Connect(eventHandler.OnKeyPressed, [&] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& key)
	{
		if (key.virtualKey == Nz::Keyboard::VKey::M && key.shift)
		{
			fsm.ChangeState(StateFactory::Get(EventStatus::Menu));
		}
	});

	m_movedSlot.Connect(eventHandler.OnMoved, [&] (const Nz::EventHandler*, const Nz::WindowEvent::PositionEvent& event)
	{
		m_text.SetContent("Position(" + Nz::String::Number(event.x) + ", " + Nz::String::Number(event.y) + ")\nM for Menu");
	});

	m_resizedSlot.Connect(eventHandler.OnResized, [&] (const Nz::EventHandler*, const Nz::WindowEvent::SizeEvent& event)
	{
		m_text.SetContent("Size(" + Nz::String::Number(event.width) + ", " + Nz::String::Number(event.height) + ")\nM for Menu");
	});
}

void WindowModificationState::DrawMenu()
{
	m_text.SetContent("Move the window or resize it, this text should change !\nM for Menu");
}
