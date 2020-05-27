#include "KeyState.hpp"

#include "StateContext.hpp"
#include "StateFactory.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <NazaraSDK/StateMachine.hpp>

KeyState::KeyState(StateContext& context) :
BaseState(context),
m_keyStatus(KeyStatus::Pressed)
{
}

void KeyState::Enter(Ndk::StateMachine& fsm)
{
	BaseState::Enter(fsm);

	Nz::EventHandler& eventHandler = m_context.window.GetEventHandler();
	m_keyPressedSlot.Connect(eventHandler.OnKeyPressed, [&] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& key)
	{
		ManageInput(KeyStatus::Pressed, key, fsm);
	});

	m_keyReleasedSlot.Connect(eventHandler.OnKeyReleased, [&] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& key)
	{
		ManageInput(KeyStatus::Released, key, fsm);
	});
}

void KeyState::DrawMenu()
{
	m_text.SetContent("Clic on a key, this text should change !\nN for alternating event\nM for Menu");
}

void KeyState::ManageInput(KeyStatus /*isKeyPressed*/, const Nz::WindowEvent::KeyEvent& key, Ndk::StateMachine& fsm)
{
	if (key.virtualKey == Nz::Keyboard::VKey::M && key.shift)
		fsm.ChangeState(StateFactory::Get(EventStatus::Menu));
	else if (key.virtualKey == Nz::Keyboard::VKey::N && key.shift)
	{
		if (m_keyStatus == KeyStatus::Pressed)
			m_keyStatus = KeyStatus::Released;
		else
			m_keyStatus = KeyStatus::Pressed;
	}
	else
	{
		Nz::String content;
		if (m_keyStatus == KeyStatus::Pressed)
			content = "Pressed: ";
		else
			content = "Released: ";

		Nz::String keyName = Nz::Keyboard::GetKeyName(key.virtualKey) + " (" + Nz::Keyboard::GetKeyName(key.scancode) + ")";
		if (keyName.IsEmpty())
		{
			m_text.SetContent("Unknown\nM for Menu");
		}
		else
		{
			content += keyName;
			if (key.alt)
				content += " alt";
			if (key.control)
				content += " control";
			if (key.repeated)
				content += " repeated";
			if (key.shift)
				content += " shift";
			if (key.system)
				content += " system";
			
			m_text.SetContent(content + "\nM for Menu");
		}
	}
}
