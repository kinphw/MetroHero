"""
Tile Definition Parser
Reads tile definitions from MetroHero source code (glyph.h)
"""
import os
import re

class TileDefinition:
    def __init__(self, symbol, glyph, image_path, walkable, desc):
        self.symbol = symbol
        self.glyph = glyph
        self.image_path = image_path  # Relative path from project root
        self.walkable = walkable
        self.desc = desc

    def __repr__(self):
        return f"Tile('{self.symbol}', {self.desc}, {self.image_path})"

class TileParser:
    def __init__(self, project_root):
        self.project_root = project_root
        self.tiles = {}

        # Start with minimal tiles (space and placeholders)
        self.special_tiles = {
            ' ': TileDefinition(' ', ' ', None, True, "Empty Space"),
        }

        # Add lowercase letters for enemies (placeholders, will be overridden by stage data)
        for i in range(ord('a'), ord('z') + 1):
            c = chr(i)
            self.special_tiles[c] = TileDefinition(c, c, None, True, f"Enemy {c.upper()}")

        # Add uppercase letters for NPCs (placeholders, will be overridden by stage data)
        for i in range(ord('A'), ord('Z') + 1):
            c = chr(i)
            self.special_tiles[c] = TileDefinition(c, c, None, True, f"NPC {c}")

    def parse_map_data_c(self):
        """Parse map_data.c for GLOBAL_TILE_PALETTE definitions"""
        map_data_path = os.path.join(self.project_root, "MetroHero", "src", "world", "map_data.c")
        if not os.path.exists(map_data_path):
            print(f"Warning: {map_data_path} not found")
            return

        try:
            with open(map_data_path, 'r', encoding='utf-8') as f:
                content = f.read()

            # Parse TileDef entries: { 'c', "path", walkable, "desc" }
            tile_pattern = r'\{\s*\'(.)\'\s*,\s*"([^"]+)"\s*,\s*(\d+)\s*,\s*"([^"]+)"\s*\}'

            for match in re.finditer(tile_pattern, content):
                symbol = match.group(1)
                image_path = match.group(2)
                walkable = int(match.group(3))
                desc = match.group(4)

                # Convert to full path with MetroHero prefix
                if not image_path.startswith("MetroHero/"):
                    image_path = "MetroHero/" + image_path

                # Update or create tile definition
                self.special_tiles[symbol] = TileDefinition(
                    symbol=symbol,
                    glyph=symbol,
                    image_path=image_path,
                    walkable=bool(walkable),
                    desc=desc
                )
                print(f"  Loaded tile '{symbol}': {desc} -> {image_path}")

        except Exception as e:
            print(f"Error parsing {map_data_path}: {e}")

    def parse_glyph_h(self):
        """Parse glyph.h for tile definitions (if needed for future expansion)"""
        glyph_path = os.path.join(self.project_root, "MetroHero", "src", "world", "glyph.h")
        if not os.path.exists(glyph_path):
            print(f"Warning: {glyph_path} not found")
            return

        # For now, we use the hardcoded special_tiles
        # Future: could parse actual definitions from glyph.h if format changes
        pass

    def get_available_stages(self):
        """Get list of available stage names"""
        stages = []
        stages_root = os.path.join(self.project_root, "MetroHero", "src", "stages")

        if os.path.exists(stages_root):
            for item in os.listdir(stages_root):
                stage_dir = os.path.join(stages_root, item)
                if os.path.isdir(stage_dir) and item.startswith("stage_"):
                    stage_file = os.path.join(stage_dir, item + ".c")
                    if os.path.exists(stage_file):
                        stages.append(item)

        return sorted(stages)

    def parse_stage_configs(self):
        """Parse ALL stage files to extract enemy and NPC image paths (legacy)"""
        stage_dirs = [
            os.path.join(self.project_root, "MetroHero", "src", "stages", "stage_01"),
            os.path.join(self.project_root, "MetroHero", "src", "stages", "stage_02"),
            os.path.join(self.project_root, "MetroHero", "src", "stages", "stage_03"),
        ]

        for stage_dir in stage_dirs:
            stage_file = os.path.join(stage_dir, os.path.basename(stage_dir) + ".c")
            if os.path.exists(stage_file):
                self._parse_stage_file(stage_file)

    def parse_specific_stage(self, stage_name):
        """Parse a specific stage file and return enemy/NPC configs"""
        stage_dir = os.path.join(self.project_root, "MetroHero", "src", "stages", stage_name)
        stage_file = os.path.join(stage_dir, stage_name + ".c")

        if not os.path.exists(stage_file):
            print(f"Stage file not found: {stage_file}")
            return {}, {}

        return self._parse_stage_file_detailed(stage_file)

    def _parse_stage_file(self, filepath):
        """Parse a single stage file for enemy and NPC configs (legacy method)"""
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()

            # Parse EnemyConfig entries
            enemy_pattern = r'\.tile\s*=\s*\'([a-z])\'\s*,.*?\.imagePath\s*=\s*"([^"]+)"'
            for match in re.finditer(enemy_pattern, content, re.DOTALL):
                tile_char = match.group(1)
                image_path = match.group(2)

                # Convert to full path with MetroHero prefix
                if not image_path.startswith("MetroHero/"):
                    image_path = "MetroHero/" + image_path

                # Update the tile definition
                if tile_char in self.special_tiles:
                    self.special_tiles[tile_char].image_path = image_path
                    print(f"  Updated enemy '{tile_char}': {image_path}")

            # Parse NPCConfig entries
            npc_pattern = r'\.tile\s*=\s*\'([A-Z])\'\s*,.*?\.imagePath\s*=\s*"([^"]+)"'
            for match in re.finditer(npc_pattern, content, re.DOTALL):
                tile_char = match.group(1)
                image_path = match.group(2)

                # Convert to full path with MetroHero prefix
                if not image_path.startswith("MetroHero/"):
                    image_path = "MetroHero/" + image_path

                # Update the tile definition
                if tile_char in self.special_tiles:
                    self.special_tiles[tile_char].image_path = image_path
                    print(f"  Updated NPC '{tile_char}': {image_path}")

        except Exception as e:
            print(f"Error parsing {filepath}: {e}")

    def _parse_stage_file_detailed(self, filepath):
        """Parse a single stage file and return detailed enemy/NPC configs"""
        enemies = {}  # {tile_char: {name, imagePath, ...}}
        npcs = {}     # {tile_char: {name, imagePath, ...}}

        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                lines = f.readlines()

            # Parse line by line, building up tile data
            current_tile = None
            current_name = None
            current_image = None
            in_struct = False

            for line in lines:
                # Detect start of struct
                if '{' in line and not in_struct:
                    in_struct = True
                    current_tile = None
                    current_name = None
                    current_image = None

                # Extract .tile
                tile_match = re.search(r'\.tile\s*=\s*\'([a-zA-Z])\'', line)
                if tile_match:
                    current_tile = tile_match.group(1)

                # Extract .name
                name_match = re.search(r'\.name\s*=\s*"([^"]*)"', line)
                if name_match:
                    current_name = name_match.group(1)

                # Extract .imagePath
                image_match = re.search(r'\.imagePath\s*=\s*"([^"]+)"', line)
                if image_match:
                    current_image = image_match.group(1)

                # Detect end of struct
                if '}' in line and in_struct:
                    in_struct = False

                    # Save the collected data
                    if current_tile and current_name and current_image:
                        image_path = current_image
                        if not image_path.startswith("MetroHero/"):
                            image_path = "MetroHero/" + image_path

                        if current_tile.islower():
                            # Enemy
                            enemies[current_tile] = {
                                'name': current_name,
                                'imagePath': image_path
                            }
                        elif current_tile.isupper():
                            # NPC
                            npcs[current_tile] = {
                                'name': current_name,
                                'imagePath': image_path
                            }

                    current_tile = None
                    current_name = None
                    current_image = None

            print(f"Parsed {os.path.basename(filepath)}: {len(enemies)} enemies, {len(npcs)} NPCs")

        except Exception as e:
            print(f"Error parsing {filepath}: {e}")

        return enemies, npcs

    def get_tile(self, symbol):
        """Get tile definition for a symbol"""
        return self.special_tiles.get(symbol)

    def get_all_common_tiles(self):
        """Get commonly used tiles for the palette"""
        # Get all non-enemy, non-NPC, non-chest tiles
        common_tiles = []
        for symbol, tile_def in self.special_tiles.items():
            # Skip space, enemies (lowercase), NPCs (uppercase), chests (digits)
            if symbol == ' ':
                continue
            if symbol.islower() or symbol.isupper() or symbol.isdigit():
                continue
            common_tiles.append(tile_def)

        # Sort by symbol for consistent display
        return sorted(common_tiles, key=lambda t: t.symbol)

    def get_enemy_tiles(self):
        """Get enemy tiles (a-z)"""
        return [self.special_tiles[chr(i)] for i in range(ord('a'), ord('z') + 1)]

    def get_npc_tiles(self):
        """Get NPC tiles (A-Z)"""
        return [self.special_tiles[chr(i)] for i in range(ord('A'), ord('Z') + 1)]

    def get_chest_tiles(self):
        """Get chest tiles (0-9)"""
        chests = []
        for i in range(10):
            c = str(i)
            if c in self.special_tiles:
                chests.append(self.special_tiles[c])
        return chests
