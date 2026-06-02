/*
 * Snake game with vim keybindings
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "snake.h"

#define MAX_SNAKE_LEN 255

static bool pauseGame = false;
static bool exitGame  = false;
static int stride = 2;
static Directions snakeDir = UP;

Snake snakePixels[MAX_SNAKE_LEN];
Vector2 snakePixelsPos[MAX_SNAKE_LEN];

static int currSnakeLen = 1;

Food food;

static int screenWidth = 450;
static int screenHeight = 450;

char warningMsg[20];

int main(void)
{
  snakePixels[0] = initSnake();
  snakePixels[0].color = GRAY;

  food = initFood();
  setFoodPosition();

  SetTraceLogLevel(3);

  snakePixels[0].position.x = screenWidth/2;
  snakePixels[0].position.y = screenHeight/2;

  InitWindow(screenWidth, screenHeight, "Cake Vim");

  SetTargetFPS(60);

  while(!WindowShouldClose() && !exitGame)
  {
    DrawGame();
  }

  CloseWindow();
  return 0;
}

// update elemens of game like: snakePixels cell positions, food position
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

#if 0
  if(IsKeyPressed(KEY_EQUAL) || IsKeyPressedRepeat(KEY_EQUAL))
  {
    // if(stride < INT_MAX) stride++;
    if(snakePixelsPos.width < INT_MAX) snakePixelsPos.width++;
    if(snakePixelsPos.height< INT_MAX) snakePixelsPos.height++;
  }
  if(IsKeyPressed(KEY_MINUS) || IsKeyPressedRepeat(KEY_MINUS))
  {
    if(snakePixelsPos.width > 0) snakePixelsPos.width--;
    if(snakePixelsPos.height> 0) snakePixelsPos.height--;
  }
#endif

  if(!pauseGame)
  {
    // FIXME: hande if distance between screen edge and snakePixels rectangle is less than stride
    if(IsKeyPressed(KEY_H) || IsKeyPressedRepeat(KEY_H)) // towards left wall
    {
      snakeDir = LEFT;
    }
    else if(IsKeyPressed(KEY_L) || IsKeyPressedRepeat(KEY_L)) // towards right wall
    {
      snakeDir = RIGHT;
    }
    else if(IsKeyPressed(KEY_J) || IsKeyPressedRepeat(KEY_J)) // towards bottom wall
    {
      snakeDir = DOWN;
    }
    else if(IsKeyPressed(KEY_K) || IsKeyPressedRepeat(KEY_K)) // towards Top wall
    {
      snakeDir = UP;
    }

    incrementPixPosition(0, snakeDir);

    // FIXME: add snakePixels killed state
    for(int i = 0; i < currSnakeLen; i++)
    {
      // FIXME: reset in both cases
      //1. if snakePixels hits a wall
      if(snakePixels[i].position.x <= 0 || snakePixels[i].position.x >= screenWidth &&
          snakePixels[i].position.y <= 0 || snakePixels[i].position.y >= screenHeight) 
      {
        snakePixels[0].position.x = screenWidth/2;
        snakePixels[0].position.y = screenHeight/2;
        currSnakeLen = 1;
        printf("Snake has hit a wall\n");
      }

      //2. if snakePixels bites itself
      if(0 && i != 0 && (abs(snakePixels[0].position.x - snakePixels[i].position.x) <= DEFAULT_SIZE &&
          abs(snakePixels[0].position.y - snakePixels[i].position.y) <= DEFAULT_SIZE))
      {
        snakePixels[0].position.x = screenWidth/2;
        snakePixels[0].position.y = screenHeight/2;
        currSnakeLen = 1;
        printf("Snake has collided with itself\n");
      }
    }

    //if snakePixels eats the food
    if(abs(snakePixels[0].position.x - food.position.x) <= DEFAULT_SIZE &&
        abs(snakePixels[0].position.y - food.position.y) <= DEFAULT_SIZE)
    {
      snakePixels[currSnakeLen] = initSnake();
      snakePixels[currSnakeLen].position = snakePixels[currSnakeLen-1].position;
      incrementPixPosition(currSnakeLen, snakeDir);
      printf("Snake has eaten food at: [%d,%d] \n", food.position.x, food.position.y);
      currSnakeLen++;
      setFoodPosition();
      // update food position such that it lies outside the body of snakePixels
    }

    for(int i = 1; i < currSnakeLen; i++)
    {
      snakePixels[i].position = snakePixelsPos[i-1];
    }

    // update snakePixels position
    for(int i = 0; i < currSnakeLen; i++)
      snakePixelsPos[i] = snakePixels[i].position;
  }
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
                              
  // Draw Snake
  for(int i = 0; i < currSnakeLen; i++)
  {
    DrawRectangle(snakePixels[i].position.x,
                  snakePixels[i].position.y, 
                  snakePixels[i].size, 
                  snakePixels[i].size, 
                  snakePixels[i].color);
  }
  
  // Draw Food
  DrawRectangle(food.position.x,
          food.position.y, 
          food.size, 
          food.size, 
          food.color);

  if(strlen(warningMsg) > 0)
    DrawText(warningMsg, screenWidth/2 , 0, 30, BLACK);

  EndDrawing();
}

struct Snake initSnake()
{
  struct Snake snakePixels = {
    .position = {0, 0},
    .color    = RED,
    .size     = DEFAULT_SIZE,
    .speed    = 1,
    .active   = true,
  };
  return snakePixels;
}

struct Food initFood()
{
  struct Food food = {
    .position = {0, 0},
    .size  = DEFAULT_SIZE,
    .color = BLUE,
  };
  return food;
}

// FIXME: check if food position collids with any snake pixel
void setFoodPosition()
{
  food.position.x = GetRandomValue(5, screenWidth);
  food.position.y = GetRandomValue(5, screenHeight);
  char s[128];
  sprintf(s, "Food spawned at: [%d, %d]\n", food.position.x, food.position.y);
  TraceLog(LOG_INFO, s);
}

void incrementPixPosition(int pixIdx, Directions snakeDir)
{
  switch (snakeDir)
  {
    case UP:
      snakePixels[pixIdx].position.y -= stride;
      break;
    case DOWN:
      snakePixels[pixIdx].position.y += stride;
      break;
    case LEFT:
      snakePixels[pixIdx].position.x -= stride;
      break;
    case RIGHT:
      snakePixels[pixIdx].position.x += stride;
      break;
  }
}
