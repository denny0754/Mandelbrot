#pragma once
#ifndef MANDELBROT_GUI_BUTTON_HPP
#define MANDELBROT_GUI_BUTTON_HPP

#include <SFML/Graphics.hpp>

#include <functional>

namespace Mandelbrot
{
	namespace Gui
	{

		class Button : public sf::Drawable
		{
		public:
			enum class ButtonState
			{
				Idle,
				Pressed,
				Hovered
			};

		private:
			sf::RectangleShape m_Button;
			sf::Text m_ButtonText;
			sf::Font m_ButtonTextFont;

			std::array<sf::Color, 3> m_ButtonStyles;

			ButtonState m_ButtonState;

		public:
			inline Button()
			{
				m_Button = sf::RectangleShape();
				m_ButtonText = sf::Text();
				m_ButtonTextFont = sf::Font();
			}

			inline Button(sf::Vector2f size, sf::Vector2f position, const std::string& btnValue = "BUTTON", unsigned int charsize = 14)
			{
				m_ButtonStyles = {
					sf::Color(20, 20, 20, 200),
					sf::Color(40, 40, 40, 200),
					sf::Color(0, 0, 0, 220)
				};

				m_Button = sf::RectangleShape();
				m_Button.setPosition(position);
				m_Button.setSize(size);
				m_Button.setFillColor(m_ButtonStyles[0]);

				m_ButtonTextFont.loadFromFile("./assets/fonts/Roboto-Regular.ttf");
				m_ButtonText.setFont(m_ButtonTextFont);
				m_ButtonText.setString(btnValue);
				m_ButtonText.setCharacterSize(charsize);

				m_ButtonText.setPosition({
						m_Button.getPosition().x, // + (m_Button.getPosition().x / 2.f) - m_ButtonText.getGlobalBounds().width / 2.f,
						m_Button.getPosition().y // + (m_Button.getPosition().y / 2.f) - m_ButtonText.getGlobalBounds().height * 3.f
				});
			}

			inline void draw(sf::RenderTarget& target, sf::RenderStates states) const override
			{
				target.draw(m_Button);
				target.draw(m_ButtonText);
			}

			inline void Update(sf::RenderWindow& win)
			{
				auto mouse_pos = sf::Mouse::getPosition(win);

				bool is_hovering = m_Button.getGlobalBounds().contains({ static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y) });
				if (is_hovering && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					m_ButtonState = ButtonState::Pressed;
				}
				else if (is_hovering)
				{
					m_ButtonState = ButtonState::Hovered;
				}
				else
				{
					m_ButtonState = ButtonState::Idle;
				}

				m_Button.setFillColor(m_ButtonStyles[static_cast<std::size_t>(m_ButtonState)]);
			}

			inline ButtonState GetCurrentState() const
			{
				return m_ButtonState;
			}

		};
	}
}

#endif