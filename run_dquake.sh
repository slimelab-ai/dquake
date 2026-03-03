#!/bin/bash
# run_dquake.sh - DQuake Terminal Runner
# Automatically detects or asks for pak0.pak path

set -e

# Binary name
DQUAKE_BIN="./dquake-terminal"

# Check if binary exists
if [ ! -x "$DQUAKE_BIN" ]; then
    echo "Error: $DQUAKE_BIN not found or not executable"
    echo "Run 'make -f Makefile.terminal' first"
    exit 1
fi

# Common paths to check for Quake data
SEARCH_PATHS=(
    "$HOME/.quake"
    "$HOME/quake"
    "$HOME/.local/share/quake"
    "/usr/share/quake"
    "/usr/local/share/quake"
    "/opt/quake"
    "$HOME/Games/quake"
    "$HOME/.steam/steam/steamapps/common/quake/quake"
    "$HOME/.var/app/com.valvesoftware.Steam/.local/share/Steam/steamapps/common/quake/quake"
    "/mnt/c/Program Files/Steam/steamapps/common/quake/quake"
    "/mnt/d/Program Files/Steam/steamapps/common/quake/quake"
)

# Check for pak0.pak
find_pak() {
    for dir in "${SEARCH_PATHS[@]}"; do
        if [ -f "$dir/id1/pak0.pak" ]; then
            echo "$dir"
            return 0
        fi
    done
    return 1
}

# Main logic
if [ -n "$QUAKE_PATH" ]; then
    QUAKE_PATH="$QUAKE_PATH"
elif find_pak >/dev/null 2>&1; then
    QUAKE_PATH=$(find_pak)
    echo "Found Quake data at: $QUAKE_PATH"
else
    echo ""
    echo "=== DQuake Terminal Mode ==="
    echo ""
    echo "No Quake data (pak0.pak) found in common locations."
    echo ""
    echo "Please enter the path to your Quake installation."
    echo "This is typically the directory containing the 'id1' folder."
    echo "You can also set the QUAKE_PATH environment variable."
    echo ""
    echo "Examples:"
    echo "  /home/you/.quake"
    echo "  /home/you/Games/quake"
    echo "  /path/to/steam/steamapps/common/quake/quake"
    echo ""
    read -p "Quake path (or 'quit' to exit): " QUAKE_PATH
    
    if [ "$QUAKE_PATH" = "quit" ] || [ -z "$QUAKE_PATH" ]; then
        echo "No path provided, exiting."
        exit 0
    fi
fi

# Verify pak0.pak exists
if [ ! -f "$QUAKE_PATH/id1/pak0.pak" ]; then
    echo "Error: $QUAKE_PATH/id1/pak0.pak not found"
    echo "Please make sure you have the Quake data files installed."
    exit 1
fi

echo ""
echo "Starting DQuake Terminal Mode..."
echo "Data directory: $QUAKE_PATH"
echo "Binary: $DQUAKE_BIN"
echo ""

# Run the game
cd "$QUAKE_PATH"
exec "$OLDPWD/$DQUAKE_BIN" -basedir "$QUAKE_PATH" "$@"