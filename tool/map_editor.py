"""
MetroHero Map Editor
WYSIWYG tile-based map editor for creating MAP_LINES[] data
"""
import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox
import os
from PIL import Image, ImageTk
from tile_parser import TileParser, TileDefinition

class MapEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("MetroHero Map Editor")

        # Fullscreen
        self.root.state('zoomed')  # Windows fullscreen
        # self.root.attributes('-fullscreen', True)  # Alternative for other platforms

        # Project root (parent of tool directory)
        self.project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

        # Tile parser
        self.parser = TileParser(self.project_root)
        self.parser.parse_map_data_c()  # Load GLOBAL_TILE_PALETTE from map_data.c
        self.parser.parse_glyph_h()

        # Get available stages
        self.available_stages = self.parser.get_available_stages()
        self.current_stage = self.available_stages[0] if self.available_stages else "stage_01"

        # Stage-specific enemies and NPCs
        self.stage_enemies = {}
        self.stage_npcs = {}

        # Map data
        self.map_width = 40
        self.map_height = 25
        self.tile_size = 24  # pixels
        self.map_data = [[' ' for _ in range(self.map_width)] for _ in range(self.map_height)]

        # Current selected tile
        self.selected_tile = '#'

        # Image cache for tiles
        self.image_cache = {}  # {image_path: PhotoImage}
        self.preload_images()

        # UI Setup
        self.setup_ui()

        # Drawing state
        self.is_drawing = False
        self.last_drawn = None

    def preload_images(self):
        """Preload all tile images into cache"""
        print("Preloading tile images...")

        # Load common tiles
        for symbol, tile_def in self.parser.special_tiles.items():
            if symbol.isalnum() and (symbol.islower() or symbol.isupper()):
                continue  # Skip enemies/NPCs, load from stage data
            if tile_def.image_path:
                full_path = os.path.join(self.project_root, tile_def.image_path)
                if os.path.exists(full_path):
                    try:
                        # Load image
                        img = Image.open(full_path)

                        # Check if this is a sprite sheet (texture_sprite directory)
                        if 'texture_sprite' in tile_def.image_path or 'sprite' in tile_def.image_path.lower():
                            # Extract top-left tile from sprite sheet
                            # Assume 2x2 sprite sheet, extract first cell
                            sprite_w = img.width // 2
                            sprite_h = img.height // 2
                            img = img.crop((0, 0, sprite_w, sprite_h))
                            print(f"  Loaded sprite: {symbol} -> {tile_def.image_path} (extracted first tile)")
                        else:
                            print(f"  Loaded: {symbol} -> {tile_def.image_path}")

                        # Resize to tile_size
                        img = img.resize((self.tile_size, self.tile_size), Image.Resampling.LANCZOS)
                        photo = ImageTk.PhotoImage(img)
                        self.image_cache[tile_def.image_path] = photo

                    except Exception as e:
                        print(f"  Error loading {full_path}: {e}")
                else:
                    print(f"  Not found: {full_path}")

        # Load stage-specific enemies
        for tile_char, enemy_data in self.stage_enemies.items():
            image_path = enemy_data['imagePath']
            full_path = os.path.join(self.project_root, image_path)
            if os.path.exists(full_path) and image_path not in self.image_cache:
                try:
                    img = Image.open(full_path)

                    # Handle sprite sheets
                    if 'texture_sprite' in image_path or 'sprite' in image_path.lower():
                        sprite_w = img.width // 2
                        sprite_h = img.height // 2
                        img = img.crop((0, 0, sprite_w, sprite_h))
                        print(f"  Loaded sprite: {tile_char} ({enemy_data['name']}) -> {image_path}")
                    else:
                        print(f"  Loaded enemy: {tile_char} ({enemy_data['name']}) -> {image_path}")

                    img = img.resize((self.tile_size, self.tile_size), Image.Resampling.LANCZOS)
                    photo = ImageTk.PhotoImage(img)
                    self.image_cache[image_path] = photo
                except Exception as e:
                    print(f"  Error loading {full_path}: {e}")

        # Load stage-specific NPCs
        for tile_char, npc_data in self.stage_npcs.items():
            image_path = npc_data['imagePath']
            full_path = os.path.join(self.project_root, image_path)
            if os.path.exists(full_path) and image_path not in self.image_cache:
                try:
                    img = Image.open(full_path)
                    img = img.resize((self.tile_size, self.tile_size), Image.Resampling.LANCZOS)
                    photo = ImageTk.PhotoImage(img)
                    self.image_cache[image_path] = photo
                    print(f"  Loaded NPC: {tile_char} ({npc_data['name']}) -> {image_path}")
                except Exception as e:
                    print(f"  Error loading {full_path}: {e}")

        print(f"Preloaded {len(self.image_cache)} images")

    def setup_ui(self):
        """Setup the main UI layout"""
        # Main container
        main_frame = tk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Top: Stage Selector bar
        top_bar = tk.Frame(main_frame, bg='#f0f0f0', height=40)
        top_bar.pack(fill=tk.X, pady=(0, 5))
        top_bar.pack_propagate(False)

        tk.Label(top_bar, text="Stage:", font=('Arial', 10, 'bold'), bg='#f0f0f0').pack(side=tk.LEFT, padx=(10, 5))

        self.stage_var = tk.StringVar(value=self.current_stage)
        stage_dropdown = ttk.Combobox(top_bar, textvariable=self.stage_var,
                                      values=self.available_stages, state='readonly', width=15)
        stage_dropdown.pack(side=tk.LEFT, padx=(0, 20))
        stage_dropdown.bind('<<ComboboxSelected>>', self.on_stage_change)

        # Main content: Left (Map + Controls) + Right (Palette)
        content_frame = tk.Frame(main_frame)
        content_frame.pack(fill=tk.BOTH, expand=True)

        # Left side: Map Canvas + Controls (2/3 width)
        left_frame = tk.Frame(content_frame)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 5))

        canvas_frame = tk.Frame(left_frame)
        canvas_frame.pack(fill=tk.BOTH, expand=True)

        # Canvas with scrollbars
        canvas_container = tk.Frame(canvas_frame)
        canvas_container.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        self.canvas = tk.Canvas(canvas_container, bg='black')

        h_scroll = tk.Scrollbar(canvas_container, orient=tk.HORIZONTAL, command=self.canvas.xview)
        v_scroll = tk.Scrollbar(canvas_container, orient=tk.VERTICAL, command=self.canvas.yview)

        self.canvas.configure(xscrollcommand=h_scroll.set, yscrollcommand=v_scroll.set)

        # Set scrollregion to allow scrolling
        self.canvas.configure(scrollregion=(0, 0,
                                           self.map_width * self.tile_size,
                                           self.map_height * self.tile_size))

        h_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        v_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Bottom: Control panel (below canvas)
        control_frame = tk.LabelFrame(left_frame, text="Map Controls", padx=10, pady=5)
        control_frame.pack(fill=tk.X, pady=(5, 0))

        # Horizontal layout for controls - pack right side first to ensure visibility
        right_controls = tk.Frame(control_frame)
        right_controls.pack(side=tk.RIGHT, padx=5)

        tk.Button(right_controls, text="Import MAP_LINES[]",
                 command=self.import_map, bg='#2196F3', fg='white',
                 font=('Arial', 9, 'bold')).pack(side=tk.RIGHT, padx=2)

        tk.Button(right_controls, text="Export MAP_LINES[]",
                 command=self.export_map, bg='#4CAF50', fg='white',
                 font=('Arial', 9, 'bold')).pack(side=tk.RIGHT, padx=2)

        left_controls = tk.Frame(control_frame)
        left_controls.pack(side=tk.LEFT, padx=5)

        tk.Label(left_controls, text="Size:").pack(side=tk.LEFT, padx=(0, 5))
        tk.Label(left_controls, text="W:").pack(side=tk.LEFT)
        self.width_var = tk.StringVar(value=str(self.map_width))
        tk.Entry(left_controls, textvariable=self.width_var, width=5).pack(side=tk.LEFT, padx=2)

        tk.Label(left_controls, text="H:").pack(side=tk.LEFT, padx=(5, 0))
        self.height_var = tk.StringVar(value=str(self.map_height))
        tk.Entry(left_controls, textvariable=self.height_var, width=5).pack(side=tk.LEFT, padx=2)

        tk.Button(left_controls, text="Resize", command=self.resize_map).pack(side=tk.LEFT, padx=5)
        tk.Button(left_controls, text="Clear", command=self.clear_map).pack(side=tk.LEFT, padx=2)
        tk.Button(left_controls, text="Fill Floor", command=self.fill_floor).pack(side=tk.LEFT, padx=2)

        # Right side: Tile Palette (1/3 width)
        right_frame = tk.Frame(content_frame, width=400)
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, padx=(5, 0))
        right_frame.pack_propagate(False)

        # Scrollable palette
        palette_canvas = tk.Canvas(right_frame, bg='white')
        palette_scroll = tk.Scrollbar(right_frame, orient=tk.VERTICAL, command=palette_canvas.yview)
        palette_canvas.configure(yscrollcommand=palette_scroll.set)

        palette_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        palette_canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Frame inside canvas for palette content
        self.palette_frame = tk.Frame(palette_canvas, bg='white')
        palette_canvas.create_window((0, 0), window=self.palette_frame, anchor='nw')

        def on_palette_configure(event):
            palette_canvas.configure(scrollregion=palette_canvas.bbox('all'))

        self.palette_frame.bind('<Configure>', on_palette_configure)

        self.load_stage_data(self.current_stage)
        self.setup_palette(self.palette_frame)

        # Canvas bindings
        self.canvas.bind('<Button-1>', self.on_canvas_click)
        self.canvas.bind('<B1-Motion>', self.on_canvas_drag)
        self.canvas.bind('<ButtonRelease-1>', self.on_canvas_release)

        # Right-click to erase (fill with space)
        self.canvas.bind('<Button-3>', self.on_canvas_right_click)
        self.canvas.bind('<B3-Motion>', self.on_canvas_right_drag)
        self.canvas.bind('<ButtonRelease-3>', self.on_canvas_release)

        # Draw initial grid
        self.draw_map()

    def load_stage_data(self, stage_name):
        """Load enemy and NPC data for a specific stage"""
        print(f"\nLoading stage: {stage_name}")
        self.stage_enemies, self.stage_npcs = self.parser.parse_specific_stage(stage_name)

        # Reload images for this stage
        self.preload_images()


    def on_stage_change(self, event):
        """Handle stage selection change"""
        new_stage = self.stage_var.get()
        if new_stage != self.current_stage:
            self.current_stage = new_stage
            self.load_stage_data(new_stage)

            # Clear and rebuild palette
            for widget in self.palette_frame.winfo_children():
                widget.destroy()
            self.setup_palette(self.palette_frame)

    def setup_palette(self, parent):
        """Setup tile palette with radio buttons and image previews"""
        parent.config(bg='white')

        # Common tiles
        common_frame = tk.LabelFrame(parent, text="Common Tiles", bg='white')
        common_frame.pack(fill=tk.X, padx=5, pady=5)

        self.tile_var = tk.StringVar(value='#')

        # Add space tile manually first
        space_tile = self.parser.get_tile(' ')
        if space_tile:
            frame = tk.Frame(common_frame, bg='white')
            frame.pack(anchor=tk.W, pady=1)
            rb = tk.Radiobutton(frame, text=f"[SPACE] - {space_tile.desc}",
                               variable=self.tile_var, value=' ',
                               command=self.on_tile_select, bg='white')
            rb.pack(side=tk.LEFT)

        common_tiles = self.parser.get_all_common_tiles()
        for tile in common_tiles:
            frame = tk.Frame(common_frame, bg='white')
            frame.pack(anchor=tk.W, pady=1)

            rb = tk.Radiobutton(frame, text=f"{tile.symbol} - {tile.desc}",
                               variable=self.tile_var, value=tile.symbol,
                               command=self.on_tile_select, bg='white')
            rb.pack(side=tk.LEFT)

            # Show small preview if image exists
            if tile.image_path and tile.image_path in self.image_cache:
                preview_size = 16
                try:
                    full_path = os.path.join(self.project_root, tile.image_path)
                    img = Image.open(full_path)
                    img = img.resize((preview_size, preview_size), Image.Resampling.LANCZOS)
                    photo = ImageTk.PhotoImage(img)
                    # Store reference to prevent garbage collection
                    label = tk.Label(frame, image=photo, width=preview_size, height=preview_size)
                    label.image = photo
                    label.pack(side=tk.LEFT, padx=5)
                except:
                    pass

        # Enemy tiles (from current stage)
        enemy_frame = tk.LabelFrame(parent, text=f"Enemies ({self.current_stage})", bg='white')
        enemy_frame.pack(fill=tk.X, padx=5, pady=5)

        for tile_char, enemy_data in sorted(self.stage_enemies.items()):
            frame = tk.Frame(enemy_frame, bg='white')
            frame.pack(anchor=tk.W, pady=1)

            rb = tk.Radiobutton(frame, text=f"{tile_char} - {enemy_data['name']}",
                               variable=self.tile_var, value=tile_char,
                               command=self.on_tile_select, bg='white')
            rb.pack(side=tk.LEFT)

            image_path = enemy_data['imagePath']
            if image_path in self.image_cache:
                preview_size = 16
                try:
                    full_path = os.path.join(self.project_root, image_path)
                    img = Image.open(full_path)

                    # Handle sprite sheets
                    if 'texture_sprite' in image_path or 'sprite' in image_path.lower():
                        sprite_w = img.width // 2
                        sprite_h = img.height // 2
                        img = img.crop((0, 0, sprite_w, sprite_h))

                    img = img.resize((preview_size, preview_size), Image.Resampling.LANCZOS)
                    photo = ImageTk.PhotoImage(img)
                    label = tk.Label(frame, image=photo, width=preview_size, height=preview_size)
                    label.image = photo
                    label.pack(side=tk.LEFT, padx=5)
                except:
                    pass

        # NPC tiles (from current stage)
        npc_frame = tk.LabelFrame(parent, text=f"NPCs ({self.current_stage})", bg='white')
        npc_frame.pack(fill=tk.X, padx=5, pady=5)

        for tile_char, npc_data in sorted(self.stage_npcs.items()):
            frame = tk.Frame(npc_frame, bg='white')
            frame.pack(anchor=tk.W, pady=1)

            rb = tk.Radiobutton(frame, text=f"{tile_char} - {npc_data['name']}",
                               variable=self.tile_var, value=tile_char,
                               command=self.on_tile_select, bg='white')
            rb.pack(side=tk.LEFT)

            image_path = npc_data['imagePath']
            if image_path in self.image_cache:
                preview_size = 16
                try:
                    full_path = os.path.join(self.project_root, image_path)
                    img = Image.open(full_path)
                    img = img.resize((preview_size, preview_size), Image.Resampling.LANCZOS)
                    photo = ImageTk.PhotoImage(img)
                    label = tk.Label(frame, image=photo, width=preview_size, height=preview_size)
                    label.image = photo
                    label.pack(side=tk.LEFT, padx=5)
                except:
                    pass

        # Chest tiles
        chest_frame = tk.LabelFrame(parent, text="Chests (0-9)", bg='white')
        chest_frame.pack(fill=tk.X, padx=5, pady=5)

        chest_tiles = self.parser.get_chest_tiles()
        for tile in chest_tiles:
            frame = tk.Frame(chest_frame, bg='white')
            frame.pack(anchor=tk.W, pady=1)

            rb = tk.Radiobutton(frame, text=f"{tile.symbol} - {tile.desc}",
                               variable=self.tile_var, value=tile.symbol,
                               command=self.on_tile_select, bg='white')
            rb.pack(side=tk.LEFT)

            if tile.image_path and tile.image_path in self.image_cache:
                preview_size = 16
                try:
                    full_path = os.path.join(self.project_root, tile.image_path)
                    img = Image.open(full_path)
                    img = img.resize((preview_size, preview_size), Image.Resampling.LANCZOS)
                    photo = ImageTk.PhotoImage(img)
                    label = tk.Label(frame, image=photo, width=preview_size, height=preview_size)
                    label.image = photo
                    label.pack(side=tk.LEFT, padx=5)
                except:
                    pass

    def on_tile_select(self):
        """Handle tile selection"""
        self.selected_tile = self.tile_var.get()

    def get_tile_color(self, symbol):
        """Get color for a tile symbol"""
        colors = {
            ' ': '#000000',  # Black (Empty)
            '#': '#808080',  # Gray (Wall)
            '.': '#2C2C2C',  # Dark gray (Floor)
            '=': '#FFD700',  # Gold (Rail)
            '+': '#8B4513',  # Brown (Door)
            '$': '#4B0082',  # Indigo (Event Door)
            '@': '#00FF00',  # Green (Spawn)
        }

        # Enemy (lowercase) - Red
        if symbol.islower():
            return '#FF4444'

        # NPC (uppercase) - Blue
        if symbol.isupper():
            return '#4444FF'

        # Chest (number) - Yellow
        if symbol.isdigit():
            return '#FFFF44'

        return colors.get(symbol, '#FFFFFF')

    def draw_map(self):
        """Draw the entire map on canvas"""
        self.canvas.delete('all')

        for y in range(len(self.map_data)):
            for x in range(len(self.map_data[y])):
                symbol = self.map_data[y][x]
                x1 = x * self.tile_size
                y1 = y * self.tile_size
                x2 = x1 + self.tile_size
                y2 = y1 + self.tile_size

                # Get tile definition or stage-specific data
                tile_def = None
                image_path = None

                # Check if it's a stage-specific enemy or NPC
                if symbol in self.stage_enemies:
                    image_path = self.stage_enemies[symbol]['imagePath']
                elif symbol in self.stage_npcs:
                    image_path = self.stage_npcs[symbol]['imagePath']
                else:
                    tile_def = self.parser.get_tile(symbol)
                    if tile_def:
                        image_path = tile_def.image_path

                # Try to draw image first
                drawn_image = False
                if image_path and image_path in self.image_cache:
                    try:
                        photo = self.image_cache[image_path]
                        self.canvas.create_image(x1 + self.tile_size // 2,
                                                y1 + self.tile_size // 2,
                                                image=photo)
                        drawn_image = True

                        # Draw border
                        self.canvas.create_rectangle(x1, y1, x2, y2,
                                                    fill='', outline='#333333', width=1)
                    except Exception as e:
                        print(f"Error drawing image for {symbol}: {e}")

                # Fallback: draw colored rectangle with symbol
                if not drawn_image:
                    color = self.get_tile_color(symbol)
                    self.canvas.create_rectangle(x1, y1, x2, y2,
                                                fill=color, outline='#444444')
                    self.canvas.create_text(x1 + self.tile_size // 2,
                                           y1 + self.tile_size // 2,
                                           text=symbol, fill='white',
                                           font=('Courier', 10, 'bold'))

    def canvas_to_grid(self, canvas_x, canvas_y):
        """Convert canvas coordinates to grid coordinates"""
        grid_x = int(canvas_x // self.tile_size)
        grid_y = int(canvas_y // self.tile_size)

        if 0 <= grid_x < len(self.map_data[0]) and 0 <= grid_y < len(self.map_data):
            return grid_x, grid_y
        return None, None

    def on_canvas_click(self, event):
        """Handle canvas click"""
        canvas_x = self.canvas.canvasx(event.x)
        canvas_y = self.canvas.canvasy(event.y)

        grid_x, grid_y = self.canvas_to_grid(canvas_x, canvas_y)
        if grid_x is not None:
            self.map_data[grid_y][grid_x] = self.selected_tile
            self.is_drawing = True
            self.last_drawn = (grid_x, grid_y)
            self.draw_map()

    def on_canvas_drag(self, event):
        """Handle canvas drag"""
        if not self.is_drawing:
            return

        canvas_x = self.canvas.canvasx(event.x)
        canvas_y = self.canvas.canvasy(event.y)

        grid_x, grid_y = self.canvas_to_grid(canvas_x, canvas_y)
        if grid_x is not None and (grid_x, grid_y) != self.last_drawn:
            self.map_data[grid_y][grid_x] = self.selected_tile
            self.last_drawn = (grid_x, grid_y)
            self.draw_map()

    def on_canvas_release(self, event):
        """Handle mouse release"""
        self.is_drawing = False
        self.last_drawn = None

    def on_canvas_right_click(self, event):
        """Handle right-click to erase (fill with space)"""
        canvas_x = self.canvas.canvasx(event.x)
        canvas_y = self.canvas.canvasy(event.y)

        grid_x, grid_y = self.canvas_to_grid(canvas_x, canvas_y)
        if grid_x is not None:
            self.map_data[grid_y][grid_x] = ' '
            self.is_drawing = True
            self.last_drawn = (grid_x, grid_y)
            self.draw_map()

    def on_canvas_right_drag(self, event):
        """Handle right-click drag to erase"""
        if not self.is_drawing:
            return

        canvas_x = self.canvas.canvasx(event.x)
        canvas_y = self.canvas.canvasy(event.y)

        grid_x, grid_y = self.canvas_to_grid(canvas_x, canvas_y)
        if grid_x is not None and (grid_x, grid_y) != self.last_drawn:
            self.map_data[grid_y][grid_x] = ' '
            self.last_drawn = (grid_x, grid_y)
            self.draw_map()

    def resize_map(self):
        """Resize the map"""
        try:
            new_width = int(self.width_var.get())
            new_height = int(self.height_var.get())

            if new_width < 1 or new_height < 1 or new_width > 100 or new_height > 100:
                messagebox.showerror("Error", "Map size must be between 1 and 100")
                return

            # Create new map data filled with spaces (not floor)
            new_data = [[' ' for _ in range(new_width)] for _ in range(new_height)]

            # Copy old data
            for y in range(min(len(self.map_data), new_height)):
                for x in range(min(len(self.map_data[0]), new_width)):
                    new_data[y][x] = self.map_data[y][x]

            self.map_data = new_data
            self.map_width = new_width
            self.map_height = new_height

            # Update scrollregion for new size
            self.canvas.configure(scrollregion=(0, 0,
                                               self.map_width * self.tile_size,
                                               self.map_height * self.tile_size))

            self.draw_map()

        except ValueError:
            messagebox.showerror("Error", "Invalid map size")

    def clear_map(self):
        """Clear the map (fill with floor)"""
        if messagebox.askyesno("Clear Map", "Clear the entire map?"):
            self.map_data = [['.' for _ in range(self.map_width)] for _ in range(self.map_height)]
            self.draw_map()

    def fill_floor(self):
        """Fill entire map with floor tiles"""
        self.map_data = [['.' for _ in range(self.map_width)] for _ in range(self.map_height)]
        self.draw_map()

    def export_map(self):
        """Export map as MAP_LINES[] C code"""
        # Create export window
        export_window = tk.Toplevel(self.root)
        export_window.title("Export MAP_LINES[]")
        export_window.geometry("800x600")

        # Generate C code
        lines = []
        lines.append("static const char* MAP_LINES[] = {")

        for y, row in enumerate(self.map_data):
            row_str = ''.join(row)
            lines.append(f'    "{row_str}",')

        lines.append("};")

        code = '\n'.join(lines)

        # Text widget
        text_frame = tk.Frame(export_window)
        text_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        text_widget = scrolledtext.ScrolledText(text_frame, font=('Courier', 10))
        text_widget.pack(fill=tk.BOTH, expand=True)
        text_widget.insert('1.0', code)

        # Copy button
        def copy_to_clipboard():
            self.root.clipboard_clear()
            self.root.clipboard_append(code)
            messagebox.showinfo("Copied", "MAP_LINES[] copied to clipboard!")

        tk.Button(export_window, text="Copy to Clipboard",
                 command=copy_to_clipboard, bg='#4CAF50', fg='white').pack(pady=10)

    def import_map(self):
        """Import map from MAP_LINES[] C code"""
        import_window = tk.Toplevel(self.root)
        import_window.title("Import MAP_LINES[]")
        import_window.geometry("800x400")

        tk.Label(import_window, text="Paste MAP_LINES[] code below:").pack(pady=5)

        text_widget = scrolledtext.ScrolledText(import_window, font=('Courier', 10))
        text_widget.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        def do_import():
            code = text_widget.get('1.0', tk.END)

            # Parse lines
            import re
            pattern = r'"([^"]+)"'
            matches = re.findall(pattern, code)

            if not matches:
                messagebox.showerror("Error", "No map lines found")
                return

            # Create new map data
            new_data = []
            max_width = 0

            for line in matches:
                new_data.append(list(line))
                max_width = max(max_width, len(line))

            # Pad shorter lines with spaces (not floor)
            for row in new_data:
                while len(row) < max_width:
                    row.append(' ')

            self.map_data = new_data
            self.map_height = len(new_data)
            self.map_width = max_width

            self.height_var.set(str(self.map_height))
            self.width_var.set(str(self.map_width))

            # Update scrollregion for new size
            self.canvas.configure(scrollregion=(0, 0,
                                               self.map_width * self.tile_size,
                                               self.map_height * self.tile_size))

            self.draw_map()
            import_window.destroy()
            messagebox.showinfo("Success", f"Map imported: {self.map_width}x{self.map_height}")

        tk.Button(import_window, text="Import", command=do_import,
                 bg='#2196F3', fg='white').pack(pady=10)

def main():
    root = tk.Tk()
    app = MapEditor(root)
    root.mainloop()

if __name__ == '__main__':
    main()
