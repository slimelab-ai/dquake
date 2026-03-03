# DQuake - Quake in Your Terminal

A modified QuakeSpasm designed to run Quake directly in your terminal using truecolor ANSI escape sequences.

## Current Status: RUNNABLE BOOTSTRAP

The terminal build now compiles and runs! The architecture is complete with:
- Terminal video driver (`vid_terminal.c`)
- Terminal input driver (`in_terminal.c`)  
- Software renderer stubs (`sw_stub.c`, `sw_model.h`, `swquake.h`)
- GL compatibility layer (`gl_compat.h`)

### What Works
- ✅ **Builds successfully** - `make -f Makefile.terminal` produces executable
- ✅ **Starts without crashing** - Basic initialization works
- ✅ **Game logic running** - Host frame loop executes
- 🚧 **Terminal rendering** - Scaffold ready, needs real output

### What's Partial
- ⚠️ **No video output yet** - Terminal driver runs but doesn't render frames
- ⚠️ **Sound disabled** - Sound DMA stubbed out

## Quick Start

```bash
# Build
make -f Makefile.terminal

# Run with the runner script (auto-detects Quake data)
./run_dquake.sh

# Or run directly with explicit path
./dquake-terminal -basedir /path/to/quake
```

### Getting Quake Data

You need the original Quake data files:
1. Buy Quake on Steam or GOG
2. Extract `pak0.pak` and `pak1.pak` to `id1/` directory
3. Point DQuake to the installation directory

## Architecture Overview

```
Quake Engine Core (unchanged)
       │
       ▼
┌─────────────────────────────────┐
│  sw_stub.c (NEW)                │
│  - Stub GL/software functions   │
│  - Model loading stubs          │
│  - Rendering no-ops             │
└─────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────┐
│  vid_terminal.c                  │
│  - Terminal video driver        │
│  - Framebuffer → ANSI output   │
│  - Truecolor escape sequences   │
└─────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────┐
│  in_terminal.c                  │
│  - Terminal input driver        │
│  - Raw mode stdin              │
│  - ESC sequence parser          │
└─────────────────────────────────┘
```

### Key Files

| File | Purpose | Status |
|------|---------|--------|
| `sw_stub.c` | GL/rendering stubs | ✅ Complete |
| `sw_model.h` | Software model types | ✅ Complete |
| `swquake.h` | Software renderer decls | ✅ Complete |
| `gl_compat.h` | GL function/type stubs | ✅ Complete |
| `vid_terminal.c` | Terminal video driver | ⚠️ Scaffold |
| `in_terminal.c` | Terminal input driver | ⚠️ Scaffold |
| `sys_terminal.c` | Terminal system funcs | ✅ Complete |
| `main_terminal.c` | Entry point | ✅ Complete |

## Building

```bash
# Terminal build
make -f Makefile.terminal

# Clean
make -f Makefile.terminal clean

# Build produces: dquake-terminal
```

## How It Works

DQuake uses extensive GL stubbing to run Quake's game logic without actual rendering:

1. **GL Compatibility Layer** (`gl_compat.h`): Provides empty inline functions for all OpenGL calls
2. **Software Stubs** (`sw_stub.c`): Implements required render callbacks as no-ops
3. **Model Types** (`sw_model.h`): Software-renderer-compatible model structures
4. **Terminal Driver** (`vid_terminal.c`): Handles terminal setup and potential output

## Next Steps for Full Rendering

To get actual visual output:

1. **Implement Framebuffer Capture**: The game logic runs but frames aren't rendered
2. **Add Software Renderer**: Either port from original Quake or write new one
3. **Connect to Terminal**: Link renderer output to `VID_Update()` in `vid_terminal.c`

See [TERMINAL_RENDER_PLAN.md](TERMINAL_RENDER_PLAN.md) for detailed roadmap.

## Tested Output

```
$ ./dquake-terminal -basedir ~/Games/quake
VID_Init: Initializing terminal video driver
VID_Init: Terminal size 180x56
Terminal: TERM=xterm-256color COLORTERM=truecolor truecolor=yes
Mod_Init: Terminal mode
R_Init: Terminal mode initialized
SNDDMA_Init: No sound in terminal mode

DQuake Terminal Mode - Frame Update
```

The game loop runs correctly; only terminal rendering output is missing.

## QuakeSpasm Attribution

DQuake is based on [QuakeSpasm](https://github.com/Novum/quakespasm), a modern Quake engine.

## License

GNU General Public License v2.0 or later - see [LICENSE.txt](LICENSE.txt)