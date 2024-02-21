#include <ecs_manager.hpp>
#include <raylib.h>

auto
main() -> int
{
  InitWindow(640, 480, "Window");
  SetTargetFPS(60);
  while (not WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawFPS(10, 10);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
