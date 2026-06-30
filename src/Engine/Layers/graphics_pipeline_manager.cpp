#include "Engine/Layers/graphics_pipeline_manager.h"

#include <fstream>
#include <ranges>

#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Core/logger.h"

namespace Engine {
  void GraphicsPipelineManager::Cleanup() {
    const auto &ctx = App::GetLayer<AppContext>();
    for (auto pipeline: m_pipelines | std::views::values) {
      SDL_ReleaseGPUGraphicsPipeline(ctx.gpuDevice, pipeline);
    }
    m_pipelines.clear();
  }

  void GraphicsPipelineManager::SetDefaultPipeline(Uint16 id) {
    m_defaultPipelineId = id;
  }

  SDL_GPUGraphicsPipeline *GraphicsPipelineManager::GetDefaultPipeline() {
    return GetPipeline(m_defaultPipelineId);
  }

  SDL_GPUShader *GraphicsPipelineManager::LoadShader(const std::string &shaderFilename, Uint32 samplerCount,
                                                     Uint32 uniformBufferCount, Uint32 storageBufferCount,
                                                     Uint32 storageTextureCount) {
    // Auto-detect the shader stage from the file name for convenience
    SDL_GPUShaderStage stage;
    if (SDL_strstr(shaderFilename.c_str(), ".vert")) {
      stage = SDL_GPU_SHADERSTAGE_VERTEX;
    } else if (SDL_strstr(shaderFilename.c_str(), ".frag")) {
      stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    } else {
      ENGINE_LOG_ERROR("Unrecognized shader stage");
      return nullptr;
    }

    auto &ctx = App::GetLayer<AppContext>();
    std::string fullPath;
    SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(ctx.gpuDevice);
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
    const char *entrypoint;
    static std::string shaderDir = std::format("{}assets/shaders/", ctx.basePath);

    if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
      fullPath = std::format("{}SPIRV/{}.spv", shaderDir, shaderFilename);
      format = SDL_GPU_SHADERFORMAT_SPIRV;
      entrypoint = "main";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
      fullPath = std::format("{}MSL/{}.MSL", shaderDir, shaderFilename);
      format = SDL_GPU_SHADERFORMAT_MSL;
      entrypoint = "main0";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
      fullPath = std::format("{}DXIL/{}.dxil", shaderDir, shaderFilename);
      format = SDL_GPU_SHADERFORMAT_DXIL;
      entrypoint = "main";
    } else {
      ENGINE_LOG_ERROR("Unrecognized shader format.");
      return nullptr;
    }

    std::ifstream file{fullPath, std::ios::binary};
    if (!file) {
      ENGINE_LOG_ERROR("Couldn't open file.");
      return nullptr;
    }

    std::vector<Uint8> code{std::istreambuf_iterator(file), {}};

    SDL_GPUShaderCreateInfo shaderInfo{
      .code_size = code.size(),
      .code = code.data(),
      .entrypoint = entrypoint,
      .format = format,
      .stage = stage,
      .num_samplers = samplerCount,
      .num_storage_textures = storageTextureCount,
      .num_storage_buffers = storageBufferCount,
      .num_uniform_buffers = uniformBufferCount,
    };

    SDL_GPUShader *shader = SDL_CreateGPUShader(ctx.gpuDevice, &shaderInfo);
    if (!shader)
      ENGINE_LOG_SDL_ERROR("Couldn't create GPU shader.");

    return shader;
  }

  void GraphicsPipelineManager::UnloadShader(SDL_GPUShader *shader) {
    if (!shader) return;
    SDL_ReleaseGPUShader(App::GetLayer<AppContext>().gpuDevice, shader);
  }

  SDL_GPUGraphicsPipeline *GraphicsPipelineManager::CreateGraphicsPipeline(SDL_GPUShader *vertShader,
                                                                           SDL_GPUShader *fragShader,
                                                                           const GraphicsPipelineCreationInfo &
                                                                           creationInfo) {
    auto &ctx = App::GetLayer<AppContext>();
    // Setup blend state and rendering format
    std::array colorTargetDescriptions = {
      SDL_GPUColorTargetDescription{
        .format = SDL_GetGPUSwapchainTextureFormat(ctx.gpuDevice, ctx.window),
        .blend_state = creationInfo.blendState,
      },
    };

    // Setup pipeline data and create the pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{
      .vertex_shader = vertShader,
      .fragment_shader = fragShader,
      .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
      .target_info = {
        .color_target_descriptions = colorTargetDescriptions.data(),
        .num_color_targets = colorTargetDescriptions.size(),
      },
    };

    if (creationInfo.vertexInputState != nullptr) {
      pipelineCreateInfo.vertex_input_state = *creationInfo.vertexInputState;
    }
    if (creationInfo.depthStencilState != nullptr) {
      pipelineCreateInfo.depth_stencil_state = *creationInfo.depthStencilState;
    }

    return SDL_CreateGPUGraphicsPipeline(ctx.gpuDevice, &pipelineCreateInfo);
  }

  bool GraphicsPipelineManager::RegisterPipeline(Uint16 id, SDL_GPUGraphicsPipeline *pipeline) {
    if (!pipeline) return false;
    if (m_pipelines.contains(id)) {
      ENGINE_LOG_WARN(std::format("Graphics Pipeline slot {} already taken.", id));
      return false;
    }
    m_pipelines[id] = pipeline;
    return true;
  }

  void GraphicsPipelineManager::UnloadPipeline(const Uint16 id) {
    if (!m_pipelines.contains(id)) return;
    SDL_ReleaseGPUGraphicsPipeline(App::GetLayer<AppContext>().gpuDevice, m_pipelines[id]);
    m_pipelines.erase(id);
  }

  SDL_GPUGraphicsPipeline *GraphicsPipelineManager::GetPipeline(const Uint16 id) {
    if (m_pipelines.contains(id)) {
      return m_pipelines[id];
    }
    return nullptr;
  }
}
