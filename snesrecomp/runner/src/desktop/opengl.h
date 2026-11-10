#pragma once
#include "display_aspect.h"

/* The shared desktop OpenGL presenter (desktop/opengl.c). util.h declares
 * OpenGLRenderer_Create itself; this header carries only the presentation
 * policy a host may need to override. */

#ifdef __cplusplus
extern "C" {
#endif

/* Force the swap interval instead of deriving it from config.
 *
 * Call BEFORE the renderer is created. A host that paces presentation on its
 * own clock (an FPS cap, or a split simulation/presentation rate) passes 0,
 * so SDL_GL_SwapWindow does not block on the display as well and fight that
 * pacing. Left unset, the presenter vsyncs unless config's DisableFrameDelay
 * is on. */
/* Swap interval for the GL presenter: 0 immediate, 1 wait for the panel,
 * -1 late-swap-tearing ("Adaptive"). Call before OpenGLRenderer_Create. */
void snesrecomp_opengl_set_vsync(int interval);
void snesrecomp_opengl_set_viewport(void (*compute)(int, int, int, int,
                                                   SnesDisplayViewport *));

/* Capture the next fully composited OpenGL frame to screenshots/*.bmp. */
void OpenGLRenderer_RequestScreenshot(void);

/* Swap the RetroArch .glslp preset at runtime (NULL/"" = off). False when
 * the GL presenter is not active or the preset failed to compile (the old
 * chain stays). */
#include <stdbool.h>
bool OpenGLRenderer_SetShader(const char *path);
/* True once the OpenGL presenter initialized (the SDL presenter has no
 * shader support). */
bool OpenGLRenderer_IsActive(void);

/* A title-owned post-process chain (e.g. librashader slang presets). Called
 * each frame after the frame texture (GL_RGBA, top row first) is uploaded and
 * the backbuffer cleared; it draws into framebuffer 0 at the viewport and
 * returns true, or returns false to fall back to the presenter's own path.
 * NULL removes it. */
typedef bool (*OpenGLRendererPostHook)(unsigned texture, int tex_w, int tex_h,
                                      int vx, int vy, int vw, int vh,
                                      int drawable_w, int drawable_h);
void OpenGLRenderer_SetPostHook(OpenGLRendererPostHook hook);

/* Host-owned layer drawn OVER the post-shader picture at the frame viewport
 * (straight-alpha ARGB8888, top row first); false when none is set. Used for
 * modal menus, so the game behind them still goes through the shader. */
#include <stdint.h>
bool snesrecomp_present_overlay(const uint32_t **argb, int *w, int *h);
/* The window-resolution modal layer (draw_modal_layer), drawn 1:1 at
 * (x, y) top-left in drawable pixels. */
bool snesrecomp_present_layer(const uint32_t **argb, int *x, int *y, int *w, int *h);

#ifdef __cplusplus
}
#endif
