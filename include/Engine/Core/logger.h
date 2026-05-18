#ifndef ENGINE_LOGGER_H
#define ENGINE_LOGGER_H

#include <spdlog/spdlog.h>
#include <memory>

namespace Engine {
    class Logger {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return coreLogger; }
        static std::shared_ptr<spdlog::logger> &GetClientLogger() { return clientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> coreLogger;
        static std::shared_ptr<spdlog::logger> clientLogger;
    };
}

#ifdef DEBUG_MODE

#define ENGINE_LOG_TRACE(...) ::Engine::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_LOG_INFO(...) ::Engine::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_LOG_WARN(...) ::Engine::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_LOG_ERROR(...) ::Engine::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_LOG_CRITICAL(...) ::Engine::Logger::GetCoreLogger()->critical(__VA_ARGS__)
#define ENGINE_LOG_SDL_ERROR(msg) ::Engine::Logger::GetCoreLogger()->error(msg "\n\n{}", SDL_GetError())

#define LOG_TRACE(...) ::Engine::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::Engine::Logger::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Engine::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Engine::Logger::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Engine::Logger::GetClientLogger()->critical(__VA_ARGS__)

#else

#define ENGINE_LOG_TRACE
#define ENGINE_LOG_INFO
#define ENGINE_LOG_WARN
#define ENGINE_LOG_ERROR
#define ENGINE_LOG_CRITICAL
#define ENGINE_LOG_SDL_ERROR

#define LOG_TRACE
#define LOG_INFO
#define LOG_WARN
#define LOG_ERROR
#define LOG_CRITICAL

#endif

#endif //ENGINE_LOGGER_H
