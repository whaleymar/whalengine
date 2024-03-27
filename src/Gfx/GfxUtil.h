#pragma once

#include "Util/Types.h"

namespace whal {

static const char* WINDOW_TITLE = "Untitled Engine";
static constexpr u32 WINDOW_WIDTH_PIXELS = 1280;
static constexpr u32 WINDOW_HEIGHT_PIXELS = 720;
static constexpr u32 PIXELS_PER_TEXEL = 4;
// constexpr u32 WINDOW_WIDTH_PIXELS = 1920;
// constexpr u32 WINDOW_HEIGHT_PIXELS = 1080;
// constexpr u32 PIXELS_PER_TEXEL = 6;

// these convert world coordinates to screen coordinates, corrected for aspect ratio
static constexpr f32 TEXELS_PER_TILE = 8;
static constexpr f32 WORLD_SCALE_RATIO = TEXELS_PER_TILE / 10.0;  // don't change this. no i don't know how i calculated it
static constexpr f32 TEXEL_SCALE_X = 1.0 / (16.0 / WORLD_SCALE_RATIO);
static constexpr f32 TEXEL_SCALE_Y = 1.0 / (9.0 / WORLD_SCALE_RATIO);

static constexpr f32 WINDOW_WIDTH_TEXELS = WINDOW_WIDTH_PIXELS / PIXELS_PER_TEXEL;
static constexpr f32 WINDOW_HEIGHT_TEXELS = WINDOW_HEIGHT_PIXELS / PIXELS_PER_TEXEL;
static constexpr f32 WINDOW_WIDTH_TILES = WINDOW_WIDTH_TEXELS / TEXELS_PER_TILE;
static constexpr f32 WINDOW_HEIGHT_TILES = WINDOW_HEIGHT_TEXELS / TEXELS_PER_TILE;

}  // namespace whal
