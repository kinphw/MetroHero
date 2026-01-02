# MetroHero Map Editor

Python-based WYSIWYG tile map editor for creating MetroHero game maps.

## Features

- **Visual Tile Palette**: Select common tiles using radio buttons
- **Keyboard Input**: Type any character directly onto the map (a-z, A-Z, 0-9, symbols)
- **WYSIWYG Canvas**: Draw maps by clicking, dragging, or typing
- **Real PNG Preview**: See actual game assets rendered on tiles
- **Export**: Generate `MAP_LINES[]` C code for direct paste into stage files
- **Import**: Load existing `MAP_LINES[]` code for editing

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

#### Tile Palette (Right Panel)
- **Custom Character**: Type any character (a-z, A-Z, 0-9, symbols) and click "Use this character"
  - Auto-selects when you type
  - Perfect for enemies (`a`, `b`, `c`), NPCs (`A`, `B`, `C`), doors (`+`, `*`, `^`)
- **Common Tiles**: Click radio buttons to select tiles for mouse painting
  - Floors: `.` `,` `-` `_` `~` (different floor types)
  - Walls: `#` `%` `&` `$` (different wall types)
  - Objects: `/` `T` `t` `B` `!` `O` (doors, trees, barricades, etc.)
  - Special: `=` `|` (rails), `<` `>` (stairs), `@` (spawn)
- **Chests**: `0-9` number tiles

#### Map Canvas (Left Panel)

**Mouse Controls:**
- **Left Click**: Place selected tile/character
- **Left Click + Drag**: Paint multiple tiles
- **Right Click**: Erase (fill with space)
- **Right Click + Drag**: Erase multiple tiles

**Map Controls:**
- **Size**: Set width and height, then click **Resize**
- **Clear**: Fill entire map with spaces
- **Fill Floor**: Fill entire map with floor tiles (`.`)
- **Export MAP_LINES[]**: Generate C code for copy-paste
- **Import MAP_LINES[]**: Load existing map data

### Workflow Example

1. Set map size (e.g., 40x25)
2. Click "Fill Floor" to create a floor base
3. Select wall (`#`) from palette and click+drag to create boundaries
4. Type character in "Custom Character" field (e.g., `a`, `A`, `+`)
5. The radio button auto-selects, then click on map to place
6. Switch between custom characters and tile palette as needed
7. Click "Export MAP_LINES[]" and copy code to your stage file

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
    "          #+#          ",
    "          #a#          ",
    "#######################"
};
```

### Importing Maps

1. Click **Import MAP_LINES[]** button
2. Paste existing `MAP_LINES[]` code
3. Click **Import** to load the map for editing

## Tile Reference

### From map_data.c (GLOBAL_TILE_PALETTE)
The editor reads tile definitions from `/MetroHero/src/world/map_data.c` and displays their actual PNG images.

**Floors:**
- `.` - Tiled Floor
- `,` - Grass Floor
- `-` - Gravel Floor
- `_` - Dirt Floor
- `~` - Water

**Walls:**
- `#` - Gray Wall
- `%` - Brick Wall
- `&` - Iron Wall
- `$` - Glass Wall

**Objects:**
- `+` `*` `^` - Doors (various types)
- `/` - Open Door
- `T` `t` - Trees (dark/light)
- `B` - Barricade
- `O` - Rock
- `!` - Signpost

**Rails & Stairs:**
- `=` - Rail (Horizontal)
- `|` - Rail (Vertical)
- `<` - Stairs Down
- `>` - Stairs Up

**Special:**
- `@` - Spawn Point
- `0-9` - Chests

### Stage-Specific (Manual Entry via Keyboard)
These are defined in individual stage files and should be typed directly:

**Enemies (stage_XX.c):**
- `a`, `b`, `c`, etc. - Enemy placements

**NPCs (stage_XX.c):**
- `A`, `B`, `C`, etc. - NPC placements

## Tips

- **Custom Character Input**: Type in the field, it auto-selects, then click to place
  - Type `a` once, click multiple times to place multiple enemies
  - Change to `b`, click to place different enemy
- **Use Palette for Tiles**: Click once, drag to paint walls and floors
- **Right-Click to Erase**: Quick way to clear mistakes
- **Check stage_XX.c**: Reference stage files to see what a, b, A, B represent

## Architecture

- **tile_parser.py**: Parses `map_data.c` for tile definitions
- **map_editor.py**: Main GUI application (Tkinter)
- **No Stage Parsing**: Enemy/NPC data is NOT read from stage files (type manually)
- **Image Loading**: Real PNG assets from `/MetroHero/assets/` directory

## Performance

- Optimized to only load tiles from `map_data.c` (no complex stage parsing)
- Fast startup and low memory usage
- Supports maps up to 100x100 tiles

## Troubleshooting

- **Custom character not placing**: Make sure the radio button is selected (auto-selects when typing)
- **Missing images**: Ensure PNG files exist in `/MetroHero/assets/`
- **Import/Export buttons hidden**: Resize window or scroll controls panel

## Future Enhancements

- Undo/Redo functionality
- Fill tool (flood fill)
- Layer support (background/foreground)
- Validation (check for spawn point, warn about unreachable areas)
