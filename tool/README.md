# MetroHero Map Editor

Python-based WYSIWYG tile map editor for creating MetroHero game maps.

## Features

- **Visual Tile Palette**: Select tiles using radio buttons
- **WYSIWYG Canvas**: Draw maps by clicking and dragging
- **Export**: Generate `MAP_LINES[]` C code for direct paste into stage files
- **Import**: Load existing `MAP_LINES[]` code for editing
- **Color-Coded Tiles**: Easy visual distinction between tile types

## Installation

1. Ensure Python 3.8+ is installed
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

## Usage

### Starting the Editor

```bash
cd tool
python map_editor.py
```

### Controls

1. **Tile Palette (Top)**: Click radio buttons to select the tile you want to draw
   - Common Tiles: `#` (wall), `.` (floor), `=` (rail), `+` (door), `$` (event door), `@` (spawn)
   - Enemies: `a-z` (lowercase letters)
   - NPCs: `A-Z` (uppercase letters)
   - Chests: `0-9` (numbers)

2. **Map Canvas (Center)**:
   - Click to place selected tile
   - Click and drag to paint multiple tiles
   - Scroll to navigate large maps

3. **Control Panel (Right)**:
   - **Resize Map**: Change map dimensions
   - **Clear Map**: Fill entire map with floor tiles
   - **Fill with Floor**: Reset to all floor tiles
   - **Export MAP_LINES[]**: Generate C code for copy-paste
   - **Import MAP_LINES[]**: Load existing map data

### Exporting Maps

1. Click **Export MAP_LINES[]** button
2. Copy the generated C code from the popup window
3. Paste directly into your stage file (e.g., `stage_01.c`)

Example output:
```c
static const char* MAP_LINES[] = {
    "#######################",
    "#...B................0#",
    "#.@..............A...1#",
    "#.......C............2#",
    "###########.###########",
};
```

### Importing Maps

1. Click **Import MAP_LINES[]** button
2. Paste existing `MAP_LINES[]` code
3. Click **Import** to load the map for editing

## Tile Color Guide

- **Gray (#808080)**: Wall (`#`)
- **Dark Gray (#2C2C2C)**: Floor (`.`)
- **Gold (#FFD700)**: Rail (`=`)
- **Brown (#8B4513)**: Door (`+`)
- **Indigo (#4B0082)**: Event Door (`$`)
- **Green (#00FF00)**: Spawn Point (`@`)
- **Red (#FF4444)**: Enemies (lowercase letters)
- **Blue (#4444FF)**: NPCs (uppercase letters)
- **Yellow (#FFFF44)**: Chests (numbers)

## Tips

- Start with **Fill with Floor** to create a base
- Use wall (`#`) to create boundaries
- Place spawn point (`@`) where the player starts
- Add enemies, NPCs, and chests as needed
- Test your map by exporting and running the game

## Troubleshooting

- If tiles don't appear correctly, ensure the `MetroHero/src/` directory structure is intact
- For very large maps (>100x100), performance may degrade
- Always test exported maps in the game to verify layout

## Future Enhancements

- Image preview for tiles (load actual PNG assets)
- Undo/Redo functionality
- Fill tool (flood fill)
- Layer support (background/foreground)
- Validation (check for spawn point, warn about unreachable areas)
