#pragma once

#include "Util/Types.h"

namespace whal {
// TODO create u32/s32/f32 for most of these

inline const char* WINDOW_TITLE = "Untitled Engine";
// only used by openGL:
inline u32 WINDOW_WIDTH_PIXELS_ACTUAL = 1280;
inline u32 WINDOW_HEIGHT_PIXELS_ACTUAL = 720;

// used by the game (openGL will scale up to the actual resolution automatically):
// EFFECTIVE GRID SIZE: 320 x 180
// inline constexpr u32 WINDOW_WIDTH_PIXELS = 320;
// inline constexpr u32 WINDOW_HEIGHT_PIXELS = 180;
// inline constexpr u32 PIXELS_PER_TEXEL = 1;

// this is a nice balance of smoothness + not doing too much work. Things are positioned in half-texels
inline constexpr u32 WINDOW_WIDTH_PIXELS = 640;
inline constexpr u32 WINDOW_HEIGHT_PIXELS = 360;
inline constexpr u32 PIXELS_PER_TEXEL = 2;

// inline constexpr u32 WINDOW_WIDTH_PIXELS = 1280;
// inline constexpr u32 WINDOW_HEIGHT_PIXELS = 720;
// inline constexpr u32 PIXELS_PER_TEXEL = 4;
// constexpr u32 WINDOW_WIDTH_PIXELS = 1600;
// constexpr u32 WINDOW_HEIGHT_PIXELS = 900;
// constexpr u32 PIXELS_PER_TEXEL = 5;
// constexpr u32 WINDOW_WIDTH_PIXELS = 1920;
// constexpr u32 WINDOW_HEIGHT_PIXELS = 1080;
// constexpr u32 PIXELS_PER_TEXEL = 6;

// for convenience:
inline constexpr s32 SWINDOW_WIDTH_PIXELS = static_cast<s32>(WINDOW_WIDTH_PIXELS);
inline constexpr s32 SPIXELS_PER_TEXEL = static_cast<s32>(PIXELS_PER_TEXEL);
inline constexpr f32 FPIXELS_PER_TEXEL = static_cast<f32>(PIXELS_PER_TEXEL);
inline constexpr f32 TEXELS_PER_TILE = 8;
inline constexpr f32 TEXELS_PER_PIXEL = 1 / FPIXELS_PER_TEXEL;
inline constexpr s32 PIXELS_PER_TILE = PIXELS_PER_TEXEL * TEXELS_PER_TILE;
inline constexpr f32 FPIXELS_PER_TILE = static_cast<f32>(PIXELS_PER_TILE);
inline constexpr f32 TILES_PER_PIXEL = 1 / FPIXELS_PER_TILE;

// these convert world coordinates to screen coordinates, corrected for aspect ratio
inline constexpr f32 WORLD_SCALE_RATIO = TEXELS_PER_TILE / 10.0;  // don't change this. no i don't know how i calculated it
inline constexpr f32 TEXEL_SCALE_X = 1.0 / (16.0 / WORLD_SCALE_RATIO);
inline constexpr f32 TEXEL_SCALE_Y = 1.0 / (9.0 / WORLD_SCALE_RATIO);

inline constexpr f32 WINDOW_WIDTH_TEXELS = WINDOW_WIDTH_PIXELS / PIXELS_PER_TEXEL;
inline constexpr f32 WINDOW_HEIGHT_TEXELS = WINDOW_HEIGHT_PIXELS / PIXELS_PER_TEXEL;
inline constexpr f32 WINDOW_WIDTH_TILES = WINDOW_WIDTH_TEXELS / TEXELS_PER_TILE;
inline constexpr f32 WINDOW_HEIGHT_TILES = WINDOW_HEIGHT_TEXELS / TEXELS_PER_TILE;

inline const char* VERTEX_UV_SHADER_PATH = "shader/vertex-uv.glsl";
inline const char* VERTEX_RGB_SHADER_PATH = "shader/vertex-rgb.glsl";
inline const char* VERTEX_RGBUV_SHADER_PATH = "shader/vertex-rgbuv.glsl";
inline const char* FRAG_SPRITE_SHADER_PATH = "shader/sprite-frag.glsl";
inline const char* FRAG_SPRITE_RGB_SHADER_PATH = "shader/sprite-rgb-frag.glsl";
inline const char* FRAG_OUTLINE_SHADER_PATH = "shader/outline-frag.glsl";
inline const char* FRAG_COLOR_SHADER_PATH = "shader/coloronly-frag.glsl";

enum ShaderType : u8 { SpritePlain, SpriteRGB, Debug, RGBonly };
const char* toString(ShaderType shaderType);

inline const char* SPRITE_TEXTURE_PATH = "data/sprite/atlas0.png";
inline const char* ATLAS_METADATA_PATH = "data/sprite/atlas.xml";

inline const char* TEXNAME_SPRITE = "sprite";

}  // namespace whal
