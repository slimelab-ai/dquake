# DQuake - Quake in Your Terminal

A modified QuakeSpasm designed to run Quake directly in your terminal using truecolor ANSI escape sequences.

**Status: BOOTSTRAP** - Terminal video/input drivers scaffolded, software renderer needed for full functionality.

## What is DQuake?

DQuake brings the classic Quake experience to the terminal. Instead of OpenGL/SDL, it uses terminal escape sequences to render the game:

- **Truecolor output**: 24-bit ANSI color for rich visuals
- **Block characters**: Unicode █, ▀, ▄ for text-mode graphics  
- **Terminal input**: Raw mode keyboard handling with escape sequence parsing
- **Future**: Kitty graphics protocol and Sixel support for higher fidelity

## Architecture

```
Quake Engine Core
       │
       ▼
┌─────────────────────────────────┐
│  vid_terminal.c (NEW)           │
│  - Terminal video driver        │
│  - Framebuffer → ANSI output    │
│  - Truecolor character mapping  │
└─────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────┐
│  in_terminal.c (NEW)            │
│  - Terminal input driver       │
│  - Raw mode stdin              │
│  - ESC sequence parser          │
└─────────────────────────────────┘
```

See **[TERMINAL_RENDER_PLAN.md](TERMINAL_RENDER_PLAN.md)** for the full technical design.

## Current Status

### ✅ Complete
- Terminal video driver scaffold (`vid_terminal.c`)
- Terminal input driver scaffold (`in_terminal.c`)  
- System integration for terminal (`sys_terminal.c`, `main_terminal.c`)
- Type stubs for non-GL build (`terminal_types.h`)
- Conditional compilation flags (`USE_TERMINAL`)

### 🚧 In Progress
- Software renderer implementation (needed to replace OpenGL `gl_*` files)
- Framebuffer downsampling for terminal dimensions
- Truecolor ANSI escape output

### 📋 Planned
- Kitty graphics protocol support
- Sixel graphics support
- Mouse input (SGR mode)
- Performance optimization (dirty cell updates)

## Building

### Full Build (Requires Software Renderer)

The complete build requires a software renderer to replace the OpenGL-based `gl_*.c` files. This is the main remaining work.

```bash
# Standard SDL/OpenGL build (works)
make

# Terminal build (requires software renderer - WIP)
make -f Makefile.terminal
```

### Compiling Terminal Components

To verify the terminal-specific code compiles:

```bash
# Compile just the terminal components
gcc -c -Wall -DUSE_TERMINAL -DNDEBUG -O2 \
    vid_terminal.c in_terminal.c sys_terminal.c main_terminal.c \
    -I.
```

## Project Structure

```
dquake/
├── Quake/                          # Original QuakeSpasm source
│   ├── gl_*.c                      # OpenGL renderer (needs replacement)
│   ├── vid_terminal.c              # Terminal video driver (NEW)
│   ├── in_terminal.c               # Terminal input driver (NEW)
│   ├── sys_terminal.c              # Terminal system functions (NEW)
│   ├── main_terminal.c             # Terminal main entry (NEW)
│   ├── terminal_types.h            # GL type stubs (NEW)
│   └── TERMINAL_RENDER_PLAN.md     # Technical design doc (NEW)
├── Makefile                        # Standard build (SDL/OpenGL)
├── Makefile.terminal               # Terminal build (partial)
└── README.md                       # This file
```

## QuakeSpasm Attribution

DQuake is based on [QuakeSpasm](https://github.com/Novum/quakespasm), a modern, maintained Quake engine derived from FitzQuake.

QuakeSpasm credits:
- John Fitzgibbons (FitzQuake)
- Kristian Duske
- Ozkan Sezer, Eric Wasylishen & others (QuakeSpasm maintainers)

Original Quake by id Software (1996).

## License

GNU General Public License v2.0 or later - see [LICENSE.txt](LICENSE.txt)

The terminal-specific additions (`vid_terminal.c`, `in_terminal.c`, etc.) are also GPL v2+.

## Contributing

1. Fork the repo
2. Create a feature branch
3. Implement software renderer or other missing pieces
4. Submit a pull request

## Next Steps

See [TERMINAL_RENDER_PLAN.md](TERMINAL_RENDER_PLAN.md) for the detailed roadmap and technical architecture.

### Immediate Priorities

1. **Software Renderer**: Either port from original Quake software renderer or implement new one
2. **Framebuffer Capture**: Get pixel data from renderer to terminal
3. **Truecolor Output**: Implement full ANSI escape sequence rendering
4. **Testing**: Verify with real game data (PAK files)