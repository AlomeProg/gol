#include <raylib.h>
#include <array>
#include <iostream>
#include <random>

const unsigned int size_map = 16;
const unsigned int size_cell = 16;

using Map = std::array<std::array<bool, size_map>, size_map>;

Map mapA;  
Map mapB;
Map *current_map;
Map *previous_map;

void InitMap()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0,1);

  for(std::size_t y = 0; y < size_map; y++)
  {
    for(std::size_t x = 0; x < size_map; x++)
    {
      mapA.at(y).at(x) = (dist(gen) == 1) ? true : false;
      mapB.at(y).at(x) = false;
    }
  }

  current_map   = &mapA;
  previous_map  = &mapB;
}

void DrawMap()
{
  for(std::size_t y = 0; y < size_map; y++)
  {
    for(std::size_t x = 0; x < size_map; x++)
    {
      Color color = (current_map->at(y).at(x)) ? WHITE : BLACK;
      const int tmp_x = x * size_cell + x;
      const int tmp_y = y * size_cell + y;
      DrawRectangle(tmp_x, tmp_y, size_cell, size_cell, color);
    }
  }
}

int main()
{
  InitWindow(800, 600, "gol");
  InitMap();
  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawMap();
    DrawFPS(0, 0);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}