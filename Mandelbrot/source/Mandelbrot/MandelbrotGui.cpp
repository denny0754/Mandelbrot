#include "MandelbrotGui.hpp"

#include "Config.hpp"
#include "Button.hpp"
#include "MandelbrotUtils.hpp"
#include "Logger.hpp"

namespace Mandelbrot
{
	namespace Gui
	{
		struct MandelbrotGuiInternalData
		{
			static inline sf::RectangleShape ImaginaryAxis = sf::RectangleShape({ 1.f , Mandelbrot::Config::WINDOW_HEIGHT });
			static inline sf::RectangleShape RealAxis = sf::RectangleShape({ Mandelbrot::Config::WINDOW_WIDTH , 1.f });

			static inline Mandelbrot::Gui::Button ZoomInButton = Mandelbrot::Gui::Button({ 100, 25 }, { 0, 0 }, "ZOOM-IN");
			static inline Mandelbrot::Gui::Button ZoomOutButton = Mandelbrot::Gui::Button({ 100, 25 }, { 100, 0 }, "ZOOM-OUT");

			static inline Mandelbrot::Gui::Button OffsetXPlusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 200, 0 }, "OFFX+");
			static inline Mandelbrot::Gui::Button OffsetXMinusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 300, 0 }, "OFFX-");

			static inline Mandelbrot::Gui::Button OffsetYPlusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 400, 0 }, "OFFY+");
			static inline Mandelbrot::Gui::Button OffsetYMinusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 500, 0 }, "OFFY-");

			static inline Mandelbrot::Gui::Button IterationsPlusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 600, 0 }, "ITER+");
			static inline Mandelbrot::Gui::Button IterationsMinusButton = Mandelbrot::Gui::Button({ 100, 25 }, { 700, 0 }, "ITER-");

			static inline Mandelbrot::Gui::Button ToggleVertexBufferButton = Mandelbrot::Gui::Button({ 100, 25 }, { 800, 0 }, "TOGGLE VB");

			static inline bool ShouldUpdateProcess = false;

			static inline bool HiddenGui = false;
		};


		void InitGui()
		{
			MandelbrotGuiInternalData::RealAxis.setPosition({ 0, Mandelbrot::Config::WINDOW_HEIGHT / 2.f });
			MandelbrotGuiInternalData::ImaginaryAxis.setPosition({ Mandelbrot::Config::WINDOW_WIDTH / 2.f,  0 });

			MandelbrotGuiInternalData::RealAxis.setFillColor(sf::Color(255, 255, 255, 100));
			MandelbrotGuiInternalData::ImaginaryAxis.setFillColor(sf::Color(255, 255, 100));
		}


		void UpdateGui(sf::RenderWindow& window)
		{
#pragma omp parallel for
			{
				MandelbrotGuiInternalData::ZoomInButton.Update(window);
				MandelbrotGuiInternalData::ZoomOutButton.Update(window);

				MandelbrotGuiInternalData::OffsetXPlusButton.Update(window);
				MandelbrotGuiInternalData::OffsetXMinusButton.Update(window);

				MandelbrotGuiInternalData::OffsetYPlusButton.Update(window);
				MandelbrotGuiInternalData::OffsetYMinusButton.Update(window);

				MandelbrotGuiInternalData::IterationsPlusButton.Update(window);
				MandelbrotGuiInternalData::IterationsMinusButton.Update(window);

				MandelbrotGuiInternalData::ToggleVertexBufferButton.Update(window);
			}

			// TODO Code below MUST be cleaned up. Either move button events on a function or create a `OnButtonPress` method inside the button class.
			if (MandelbrotGuiInternalData::OffsetXPlusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				auto offset = Mandelbrot::GetOffset();
				Mandelbrot::SetOffset(
					{
						offset.x + 0.0008,
						offset.y
					}
				);
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}
			else if (MandelbrotGuiInternalData::OffsetXMinusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				auto offset = Mandelbrot::GetOffset();
				Mandelbrot::SetOffset(
					{
						offset.x - 0.0008,
						offset.y
					}
				);
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}

			if (MandelbrotGuiInternalData::OffsetYPlusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				auto offset = Mandelbrot::GetOffset();
				Mandelbrot::SetOffset(
					{
						offset.x,
						offset.y + 0.0008
					}
				);
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}
			else if (MandelbrotGuiInternalData::OffsetYMinusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				auto offset = Mandelbrot::GetOffset();
				Mandelbrot::SetOffset(
					{
						offset.x,
						offset.y - 0.0008
					}
				);
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}

			if (MandelbrotGuiInternalData::ZoomInButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				auto lzoom = Mandelbrot::GetZoom();
				Mandelbrot::SetZoom(lzoom - 0.0004);
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}
			else if (MandelbrotGuiInternalData::ZoomOutButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				auto lzoom = Mandelbrot::GetZoom();
				Mandelbrot::SetZoom(lzoom + 0.0004);
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}

			if (MandelbrotGuiInternalData::IterationsPlusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				auto iters = Mandelbrot::GetMaxIterations();
				Mandelbrot::SetMaxIterations(iters + 1000);
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}
			else if (MandelbrotGuiInternalData::IterationsMinusButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				auto iters = Mandelbrot::GetMaxIterations();
				if (iters != 1000)
				{
					Mandelbrot::SetMaxIterations(iters - 1000);
					MandelbrotGuiInternalData::ShouldUpdateProcess = true;
				}
			}

			if (MandelbrotGuiInternalData::ToggleVertexBufferButton.GetCurrentState() == Mandelbrot::Gui::Button::ButtonState::Pressed)
			{
				bool using_vertex_buffer = Mandelbrot::IsUsingVertexBuffer();

				Logger::GetLogger()->info("Vertex Buffer set to {}", using_vertex_buffer);
				if (!using_vertex_buffer)
				{
					Logger::GetLogger()->warn("For a better experience, it is recommended to use VertexBuffer");
				}

				Mandelbrot::UseVertexBuffer(!Mandelbrot::IsUsingVertexBuffer());
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}

			// This will reset Mandelbrot data to default values.
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
			{
				Mandelbrot::SetZoom(Mandelbrot::GetDefaultZoom());
				Mandelbrot::SetOffset(Mandelbrot::GetDefaultOffset());
				Mandelbrot::UseVertexBuffer(false);
				Mandelbrot::SetMaxIterations(Mandelbrot::GetDefaultMaxIterations());
				Mandelbrot::SetMaxThreads(std::thread::hardware_concurrency());
				MandelbrotGuiInternalData::ShouldUpdateProcess = true;
			}

			if (MandelbrotGuiInternalData::ShouldUpdateProcess)
			{
				Logger::GetLogger()->info("Current Settings:");
				Logger::GetLogger()->info("\tOffsetX: {:<10}", Mandelbrot::GetOffset().x);
				Logger::GetLogger()->info("\tOffsetY: {:<10}", Mandelbrot::GetOffset().y);
				Logger::GetLogger()->info("\tZoom: {:<10}", Mandelbrot::GetZoom());
				Logger::GetLogger()->info("\tMax Iterations: {:<10}", Mandelbrot::GetMaxIterations());
				Logger::GetLogger()->info("\tThreads: {:<10}", Mandelbrot::GetMaxThreads());


				sf::Thread process_thread(&Mandelbrot::ProcessMt);
				process_thread.launch();
				MandelbrotGuiInternalData::ShouldUpdateProcess = false;
			}
		}

		void DrawGui(sf::RenderWindow& window)
		{
			if (!MandelbrotGuiInternalData::HiddenGui)
			{
				window.draw(MandelbrotGuiInternalData::RealAxis);
				window.draw(MandelbrotGuiInternalData::ImaginaryAxis);
				window.draw(MandelbrotGuiInternalData::ZoomInButton);
				window.draw(MandelbrotGuiInternalData::ZoomOutButton);
				window.draw(MandelbrotGuiInternalData::OffsetXPlusButton);
				window.draw(MandelbrotGuiInternalData::OffsetXMinusButton);
				window.draw(MandelbrotGuiInternalData::OffsetYPlusButton);
				window.draw(MandelbrotGuiInternalData::OffsetYMinusButton);
				window.draw(MandelbrotGuiInternalData::IterationsPlusButton);
				window.draw(MandelbrotGuiInternalData::IterationsMinusButton);
				window.draw(MandelbrotGuiInternalData::ToggleVertexBufferButton);
			}
		}

		void HideGui()
		{
			MandelbrotGuiInternalData::HiddenGui = true;
		}

		void ShowGui()
		{
			MandelbrotGuiInternalData::HiddenGui = false;
		}

	}
}