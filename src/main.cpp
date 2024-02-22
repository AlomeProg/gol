#include <raylib.h>

int main()
{
  InitWindow(800, 600, "gol");
  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(0, 0);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}