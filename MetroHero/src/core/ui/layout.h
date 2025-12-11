#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

// Screen Dimensions (Expanded & Centered)
// Screen Dimensions (Expanded & Centered)
#define SCREEN_W 210
#define SCREEN_H 55

#define SCREEN_PADDING_LEFT 20
#define SCREEN_PADDING_TOP 6

// Map Viewport
#define VIEWPORT_X (SCREEN_PADDING_LEFT)
#define VIEWPORT_Y (SCREEN_PADDING_TOP)
#define VIEWPORT_W 120 
#define VIEWPORT_H 30

// Right Side Panels (Status / Equipment / Dialogue)
#define PANEL_X (VIEWPORT_X + VIEWPORT_W)
#define PANEL_WIDTH 52

// Status Panel
#define STATUS_X PANEL_X
#define STATUS_Y (SCREEN_PADDING_TOP)
#define STATUS_W PANEL_WIDTH
#define STATUS_H 14

// Equipment Panel
#define EQUIP_X PANEL_X
#define EQUIP_Y (STATUS_Y + STATUS_H)
#define EQUIP_W PANEL_WIDTH
#define EQUIP_H 16

// Dialogue Panel (Overlays Status + Equip)
#define DIALOGUE_X PANEL_X
#define DIALOGUE_Y (SCREEN_PADDING_TOP)
#define DIALOGUE_W PANEL_WIDTH
#define DIALOGUE_H (STATUS_H + EQUIP_H)

// Log Panel
#define LOG_X (SCREEN_PADDING_LEFT)
#define LOG_Y (VIEWPORT_Y + VIEWPORT_H) // +2 padding removed
#define LOG_W (VIEWPORT_W + PANEL_WIDTH)
#define LOG_H 14
#define LOG_LINES 200

#endif
