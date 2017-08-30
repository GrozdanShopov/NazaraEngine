// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/TextAreaWidget.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>
#include <limits>

namespace Ndk
{
	TextAreaWidget::TextAreaWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_cursorPosition(0U, 0U),
	m_cursorGlyph(0),
	m_multiLineEnabled(false),
	m_readOnly(false)
	{
		m_cursorSprite = Nz::Sprite::New();
		m_cursorSprite->SetColor(Nz::Color::Black);
		m_cursorSprite->SetSize(1.f, float(m_drawer.GetFont()->GetSizeInfo(m_drawer.GetCharacterSize()).lineHeight));

		m_cursorEntity = CreateEntity();
		m_cursorEntity->AddComponent<GraphicsComponent>().Attach(m_cursorSprite, 10);
		m_cursorEntity->AddComponent<NodeComponent>().SetParent(this);
		m_cursorEntity->Enable(false);

		m_textSprite = Nz::TextSprite::New();

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);

		SetCursor(Nz::SystemCursor_Text);

		Layout();
	}

	void TextAreaWidget::AppendText(const Nz::String& text)
	{
		m_drawer.AppendText(text);

		m_textSprite->Update(m_drawer);
	}

	std::size_t TextAreaWidget::GetHoveredGlyph(float x, float y) const
	{
		std::size_t glyphCount = m_drawer.GetGlyphCount();
		if (glyphCount > 0)
		{
			std::size_t lineCount = m_drawer.GetLineCount();
			std::size_t line = 0U;
			for (; line < lineCount - 1; ++line)
			{
				Nz::Rectf lineBounds = m_drawer.GetLine(line).bounds;
				if (lineBounds.GetMaximum().y > y)
					break;
			}

			std::size_t upperLimit = (line != lineCount - 1) ? m_drawer.GetLine(line + 1).glyphIndex : glyphCount + 1;

			std::size_t i = m_drawer.GetLine(line).glyphIndex;
			for (; i < upperLimit - 1; ++i)
			{
				Nz::Rectf bounds = m_drawer.GetGlyph(i).bounds;
				if (x < bounds.x + bounds.width * 0.75f)
					break;
			}

			return i;
		}

		return 0;
	}

	void TextAreaWidget::ResizeToContent()
	{
		SetContentSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));
	}

	void TextAreaWidget::Write(const Nz::String& text)
	{
		if (m_cursorGlyph >= m_drawer.GetGlyphCount())
		{
			AppendText(text);
			SetCursorPosition(m_drawer.GetGlyphCount());
		}
		else
		{
			Nz::String currentText = m_drawer.GetText();
			currentText.Insert(currentText.GetCharacterPosition(m_cursorGlyph), text);
			SetText(currentText);

			SetCursorPosition(m_cursorGlyph + text.GetLength());
		}
	}

	void TextAreaWidget::Layout()
	{
		BaseWidget::Layout();

		m_textEntity->GetComponent<NodeComponent>().SetPosition(GetContentOrigin());

		RefreshCursor();
	}

	void TextAreaWidget::OnKeyPressed(const Nz::WindowEvent::KeyEvent& key)
	{
		switch (key.code)
		{
			case Nz::Keyboard::Delete:
			{
				const Nz::String& text = m_drawer.GetText();

				Nz::String newText;
				if (m_cursorGlyph > 0)
					newText.Append(text.SubString(0, text.GetCharacterPosition(m_cursorGlyph) - 1));

				if (m_cursorGlyph < m_drawer.GetGlyphCount())
					newText.Append(text.SubString(text.GetCharacterPosition(m_cursorGlyph + 1)));

				m_drawer.SetText(newText);
				m_textSprite->Update(m_drawer);
				break;
			}

			case Nz::Keyboard::Down:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyDown(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					break;

				MoveCursor({0, 1});
				break;
			}

			case Nz::Keyboard::Left:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyLeft(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					break;

				MoveCursor({-1, 0});
				break;
			}

			case Nz::Keyboard::Right:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyRight(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					break;

				MoveCursor({1, 0});
				break;
			}

			case Nz::Keyboard::Up:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyUp(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					break;

				MoveCursor({0, -1});
				break;
			}
		}
	}

	void TextAreaWidget::OnKeyReleased(const Nz::WindowEvent::KeyEvent& key)
	{
	}

	void TextAreaWidget::OnMouseEnter()
	{
		m_cursorEntity->Enable(true);
	}

	void TextAreaWidget::OnMouseButtonPress(int x, int y, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
		{
			GrabKeyboard();

			SetCursorPosition(GetHoveredGlyph(float(x), float(y)));
		}
	}

	void TextAreaWidget::OnMouseMoved(int x, int y, int deltaX, int deltaY)
	{
	}

	void TextAreaWidget::OnMouseExit()
	{
		m_cursorEntity->Enable(false);
	}

	void TextAreaWidget::OnTextEntered(char32_t character, bool /*repeated*/)
	{
		if (m_readOnly)
			return;

		switch (character)
		{
			case '\b':
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyBackspace(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					break;

				const Nz::String& text = m_drawer.GetText();

				Nz::String newText;
				if (m_cursorGlyph > 1)
					newText.Append(text.SubString(0, text.GetCharacterPosition(m_cursorGlyph - 1) - 1));

				if (m_cursorGlyph < m_drawer.GetGlyphCount())
					newText.Append(text.SubString(text.GetCharacterPosition(m_cursorGlyph)));

				MoveCursor({-1, 0});
				SetText(newText);
				break;
			}

			case '\r':
			case '\n':
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyReturn(this, &ignoreDefaultAction);

				if (ignoreDefaultAction || !m_multiLineEnabled)
					break;

				Write(Nz::String('\n'));
				break;
			}

			default:
			{
				if (Nz::Unicode::GetCategory(character) == Nz::Unicode::Category_Other_Control)
					break;

				Write(Nz::String::Unicode(character));
				break;
			}
		}
	}

	void TextAreaWidget::RefreshCursor()
	{
		const auto& lineInfo = m_drawer.GetLine(m_cursorPosition.y);

		std::size_t glyphCount = m_drawer.GetGlyphCount();
		float position;
		if (glyphCount > 0 && lineInfo.glyphIndex < m_cursorGlyph)
		{
			const auto& glyph = m_drawer.GetGlyph(std::min(m_cursorGlyph, glyphCount - 1));
			position = glyph.bounds.x;
			if (m_cursorGlyph >= glyphCount)
				position += glyph.bounds.width;
		}
		else
			position = 0.f;

		Nz::Vector2f contentOrigin = GetContentOrigin();

		m_cursorEntity->GetComponent<NodeComponent>().SetPosition(contentOrigin.x + position, contentOrigin.y + lineInfo.bounds.y);
	}
}
