#include <raylib.h>
#include <array>
#include <iostream>
#include <random>
#include <chrono>

const unsigned int size_map = 128;
const unsigned int size_cell = 8;

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
  next_map      = &mapB;
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

      if (current_map->at(y).at(x) == false)
      {
        if (neighbors == 3)
          next_map->at(y).at(x) = true;
        else 
          next_map->at(y).at(x) = false;
      }
      else 
      {
        if (neighbors == 2 || neighbors == 3)
          next_map->at(y).at(x) = true;
        else
          next_map->at(y).at(x) = false;
      }
    }
  }
  std::swap(current_map, next_map);
}

int main()
{
  InitWindow(800, 600, "gol");
  InitMap();
  
  int size_texture = size_map * size_cell;
  for (int i = 0; i < size_map; i++) size_texture += i;
  RenderTexture2D texture = LoadRenderTexture(size_texture, size_texture);
  
  auto last_time = std::chrono::steady_clock::now();

  BeginTextureMode(texture);
  DrawMap();
  EndTextureMode();

  Camera2D camera = 
  {
    .offset = {400.0f, 300.0f},
    .target = {0.0f, 0.0f},
    .rotation = 0.0f,
    .zoom = 1.0f,
  };

  while (!WindowShouldClose())
  {
    auto cur_time = std::chrono::steady_clock::now();
    auto ela_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - last_time).count();
    if (ela_time >= 100)
    {
      TickMap();
      last_time = cur_time;
      BeginTextureMode(texture);
      DrawMap();
      EndTextureMode();
    }

    const Rectangle source_rec = 
    {
      .x = 0, .y = 0,
      .width = static_cast<float>(size_texture),
      .height = static_cast<float>(-size_texture),
    };

    BeginDrawing();
    {
      BeginMode2D(camera);
      {
        ClearBackground(BLACK);
        DrawTextureRec(texture.texture, source_rec, {0,0}, WHITE);
      }
      EndMode2D();
      DrawFPS(0, 0);
    }
    EndDrawing();
  }
  CloseWindow();
  return 0;
}