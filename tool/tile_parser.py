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

        # Define tiles with actual image paths
        self.special_tiles = {
            '@': TileDefinition('@', '@', "MetroHero/assets/person_down.png", True, "Spawn Point"),
            '.': TileDefinition('.', '·', "MetroHero/assets/floor.png", True, "Floor"),
            '#': TileDefinition('#', '█', "MetroHero/assets/wall.png", False, "Wall"),
            '=': TileDefinition('=', '═', "MetroHero/assets/floor.png", True, "Rail"),
            '+': TileDefinition('+', '┼', "MetroHero/assets/door.png", True, "Door"),
            '$': TileDefinition('$', '▓', "MetroHero/assets/door.png", False, "Event Door"),
        }

        # Add lowercase letters for enemies (a, b, c with known images)
        enemy_images = {
            'a': "MetroHero/assets/enemy/texture/cat.png",
            'b': "MetroHero/assets/enemy/texture/security_robot.png",
            'c': "MetroHero/assets/enemy/texture_sprite/1c.png",
        }
        for i in range(ord('a'), ord('z') + 1):
            c = chr(i)
            img_path = enemy_images.get(c, None)
            self.special_tiles[c] = TileDefinition(c, c, img_path, True, f"Enemy {c.upper()}")

        # Add uppercase letters for NPCs
        npc_images = {
            'A': "MetroHero/assets/npc/1A_face.png",
            'B': "MetroHero/assets/old_man.png",
            'C': "MetroHero/assets/citizen_black.png",
        }
        for i in range(ord('A'), ord('Z') + 1):
            c = chr(i)
            img_path = npc_images.get(c, None)
            self.special_tiles[c] = TileDefinition(c, c, img_path, True, f"NPC {c}")

        # Add numbers for chests
        for i in range(10):
            c = str(i)
            self.special_tiles[c] = TileDefinition(c, c, "MetroHero/assets/chest_closed.png", True, f"Chest {c}")

    def parse_glyph_h(self):
        """Parse glyph.h for tile definitions (if needed for future expansion)"""
        glyph_path = os.path.join(self.project_root, "MetroHero", "src", "world", "glyph.h")
        if not os.path.exists(glyph_path):
            print(f"Warning: {glyph_path} not found")
            return

        # For now, we use the hardcoded special_tiles
        # Future: could parse actual definitions from glyph.h if format changes
        pass

    def get_tile(self, symbol):
        """Get tile definition for a symbol"""
        return self.special_tiles.get(symbol)

    def get_all_common_tiles(self):
        """Get commonly used tiles for the palette"""
        common = ['#', '.', '=', '+', '$', '@']
        return [self.special_tiles[c] for c in common if c in self.special_tiles]

    def get_enemy_tiles(self):
        """Get enemy tiles (a-z)"""
        return [self.special_tiles[chr(i)] for i in range(ord('a'), ord('z') + 1)]

    def get_npc_tiles(self):
        """Get NPC tiles (A-Z)"""
        return [self.special_tiles[chr(i)] for i in range(ord('A'), ord('Z') + 1)]

    def get_chest_tiles(self):
        """Get chest tiles (0-9)"""
        return [self.special_tiles[str(i)] for i in range(10)]
