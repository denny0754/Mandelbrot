#pragma once
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

class Logger
{
private:
	inline static std::shared_ptr<spdlog::logger> s_Logger = nullptr;

public:
	inline static void Init(const std::string& loggerName)
	{
		std::string logger_name = loggerName.empty() ? "LOGGER" : loggerName;

		spdlog::sink_ptr file_sink = std::make_shared< spdlog::sinks::basic_file_sink_mt>(logger_name, false);
		spdlog::sink_ptr stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(spdlog::color_mode::always);

		spdlog::sinks_init_list sinks = { file_sink, stdout_sink };

		s_Logger = std::make_shared<spdlog::logger>(logger_name, sinks);
		s_Logger->set_pattern("[%l] %n: %^%v%$");
		s_Logger->set_level(spdlog::level::level_enum::trace);
	}

	inline static spdlog::logger* GetLogger()
	{
		return s_Logger.get();
	}

	inline static void SetLoggerLevel(spdlog::level::level_enum level)
	{
		s_Logger->set_level(level);
	}
};

#endif