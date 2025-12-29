#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

// Screen Dimensions (Expanded & Centered)
// Text Grid Unit: 8x16 pixels
#define GRID_W 8
#define GRID_H 16

// Screen Dimensions (Expanded & Centered)
#define SCREEN_W 210
#define SCREEN_H 55

#define SCREEN_PADDING_LEFT 20
#define SCREEN_PADDING_TOP 6

// Map Viewport (Layout in Grid Units)
#define VIEWPORT_X (SCREEN_PADDING_LEFT)
#define VIEWPORT_Y (SCREEN_PADDING_TOP)
#define VIEWPORT_W 120 
#define VIEWPORT_H 30

// ★ Map Tile Rendering Constants (32x32 Mixed Resolution)
#define MAP_TILE_SIZE 32
// (VIEWPORT_W * GRID_W) / MAP_TILE_SIZE = (120*8)/32 = 960/32 = 30
#define VIEWPORT_MAP_W 30
// ((VIEWPORT_H * GRID_H) / MAP_TILE_SIZE) * GRID_H? No, Total Pixel Height / 32
// Total Pixel Height = 30 * 16 = 480. 480 / 32 = 15.
#define VIEWPORT_MAP_H 15

// Right Side Panels (Status / Equipment / Dialogue)
#define PANEL_X (VIEWPORT_X + VIEWPORT_W)
#define PANEL_WIDTH 52

// Status Panel
#define STATUS_X PANEL_X
#define STATUS_Y (SCREEN_PADDING_TOP)
#define STATUS_W PANEL_WIDTH
#define STATUS_H 14

// Equipment Panel (Reduced Height)
#define EQUIP_X PANEL_X
#define EQUIP_Y (STATUS_Y + STATUS_H)
#define EQUIP_W PANEL_WIDTH
#define EQUIP_H 8

// ★ Quest Tracker Panel (New - Below Equipment)
#define QUEST_X PANEL_X
#define QUEST_Y (EQUIP_Y + EQUIP_H)
#define QUEST_W PANEL_WIDTH
#define QUEST_H 8

// Dialogue Panel (Overlays Status + Equip + Quest)
#define DIALOGUE_X PANEL_X
#define DIALOGUE_Y (SCREEN_PADDING_TOP)
#define DIALOGUE_W PANEL_WIDTH
#define DIALOGUE_H (STATUS_H + EQUIP_H + QUEST_H)

// Log Panel (Left - General)
#define LOG_X (SCREEN_PADDING_LEFT)
#define LOG_Y (VIEWPORT_Y + VIEWPORT_H) 
#define LOG_W ((VIEWPORT_W + PANEL_WIDTH) / 2)
#define LOG_H 14
#define LOG_LINES 200

// Combat Log Panel (Right - Combat)
#define COMBAT_LOG_X (LOG_X + LOG_W)
#define COMBAT_LOG_Y LOG_Y
#define COMBAT_LOG_W ((VIEWPORT_W + PANEL_WIDTH) - LOG_W)
#define COMBAT_LOG_H LOG_H
#define COMBAT_LOG_LINES 200

#endif
