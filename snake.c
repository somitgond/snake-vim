/*
 * Snake game with vim keybindings
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "raylib.h"
#include "snake.h"

static bool pauseGame = false;
static bool exitGame  = false;
unsigned int stride = 1;
// Circle foodPos = {0, 0, 5, GREEN};
Rectangle snakePos = {0, 0, 5, 5};


static int screenWidth = 0;
static int screenHeight = 0;

char warningMsg[20];

int main(void)
{
  // snakePos.width = stride;
  // snakePos.height = stride;

  SetTraceLogLevel(3);

  screenWidth = 450;
  screenHeight = 450;

  snakePos.x = screenWidth/2;
  snakePos.y = screenHeight/2;

  InitWindow(screenWidth, screenHeight, "Cake Vim");

  SetTargetFPS(60);

  while(!WindowShouldClose() && !exitGame)
  {
    DrawGame();
  }

  CloseWindow();
  return 0;
}

void DrawGame()
{
  // Draw
  BeginDrawing();
  DrawFPS(0, 0);

  screenHeight = GetScreenHeight();
  screenWidth  = GetScreenWidth();

  ClearBackground(LIGHTGRAY); // set background color
                              
  UpdateGame();

  // Draw Rectangle
  DrawRectangle(snakePos.x, snakePos.y, snakePos.width, snakePos.height, RED);

  if(strlen(warningMsg) > 0)
    DrawText(warningMsg, screenWidth/2 , 0, 30, BLACK);

  EndDrawing();
}

// snake movement
void UpdateGame()
{
  if(IsKeyPressed(KEY_SPACE)) 
  {
    pauseGame = !pauseGame;
    if(pauseGame)
    {
      snprintf(warningMsg, sizeof(warningMsg), "Game Paused");
    }
    else 
    {
      snprintf(warningMsg, sizeof(warningMsg), "");
    }
  }
  if(IsKeyPressed(KEY_Q))
    exitGame = true;

  if(IsKeyPressed(KEY_EQUAL) || IsKeyPressedRepeat(KEY_EQUAL))
  {
    // if(stride < INT_MAX) stride++;
    if(snakePos.width < INT_MAX) snakePos.width++;
    if(snakePos.height< INT_MAX) snakePos.height++;
  }
  if(IsKeyPressed(KEY_MINUS) || IsKeyPressedRepeat(KEY_MINUS))
  {
    if(snakePos.width > 0) snakePos.width--;
    if(snakePos.height> 0) snakePos.height--;
  }

  if(!pauseGame)
  {
    // FIXME: hande if distance between screen edge and snake rectangle is less than stride
    if(IsKeyPressed(KEY_H) || IsKeyPressedRepeat(KEY_H)) // towards left wall
    {
      if(snakePos.x - stride <= snakePos.width)
        snakePos.x = screenWidth - snakePos.width;
      else
        snakePos.x-=stride;
    }
    else if(IsKeyPressed(KEY_J) || IsKeyPressedRepeat(KEY_J)) // towards bottom wall
    {
      if(snakePos.y + stride + snakePos.height >= screenHeight)
        snakePos.y = 0;
      else
        snakePos.y+=stride;
    }
    else if(IsKeyPressed(KEY_K) || IsKeyPressedRepeat(KEY_K)) // towards Top wall
    {
      if(snakePos.y - stride <= snakePos.height)
        snakePos.y = screenHeight - snakePos.height;
      else
        snakePos.y-=stride;

    }
    else if(IsKeyPressed(KEY_L) || IsKeyPressedRepeat(KEY_L)) // towards right wall
    {
      if(snakePos.x + snakePos.width + stride >= screenWidth)
        snakePos.x = 0;
      else
        snakePos.x+=stride;
    }
  }
}
