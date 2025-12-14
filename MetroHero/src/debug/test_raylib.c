/*******************************************************************************************
*
*   MetroHero - Raylib Test Code
*
*   This is a standalone test to verify Raylib integration.
*   Compile this file linking with raylib.lib to test.
*
********************************************************************************************/

#include "raylib.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    // 1. 윈도우 생성
    InitWindow(screenWidth, screenHeight, "MetroHero - Raylib Test Mode");

    SetTargetFPS(60);               // 60프레임 고정
    //--------------------------------------------------------------------------------------

    // 이미지 로드 테스트 (파일이 있어야 함, 없으면 사각형 대체)
    // Texture2D testTexture = LoadTexture("assets/test.png");

    // Main game loop
    while (!WindowShouldClose())    // ESC키나 닫기 버튼 누를 때까지 반복
    {
        // Update
        //----------------------------------------------------------------------------------
        // 키 입력 처리 등
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);  // 배경 지우기 (흰색)

            DrawText("Raylib is working!", 190, 200, 40, LIGHTGRAY);
            DrawText("Press ESC to close.", 190, 250, 20, GRAY);

            // 간단한 도형 그리기 (이미지 대신)
            DrawRectangle(50, 50, 100, 100, RED);
            DrawText("Red Box", 50, 160, 20, RED);

            // 마우스 위치 표시
            DrawText(TextFormat("Mouse: %d, %d", GetMouseX(), GetMouseY()), 10, 10, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // UnloadTexture(testTexture);
    CloseWindow();        // 윈도우 닫기
    //--------------------------------------------------------------------------------------

    return 0;
}
