#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

// Screen Dimensions
#define SCREEN_W 120
#define SCREEN_H 30

// Status Panel
#define STATUS_X 120
#define STATUS_Y 0
#define STATUS_W 48
#define STATUS_H 14

// Equipment Panel
#define EQUIP_X 120
#define EQUIP_Y 14
#define EQUIP_W 48
#define EQUIP_H 16

// Map Viewport
#define VIEWPORT_X 0
#define VIEWPORT_Y 0
#define VIEWPORT_W 118 
#define VIEWPORT_H 30
// Note: Map render width is explicitly 60 double-width chars = 120 width visually? 
// Original viewport.c used 60 for map_draw_viewport call check if this aligns.

// Dialogue Panel
#define DIALOGUE_X 120
#define DIALOGUE_Y 0
#define DIALOGUE_W 48
#define DIALOGUE_H 30

// Log Panel
#define LOG_X 0
#define LOG_Y 30
#define LOG_W 168
#define LOG_H 12
#define LOG_LINES 200

#endif
