#include "Engine/Core/logger.h"

#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Engine::Logger::coreLogger;
std::shared_ptr<spdlog::logger> Engine::Logger::clientLogger;

void Engine::Logger::Init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");

    coreLogger = spdlog::stdout_color_mt("ENGINE");
    coreLogger->set_level(spdlog::level::trace);

    clientLogger = spdlog::stdout_color_mt("APP");
    clientLogger->set_level(spdlog::level::trace);
}
