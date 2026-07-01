#ifndef APP_ENGINE_ENGINE_H
#define APP_ENGINE_ENGINE_H

#include "Core/app.h"
#include "Core/app_context.h"
#include "Core/components.h"
#include "Core/utils.h"

#include "Layers/graphics_pipeline_manager.h"
#include "Layers/scene_manager.h"
#include "Layers/texture_manager.h"
#include "Layers/texture_region_manager.h"
#include "Layers/texture_sampler_manager.h"

#include "Rendering/RenderPasses/render_target_render_pass.h"

#include "Rendering/2D/camera_2d.h"
#include "Rendering/2D/renderer_2d.h"
#include "Rendering/2D/shape_2d_batch.h"

#if USE_IMGUI
#include "imgui.h"
#endif

#endif // APP_ENGINE_ENGINE_H
