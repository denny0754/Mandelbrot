#include "MandelbrotUtils.hpp"
#include "Config.hpp"
#include "Logger.hpp"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <array>
#include <functional>
#include <mutex>

namespace Mandelbrot
{
	struct MandelbrotPlaneData
	{
		long double Zoom;
		long double OffsetX;
		long double OffsetY;
	};

	struct MandelbrotProcessData
	{
		std::size_t MinX;
		std::size_t MaxX;
		std::size_t MinY;
		std::size_t MaxY;
		MandelbrotPlaneData Data;
	};

	struct MandelbrotInternalData
	{
		static constexpr std::size_t MandelbrotArraySize = static_cast<std::size_t>(Config::WINDOW_WIDTH) * static_cast<std::size_t>(Config::WINDOW_HEIGHT);

		static inline std::size_t ThreadCounter = std::thread::hardware_concurrency();

		static inline std::size_t MaxIterations = 1000;
		static inline std::size_t DefaultMaxIterations = MaxIterations;

		struct MandelbrotVertexBuffer
		{
			sf::Vertex MandelbrotVertices[MandelbrotInternalData::MandelbrotArraySize];
			sf::VertexBuffer MandelbrotBuffer = sf::VertexBuffer(sf::PrimitiveType::Points, sf::VertexBuffer::Usage::Stream);
		};

		struct MandelbrotSprite
		{
			sf::Sprite MdSprite;
			sf::Texture MdTexture;
			sf::Image MdImage;
		};

		// Mandelbrot Set Storages
		static inline MandelbrotVertexBuffer MdVertexBuffer;
		static inline MandelbrotSprite MdSprite;

		// Pointer to the draw function.
		static inline std::function<void(sf::RenderWindow&)> DrawFncPtr;

		// Pointers to Process Single-Threaded and Multi-Threaded functions.
		static inline std::function<void(const MandelbrotProcessData&)> ProcessFncPtrSt;
		static inline std::function<void(const MandelbrotProcessData&)> ProcessFncPtrMt;

		// If true, the generation process and the draw function will use the VertexBuffer.
		// Otherwise, a sprite is used.
		static inline bool UsingVertexBuffer = sf::VertexBuffer::isAvailable();

		// Data of the plane. See `MandelbrotData`.
		static inline MandelbrotPlaneData PlaneData = MandelbrotPlaneData();
		static inline MandelbrotPlaneData PreviousPlaneData = MandelbrotPlaneData();
		static inline MandelbrotPlaneData DefaultPlaneData = MandelbrotPlaneData();

		static inline bool StateChanged = false;

		static inline std::unordered_map<std::size_t, sf::Color> MandelbrotSetColors = {};

		// Mutex used on Multi-threaded functions
		static inline std::mutex Mutex = std::mutex();
	};

	void DrawVertexBuffer(sf::RenderWindow& renderer);
	void DrawSprite(sf::RenderWindow& renderer);

	void ProcessStUsingSprite(const MandelbrotProcessData& data);
	void ProcessStUsingVertexBuffer(const MandelbrotProcessData& data);

	void ProcessMtUsingVertexBuffer(const MandelbrotProcessData& data);
	void ProcessMtUsingSprite(const MandelbrotProcessData& data);

	// Returns the true x-y coordinates of the Set.
	sf::Vector2ld ScaleToPlane(const sf::Vector2ld& coords, const MandelbrotPlaneData& data);

	// Initializes the Mandelbrot set
	void Init()
	{
		// Initializing Vertices screen coordinates
		for (std::size_t y = 0; y < Config::WINDOW_HEIGHT; y++)
		{
			for (std::size_t x = 0; x < Config::WINDOW_WIDTH; x++)
			{
				std::size_t j = (y * Config::WINDOW_WIDTH) + x;
				MandelbrotInternalData::MdVertexBuffer.MandelbrotVertices[j].position = { static_cast<float>(x), static_cast<float>(y) };
			}
		}

		MandelbrotInternalData::MdSprite.MdImage.create(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);

		// Initializing function pointers
		if (MandelbrotInternalData::UsingVertexBuffer)
		{
			MandelbrotInternalData::ProcessFncPtrSt = std::bind(Mandelbrot::ProcessStUsingVertexBuffer, std::placeholders::_1);
			MandelbrotInternalData::ProcessFncPtrMt = std::bind(Mandelbrot::ProcessMtUsingVertexBuffer, std::placeholders::_1);
			MandelbrotInternalData::DrawFncPtr = std::bind(Mandelbrot::DrawVertexBuffer, std::placeholders::_1);
		}
		else
		{
			MandelbrotInternalData::ProcessFncPtrSt = std::bind(Mandelbrot::ProcessStUsingSprite, std::placeholders::_1);
			MandelbrotInternalData::ProcessFncPtrMt = std::bind(Mandelbrot::ProcessMtUsingSprite, std::placeholders::_1);
			MandelbrotInternalData::DrawFncPtr = std::bind(Mandelbrot::DrawSprite, std::placeholders::_1);
		}

		MandelbrotInternalData::MdVertexBuffer.MandelbrotBuffer.create(MandelbrotInternalData::MandelbrotArraySize);
		MandelbrotInternalData::MdVertexBuffer.MandelbrotBuffer.update(MandelbrotInternalData::MdVertexBuffer.MandelbrotVertices);

		Logger::GetLogger()->info("Mandelbrot Set Data Initialized.");
		Logger::GetLogger()->info("\t=> Available Threads: {}", MandelbrotInternalData::ThreadCounter);

		for (std::size_t iter = 0; iter < 128; iter++)
		{
			MandelbrotInternalData::MandelbrotSetColors[iter] = GetPointColor(iter);
		}
		MandelbrotInternalData::MandelbrotSetColors[MandelbrotInternalData::MaxIterations] = sf::Color::Black;
	}

	void ProcessStUsingSprite(const MandelbrotProcessData& data)
	{
		for (std::size_t y = 0; y < Config::WINDOW_HEIGHT; y++)
		{
			for (std::size_t x = 0; x < Config::WINDOW_WIDTH; x++)
			{
				std::size_t j = (y * Config::WINDOW_WIDTH) + x;

				// Scaling x and y coordinates to real/imaginary coords.
				sf::Vector2ld plane_coords = ScaleToPlane(
					{ static_cast<long double>(x), static_cast<long double>(y) },
					MandelbrotInternalData::PlaneData
				);
				MandelbrotInternalData::MdSprite.MdImage.setPixel(
					static_cast<unsigned int>(x),
					static_cast<unsigned int>(y),
					GetPointColor(GetPointIterations(plane_coords))
				);
			}
		}

		MandelbrotInternalData::MdSprite.MdTexture.loadFromImage(MandelbrotInternalData::MdSprite.MdImage);
		MandelbrotInternalData::MdSprite.MdSprite.setTexture(MandelbrotInternalData::MdSprite.MdTexture);
	}

	void ProcessStUsingVertexBuffer(const MandelbrotProcessData& data)
	{
		for (std::size_t y = 0; y < Config::WINDOW_HEIGHT; y++)
		{
			for (std::size_t x = 0; x < Config::WINDOW_WIDTH; x++)
			{
				std::size_t j = (y * Config::WINDOW_WIDTH) + x;

				// Scaling x and y coordinates to real/imaginary coords.
				sf::Vector2ld plane_coords = ScaleToPlane(
					{ static_cast<long double>(x), static_cast<long double>(y) },
					MandelbrotInternalData::PlaneData
				);

				MandelbrotInternalData::MdVertexBuffer.MandelbrotVertices[j].color = MandelbrotInternalData::MandelbrotSetColors.at(GetPointIterations(plane_coords));
			}
		}
		MandelbrotInternalData::MdVertexBuffer.MandelbrotBuffer.update(MandelbrotInternalData::MdVertexBuffer.MandelbrotVertices);
	}

	// Process Mandelbrot points in Single-threaded Mode
	void ProcessSt()
	{
		// No need to initialize `data` correctly as it won't be used by the process function.
		MandelbrotProcessData data{ 0 };
		MandelbrotInternalData::ProcessFncPtrSt(data);
	}

	void ProcessMtUsingVertexBuffer(const MandelbrotProcessData& data)
	{
		std::size_t min_off = (data.MinY * Config::WINDOW_WIDTH) + data.MinX;
		std::size_t max_off = (data.MaxY * Config::WINDOW_WIDTH) + data.MaxX;
		std::size_t vert_sz = max_off - min_off;

		std::vector<sf::Vertex> vert_buffer;
		vert_buffer.resize(vert_sz);

		std::size_t off = 0;

		for (std::size_t y = data.MinY; y < data.MaxY; y++)
		{
			for (std::size_t x = data.MinX; x < data.MaxX; x++)
			{
				sf::Vector2ld plane_coords = ScaleToPlane(
					{ static_cast<long double>(x), static_cast<long double>(y) },
					data.Data
				);

				vert_buffer[off].color = GetPointColor(GetPointIterations(plane_coords));
				vert_buffer[off].position = { static_cast<float>(x), static_cast<float>(y) };
				off++;
			}
		}
		MandelbrotInternalData::MdVertexBuffer.MandelbrotBuffer.update(vert_buffer.data(), vert_sz, static_cast<unsigned int>(min_off));
	}

	void ProcessMtUsingSprite(const MandelbrotProcessData& data)
	{
		for (std::size_t y = data.MinY; y < data.MaxY; y++)
		{
			for (std::size_t x = data.MinX; x < data.MaxX; x++)
			{
				std::size_t j = (y * Config::WINDOW_WIDTH) + x;

				// Scaling x and y coordinates to real/imaginary coords.
				sf::Vector2ld plane_coords = ScaleToPlane(
					{ static_cast<long double>(x), static_cast<long double>(y) },
					data.Data
				);
				// Should we use `std::lock_guard<std::mutex> mutex`?
				//std::lock_guard<std::mutex> mutex(MandelbrotInternalData::Mutex);
				MandelbrotInternalData::MdSprite.MdImage.setPixel(
					static_cast<unsigned int>(x),
					static_cast<unsigned int>(y),
					GetPointColor(GetPointIterations(plane_coords))
				);
			}
		}
	}

	// Process Mandelbrot points in Multi-threaded Mode
	void ProcessMt()
	{
		const std::size_t work_on_x_axis = (Config::WINDOW_WIDTH / MandelbrotInternalData::ThreadCounter);
		const std::size_t work_on_y_axis = (Config::WINDOW_HEIGHT / MandelbrotInternalData::ThreadCounter);

		std::size_t min_x = 0;
		std::size_t max_x = work_on_x_axis;
		std::size_t min_y = 0;
		std::size_t max_y = work_on_y_axis;

		std::vector<std::thread> threads;
		int tid = 0;

		for (int i = 0; i < MandelbrotInternalData::ThreadCounter; i++)
		{
			for (int j = 0; j < MandelbrotInternalData::ThreadCounter; j++)
			{
				MandelbrotProcessData data;
				data.Data = MandelbrotInternalData::PlaneData;
				data.MinX = min_x;
				data.MaxX = max_x;
				data.MinY = min_y;
				data.MaxY = max_y;

				//Logger::GetLogger()->trace("TID=[{}] - min_x={} - max_x={} - min_y={} - max_y={}", tid, data.MinX, data.MaxX, data.MinY, data.MaxY);

				// Pushing the new thread to the vector
				threads.push_back(std::thread(MandelbrotInternalData::ProcessFncPtrMt, data));
				// Updating the workload of the next thread.
				min_x = max_x;
				max_x += work_on_x_axis;
				tid++;
			}
			// Updating the Y workload for the next row of threads.
			min_y = max_y;
			max_y += work_on_y_axis;
			// Resetting the X workload for the next row of threads.
			min_x = 0;
			max_x = work_on_x_axis;
		}

		for (auto& t : threads)
		{
			t.join();
		}
	}

	// Returns the true x-y coordinates of the Set.
	sf::Vector2ld ScaleToPlane(const sf::Vector2ld& coords)
	{
		sf::Vector2ld plane_coords;
		plane_coords.x = (coords.x - Mandelbrot::Config::WINDOW_WIDTH / 2.0) * GetZoom() + GetOffset().x;
		plane_coords.y = (coords.y - Mandelbrot::Config::WINDOW_HEIGHT / 2.0) * GetZoom() + GetOffset().y;
		return plane_coords;
	}

	sf::Vector2ld ScaleToPlane(const sf::Vector2ld& coords, const MandelbrotPlaneData& data)
	{
		sf::Vector2ld plane_coords;
		plane_coords.x = (coords.x - Mandelbrot::Config::WINDOW_WIDTH / 2.0) * data.Zoom + data.OffsetX;
		plane_coords.y = (coords.y - Mandelbrot::Config::WINDOW_HEIGHT / 2.0) * data.Zoom + data.OffsetY;
		return plane_coords;
	}

	std::size_t GetPointIterations(const sf::Vector2ld& plane_coords)
	{
		long double zReal = plane_coords.x;
		long double zImag = plane_coords.y;

#pragma omp parallel for
		for (std::size_t iter = 0; iter < MandelbrotInternalData::MaxIterations; iter++) {
			long double r2 = zReal * zReal;
			long double i2 = zImag * zImag;
			if (r2 + i2 > 4.0)
			{
				return iter;
			}
			zImag = 2.0 * zReal * zImag + plane_coords.y;
			zReal = r2 - i2 + plane_coords.x;
		}
		return MandelbrotInternalData::MaxIterations;
	}

	void SetMaxIterations(std::size_t iter)
	{
		MandelbrotInternalData::MaxIterations = iter;
		MandelbrotInternalData::MandelbrotSetColors[MandelbrotInternalData::MaxIterations] = sf::Color::Black;
	}

	std::size_t GetMaxIterations()
	{
		return MandelbrotInternalData::MaxIterations;
	}

	void SetDefaultMaxIterations(std::size_t iter)
	{
		if (iter > 0)
		{
			MandelbrotInternalData::DefaultMaxIterations = iter;
		}
	}

	std::size_t GetDefaultMaxIterations()
	{
		return MandelbrotInternalData::DefaultMaxIterations;
	}

	sf::Color GetPointColor(std::size_t iterations)
	{
		uint8_t r, g, b;

		if (iterations == MandelbrotInternalData::MaxIterations)
		{
			return { 0, 0, 0 };
		}
		else if (iterations == 0)
		{
			return { 255, 0, 0 };
		}
		// colour gradient:      Red -> Blue -> Green -> Red -> Black
		// corresponding values:  0  ->  16  ->  32   -> 64  ->  127 (or -1)
		if (iterations < 16)
		{
			r = 16 * (16 - (uint8_t)iterations);
			g = 0;
			b = 16 * (uint8_t)iterations - 1;
		}
		else if ((uint8_t)iterations < 32)
		{
			r = 0;
			g = 16 * ((uint8_t)iterations - 16);
			b = 16 * (32 - (uint8_t)iterations) - 1;
		}
		else if ((uint8_t)iterations < 64)
		{
			r = 8 * ((uint8_t)iterations - 32);
			g = 8 * (64 - (uint8_t)iterations) - 1;
			b = 0;
		}
		else
		{ // range is 64 - 127
			r = 255 - ((uint8_t)iterations - 64) * 4;
			g = 0;
			b = 0;
		}

		return sf::Color{ r, g, b, 255 };
	}

	void SetMaxThreads(std::size_t threads)
	{
		MandelbrotInternalData::ThreadCounter = threads;
	}

	std::size_t GetMaxThreads()
	{
		return MandelbrotInternalData::ThreadCounter;
	}

	void UseVertexBuffer(bool enable)
	{
		MandelbrotInternalData::UsingVertexBuffer = enable;

		// Use VertexBuffer
		if (sf::VertexBuffer::isAvailable() && enable)
		{
			MandelbrotInternalData::ProcessFncPtrSt = std::bind(Mandelbrot::ProcessStUsingVertexBuffer, std::placeholders::_1);
			MandelbrotInternalData::ProcessFncPtrMt = std::bind(Mandelbrot::ProcessMtUsingVertexBuffer, std::placeholders::_1);
			MandelbrotInternalData::DrawFncPtr = std::bind(Mandelbrot::DrawVertexBuffer, std::placeholders::_1);
		}
		// Use Sprite
		else if(enable && !sf::VertexBuffer::isAvailable() || !enable)
		{
			MandelbrotInternalData::ProcessFncPtrSt = std::bind(Mandelbrot::ProcessStUsingSprite, std::placeholders::_1);
			MandelbrotInternalData::ProcessFncPtrMt = std::bind(Mandelbrot::ProcessMtUsingSprite, std::placeholders::_1);
			MandelbrotInternalData::DrawFncPtr = std::bind(Mandelbrot::DrawSprite, std::placeholders::_1);
		}
	}

	bool IsUsingVertexBuffer()
	{
		return MandelbrotInternalData::UsingVertexBuffer;
	}

	void DrawMandelbrotSet(sf::RenderWindow& renderer)
	{
		MandelbrotInternalData::DrawFncPtr(renderer);
	}

	void DrawVertexBuffer(sf::RenderWindow& renderer)
	{
		renderer.draw(MandelbrotInternalData::MdVertexBuffer.MandelbrotBuffer);
	}
	
	void DrawSprite(sf::RenderWindow& renderer)
	{
		// Sprite must be updated here, otherwise, a black image would be drawn to the screen.
		// I don't what causes that. Probably is related to the fact the one or more thread exists
		// before others and the texture and sprite are updated before every thread being done.
		// The `std::lock_guard` isn't needed also. The resource(sprite, texture and image) could be free at any time in the process thread.
		MandelbrotInternalData::MdSprite.MdTexture.loadFromImage(MandelbrotInternalData::MdSprite.MdImage);
		MandelbrotInternalData::MdSprite.MdSprite.setTexture(MandelbrotInternalData::MdSprite.MdTexture);
		renderer.draw(MandelbrotInternalData::MdSprite.MdSprite);
	}

	void SetZoom(const long double& zoom)
	{
		MandelbrotInternalData::PreviousPlaneData.Zoom = MandelbrotInternalData::PlaneData.Zoom;

		MandelbrotInternalData::StateChanged = true;

		MandelbrotInternalData::PlaneData.Zoom = zoom;
	}

	long double GetZoom()
	{
		return MandelbrotInternalData::PlaneData.Zoom;
	}

	void SetDefaultZoom(long double zoom)
	{
		MandelbrotInternalData::DefaultPlaneData.Zoom = zoom;
	}

	long double GetDefaultZoom()
	{
		return MandelbrotInternalData::DefaultPlaneData.Zoom;
	}

	void SetOffset(const sf::Vector2ld& offset)
	{
		MandelbrotInternalData::PreviousPlaneData.OffsetX = MandelbrotInternalData::PlaneData.OffsetX;
		MandelbrotInternalData::PreviousPlaneData.OffsetY = MandelbrotInternalData::PlaneData.OffsetY;

		MandelbrotInternalData::StateChanged = true;

		MandelbrotInternalData::PlaneData.OffsetX = offset.x;
		MandelbrotInternalData::PlaneData.OffsetY = offset.y;
	}

	sf::Vector2ld GetOffset()
	{
		return {
			MandelbrotInternalData::PlaneData.OffsetX,
			MandelbrotInternalData::PlaneData.OffsetY
		};
	}

	void SetDefaultOffset(const sf::Vector2ld& offset)
	{
		MandelbrotInternalData::DefaultPlaneData.OffsetX = offset.x;
		MandelbrotInternalData::DefaultPlaneData.OffsetY = offset.y;
	}

	sf::Vector2ld GetDefaultOffset()
	{
		return {
			MandelbrotInternalData::DefaultPlaneData.OffsetX,
			MandelbrotInternalData::DefaultPlaneData.OffsetY
		};
	}

	void Update()
	{
		if (MandelbrotInternalData::StateChanged)
		{
			ProcessMt();
			MandelbrotInternalData::StateChanged = false;
		}
	}

	void ForceUpdate()
	{
		ProcessMt();
		MandelbrotInternalData::StateChanged = false;
	}

}