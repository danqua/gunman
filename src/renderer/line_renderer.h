#pragma once
#include "core/types.h"
#include "core/memory.h"
#include "renderer/camera.h"

// Initializes the line renderer.
void LineRenderer_Init(Arena* arena);

// Shuts down the line renderer.
void LineRenderer_Shutdown();

// Begins a new frame for the line renderer, setting up necessary state.
void LineRenderer_BeginFrame(const Camera* camera);

// Ends the current frame, rendering all queued lines.
void LineRenderer_EndFrame();

// Draws a line between two points with a specified color.
void LineRenderer_DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec4 color);