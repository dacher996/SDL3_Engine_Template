#ifndef ENGINE_APP_CONTEXT_H
#define ENGINE_APP_CONTEXT_H

#include <string>

/// Application context
struct AppContext {
    /// The working directory of the app
    std::string basePath;

    /// The delta time since last frame
    float deltaTime;

    /// The logical width of the game
    int gameWidth;

    /// The logical height of the game
    int gameHeight;

    /// The width of the window
    int windowWidth;

    /// The height of the window
    int windowHeight;
};

#endif //ENGINE_APP_CONTEXT_H
