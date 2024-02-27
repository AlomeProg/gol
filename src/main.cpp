#include <raylib.h>
#include <array>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>

const unsigned int size_map = 128;
const unsigned int size_cell = 8;

using Map = std::array<std::array<bool, size_map>, size_map>;

Map mapA;  
Map mapB;
Map *current_map;
Map *next_map;

std::mutex mutex_map;

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

      bool is_alive = current_map->at(y).at(x);
      bool will_live = is_alive ? (neighbors == 2 || neighbors == 3) : (neighbors == 3);

      next_map->at(y).at(x) = will_live;
    }
  }
  std::swap(current_map, next_map);
}

void TickOneThread(std::size_t start_y, std::size_t end_y) {
  for (std::size_t y = start_y; y < end_y; ++y) {
    for (std::size_t x = 0; x < size_map; ++x) {
      int neighbors = CheckNeighbors(x, y);

      bool is_alive = current_map->at(y).at(x);
      bool will_live = is_alive ? (neighbors == 2 || neighbors == 3) : (neighbors == 3);

      // Блокируем мьютекс перед изменением следующей карты
      std::lock_guard<std::mutex> lock(mutex_map);
      next_map->at(y).at(x) = will_live;
    }
  }
}

void MultiTickMap(std::vector<std::thread>& threads)
{
  const std::size_t num_threads = threads.size();
  const std::size_t rows_per_thread = size_map / num_threads;
  
  // std::vector<std::thread> threads; 
  
  for(size_t i = 0; i < num_threads; i++)
  {
    std::size_t start_y = i * rows_per_thread;
    std::size_t end_y   = (i == num_threads - 1) ? size_map : (start_y + rows_per_thread);
    threads[i] = std::thread(TickOneThread, start_y, end_y);
  }

  for(auto& thread : threads) thread.join();
  std::swap(current_map, next_map);
}

void UpdateCamera(Camera2D& camera)
{
  static float delta, speed;
  delta = GetFrameTime();
  speed = 256;

  if (IsKeyDown(KEY_LEFT_SHIFT)) speed  *= 2;
  if (IsKeyDown(KEY_W)) camera.target.y -= speed * delta;
  if (IsKeyDown(KEY_A)) camera.target.x -= speed * delta;
  if (IsKeyDown(KEY_S)) camera.target.y += speed * delta;
  if (IsKeyDown(KEY_D)) camera.target.x += speed * delta;
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
    .offset   = {400.0f, 300.0f},
    .target   = {0.0f, 0.0f},
    .rotation = 0.0f,
    .zoom     = 1.0f,
  };

  const std::size_t num_threads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads(num_threads);

  while (!WindowShouldClose())
  {
    auto cur_time = std::chrono::steady_clock::now();
    auto ela_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - last_time).count();
    if (ela_time >= 100)
    {
      // TickMap();
      MultiTickMap(threads);
      last_time = cur_time;
      BeginTextureMode(texture);
      DrawMap();
      EndTextureMode();
    }
    
    UpdateCamera(camera);

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