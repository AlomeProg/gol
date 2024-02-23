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
Map *next_map;

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
  next_map  = &mapB;
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

int CheckNeighbors(std::size_t x, std::size_t y)
{
  int neighbors = 0;

  for (int iy = -1; iy <= 1; iy++)
  {
    for (int ix = -1; ix <= 1; ix++)
    {
      if (iy == 0 && ix == 0) continue;
      int new_y = y + iy;
      int new_x = x + ix;
      
      if (new_y < 0 || new_y >= size_map) continue;
      if (new_x < 0 || new_x >= size_map) continue;

      if (current_map->at(new_y).at(new_x) == true) 
        neighbors++; 
    }
  }

  return neighbors;
}

void TickMap()
{
  for(std::size_t y = 0; y < size_map; y++)
  {
    for(std::size_t x = 0; x < size_map; x++)
    {
      int neighbors = CheckNeighbors(x, y);

      if (neighbors == 3 && current_map->at(y).at(x) == false)
        next_map->at(y).at(x) = true;

      if (neighbors == 2 || neighbors == 3) 
        next_map->at(y).at(x) = true;

      if (neighbors < 2 || neighbors > 3)
        next_map->at(y).at(x) = false;
    }
  }
  std::swap(current_map, next_map);
}

int main()
{
  InitWindow(800, 600, "gol");
  InitMap();
  while (!WindowShouldClose())
  {
    TickMap();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawMap();
    DrawFPS(0, 0);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}