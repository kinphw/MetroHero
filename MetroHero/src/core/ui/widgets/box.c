#include "box.h"
#include "../text/render.h"

#include "raylib.h" // Explicitly include for DrawRectangle functions
#include "../text/glyph.h" // For display_width

void ui_draw_box(int x, int y, int w, int h, const char* title) {
    // 1. Convert Grid to Pixels
    int px = x * 8;
    int py = y * 16;
    int pw = w * 8;
    int ph = h * 16;
    
    // 2. Determine Color
    // Use a default border color (usually Gray/White for MetroHero retro style)
    // If we want to support colored borders via argument, we'd need to change signature.
    // For now, match the previous code: "\033[0m" -> White/Reset.
    Color borderColor = WHITE; 
    
    // 3. Draw Box (Raylib)
    // Use texture mode if this is called within a frame batch, but simple shapes usually work fine.
    // However, since we are mixing with text which uses `ui_begin_texture_mode` (implicitly or explicitly),
    // it is safer to be consistent. But ui_draw_box is usually high level.
    // Let's assume we can draw directly.
    
    // Draw thick border (2px) to match the bold look
    DrawRectangleLines(px, py, pw, ph, borderColor);
    DrawRectangleLines(px + 1, py + 1, pw - 2, ph - 2, borderColor);
    
    // 4. Draw Title with Masking
    if (title) {
        // Calculate title width in grid units
        int titleGridW = display_width(title);
        
        // Calculate title position (starts at x + 2 grid units usually)
        int titleGridX = x + 2;
        int titlePxX = titleGridX * 8;
        
        // Masking Rectangle (Black box to hide the top border line behind title)
        // Add some padding (1 char before, 1 char after)
        // Previous code had "─ " before and " ─" after.
        // Let's just mask the area for the text + padding.
        
        int maskPxW = (titleGridW + 2) * 8; // +2 for padding spaces
        
        // Draw Black Rectangle to mask top border
        DrawRectangle(titlePxX - 8, py, maskPxW, 4, BLACK); // Height 4 is enough to cover the top line (which is at py)
        // Wait, the text is drawn at py. The border is at py.
        // If we draw text at `y`, it occupies the row.
        // Mask the full height of the top row? No, just the border line.
        // DrawRectangle(titlePxX - 8, py, maskPxW, 16, BLACK);
        DrawRectangle(titlePxX - 8, py, maskPxW, 16, BLACK);

        // Draw Title Text
        // Use BRIGHT_WHITE for title as per previous code
        ui_draw_text_clipped(titleGridX, y, w - 4, title, "\033[97m");
    }
}
