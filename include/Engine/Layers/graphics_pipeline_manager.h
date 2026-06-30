#ifndef ENGINE_GRAPHICS_PIPELINE_MANAGER_H
#define ENGINE_GRAPHICS_PIPELINE_MANAGER_H

#include <string>
#include <unordered_map>

#include "Engine/Rendering/graphics_pipeline_creation_info.h"
#include "SDL3/SDL_gpu.h"

namespace Engine {
    /// Stores and manages the lifecycle of GPU Graphics Pipelines
    class GraphicsPipelineManager {
    public :
        /// Registers a graphics pipeline under the specific id.
        ///
        /// Returns success if the graphics pipeline was registered. If the id was already taken, the pipeline won't be
        /// registered and the function will return with `false`.
        bool RegisterPipeline(Uint16 id, SDL_GPUGraphicsPipeline *pipeline);

        /// Frees a graphics pipeline with a specific id from memory.
        void UnloadPipeline(Uint16 id);

        /// Returns the pipeline previously registered under the provided id or nullptr if the id is invalid.
        SDL_GPUGraphicsPipeline *GetPipeline(Uint16 id);

        /// Frees all pipelines from memory
        void Cleanup();

        /// Sets the default graphics pipeline
        void SetDefaultPipeline(Uint16 id);

        /// Retrieves the default graphics pipeline
        SDL_GPUGraphicsPipeline *GetDefaultPipeline();

        /// Loads a shader from file and returns a pointer to it.
        static SDL_GPUShader *LoadShader(const std::string &shaderFilename,
                                         Uint32 samplerCount = 0,
                                         Uint32 uniformBufferCount = 0,
                                         Uint32 storageBufferCount = 0,
                                         Uint32 storageTextureCount = 0);

        /// Frees the shader from memory
        static void UnloadShader(SDL_GPUShader *shader);

        /// Creates a graphics pipeline from provided vertex/fragment shader and returns a pointer to it.
        static SDL_GPUGraphicsPipeline *CreateGraphicsPipeline(SDL_GPUShader *vertShader, SDL_GPUShader *fragShader,
                                                               const GraphicsPipelineCreationInfo &creationInfo);

    private:
        std::unordered_map<Uint16, SDL_GPUGraphicsPipeline *> m_pipelines;
        Uint16 m_defaultPipelineId{0};
    };
}

#endif //ENGINE_GRAPHICS_PIPELINE_MANAGER_H
