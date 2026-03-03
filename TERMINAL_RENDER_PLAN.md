# DQuake Terminal Render Plan

**Goal:** Run Quake in a terminal using truecolor ASCII/block character rendering, inspired by dcat-style terminal output.

## Overview

This document outlines the architecture for replacing QuakeSpasm's SDL/OpenGL video output with terminal backends. The target is playable Quake in terminals that support truecolor (24-bit color) escape codes.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Quake Engine Core                       │
│  (host.c, sv_*.c, cl_*.c, pr_*.c, world.c, etc.)            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Render Layer (render.h)                   │
│  r_*.c files (r_brush.c, r_alias.c, r_world.c, r_part.c)    │
└─────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┴───────────────┐
              │                               │
              ▼                               ▼
┌─────────────────────────┐   ┌───────────────────────────────┐
│   gl_vidsdl.c (SDL/GL)  │   │     vid_terminal.c (NEW)      │
│   [COMPILE-OPTIONAL]    │   │     Terminal Backend          │
└─────────────────────────┘   └───────────────────────────────┘
                                              │
                                              ▼
                              ┌───────────────────────────────┐
                              │    Terminal Output Backends    │
                              │  ─────────────────────────────│
                              │  1. truecolor (primary)        │
                              │     ANSI 24-bit color escapes  │
                              │  2. kitty (optional future)    │
                              │     Kitty graphics protocol    │
                              │  3. sixel (optional future)    │
                              │     Sixel graphics             │
                              └───────────────────────────────┘
```

## Video Driver Interface

The Quake engine calls into the video driver through functions defined in `vid.h`:

- `VID_Init()` - Initialize video subsystem
- `VID_Shutdown()` - Clean up
- `VID_Update(vrect_t *rects)` - Flush framebuffer to screen
- `VID_GetWindow()` - Get window handle (unused for terminal)
- `VID_HasMouseOrInputFocus()` - Input focus state
- `VID_IsMinimized()` - Minimized state (unused for terminal)

For terminal mode, `vid_terminal.c` implements these with terminal-specific logic.

## Frame Conversion Strategy

### Stage 1: Truecolor Block Characters (Primary Target)

Render each pixel block (2-character cell for aspect) using:

1. **Downsample**: Scale framebuffer to terminal dimensions (typically 80-240 cols × 25-70 rows)
2. **Character Selection**: Use Unicode block characters for texture:
   - `█` (full block) - solid color
   - `▀` ▄` (half blocks) - two-color cells
   - `░` `▒` `▓` - gradient/dithered effects
3. **Color Mapping**: Use ANSI truecolor escapes:
   - `\x1b[38;2;R;G;Gm` (foreground)
   - `\x1b[48;2;R;G;Bm` (background)

### Stage 2: Optimized Diff Updates

Don't redraw entire screen each frame:
- Track cell-by-cell changes in a "dirty" buffer
- Use cursor positioning (`\x1b[row;colH`) to update only changed cells
- Reset terminal state efficiently at frame boundaries

### Stage 3: Kitty/Sixel (Future Extensions)

For terminals supporting graphics protocols:
- **Kitty**: Encode framebuffer as base64 PNG, use inline images
- **Sixel**: Emit sixel commands for proper bitmap rendering

These provide much higher fidelity but reduce portability.

## Input Loop Changes

Terminal input requires different handling than SDL:

### Current (SDL)
- Event-driven: `SDL_PollEvent()` in `in_sdl.c`
- Full keyboard/mouse with SDL keycodes

### Terminal Mode
- Raw stdin read with termios (disable canonical mode, echo)
- ANSI escape sequence parsing for:
  - Arrow keys (`\x1b[A/B/C/D`)
  - Function keys (`\x1bOP` through `\x1b[19~`)
  - Special keys (Home, End, PgUp, etc.)
- Mouse input: 
  - Basic: Terminal scroll wheel (rarely useful)
  - Full: SGR mouse mode (`\x1b[?1006h`) for click/drag support

### Input Mapping

Create `in_terminal.c` implementing the input interface:

```c
// Key mapping from terminal sequences to Quake keycodes
static struct {
    const char *sequence;
    int quake_key;
} key_map[] = {
    {"\x1b[A", K_UPARROW},
    {"\x1b[B", K_DOWNARROW},
    {"\x1b[C", K_RIGHTARROW},
    {"\x1b[D", K_LEFTARROW},
    {"\x1b[1~", K_HOME},
    {"\x1b[4~", K_END},
    // ... etc
};
```

## Build System Integration

Add compile-time option `USE_TERMINAL=1` to Makefile:

```makefile
ifeq ($(USE_TERMINAL),1)
CFLAGS += -DUSE_TERMINAL
SYSOBJ_GL_VID := vid_terminal.o
SYSOBJ_INPUT := in_terminal.o
# No SDL required for terminal build
LIBS := -lm $(NET_LIBS) $(CODECLIBS)
else
# Standard SDL/GL build
SYSOBJ_GL_VID := gl_vidsdl.o
SYSOBJ_INPUT := in_sdl.o
endif
```

## Rendering Pipeline Changes

Current QuakeSpasm:
```
Game State → OpenGL Rendering → SDL Window Surface → Display
```

DQuake Terminal:
```
Game State → Software Rendering → Framebuffer (byte[])
           → Downsample to terminal grid
           → Apply character/color mapping
           → ANSI escape output → stdout
```

### Software Rendering Requirement

The existing `gl_*.c` files use OpenGL. For terminal output, we need:

1. **Option A**: Add a software renderer (like original Quake's `r_*.c` software paths)
   - More work, but complete solution
   - Can leverage existing Quake software rendering code

2. **Option B**: Render to offscreen buffer, then read pixels back
   - Requires OpenGL context (headless EGL or similar)
   - Defeats the "terminal-only" goal

3. **Option C**: Start with headless software renderer stub
   - Minimal initial implementation
   - Game logic runs, but no visible rendering yet
   - Add proper software rendering incrementally

**Recommendation**: Start with Option C. Create a software rendering path that:
- Computes what should be visible
- Outputs a text-mode approximation (top-down view, status, movement info)
- Later: Add proper framebuffer-based rendering

## Synchronization & Frame Rate

- Terminal updates are much slower than 60fps
- Cap frame rate to terminal capability (30fps max for most terminals)
- Use frame skipping: update terminal every N game frames
- Consider async output thread for smooth rendering

## Threading Considerations

- Terminal output is blocking (write to stdout)
- Game logic runs in main thread
- Consider double-buffering: render to buffer A while buffer B displays

## Phase 1 Implementation Checklist

- [x] Clone QuakeSpasm base
- [ ] Add `vid_terminal.c` scaffold
- [ ] Add `in_terminal.c` scaffold  
- [ ] Test build with `USE_TERMINAL=1`
- [ ] Implement basic terminal init/shutdown
- [ ] Implement raw mode for input
- [ ] Implement ESC sequence parser for keys
- [ ] Implement initial truecolor output (framebuffer → ANSI)
- [ ] Test with actual game data (PAK files)

## Terminal Compatibility

### Known Good Terminals

- **kitty**: Full graphics support, truecolor, inline images
- **Alacritty**: Truecolor, good performance
- **gnome-terminal**: Truecolor, decent performance
- **xterm**: Limited to 256 colors in most configs
- **tmux/screen**: Passthrough may work; test needed

### Feature Detection

Implement `Terminal_DetectCapabilities()`:
```c
typedef struct {
    qboolean has_truecolor;    // 24-bit color
    qboolean has_kitty;        // Kitty graphics
    qboolean has_sixel;         // Sixel graphics  
    qboolean has_mouse;        // SGR mouse mode
    int cols, rows;            // Terminal size
} terminal_caps_t;
```

Query `$TERM`, `$COLORTERM`, and terminal responses to detect features.

## Next Steps After Baseline

1. Get game running headless (no rendering, just input/logic)
2. Add status display (position, health, ammo as text)
3. Add ASCII art rendering of world state
4. Add proper downsampled framebuffer output
5. Optimize with change-detection
6. Add mouse support (SGR mode)
7. Add Kitty/Sixel backends

---

## References

- QuakeSpasm Source: https://github.com/Novum/quakespasm
- ANSI Escape Codes: https://en.wikipedia.org/wiki/ANSI_escape_code
- Kitty Graphics Protocol: https://sw.kovidgoyal.net/kitty/graphics-protocol/
- Sixel: https://github.com/saitoha/libsixel