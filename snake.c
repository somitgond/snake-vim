/*
 * Snake game with vim keybindings
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#include "snake.h"

#define MAX_SNAKE_LEN 255

static bool pauseGame = false;
static bool exitGame  = false;
static Directions snakeDir = UP;
static uint64_t frameCounterMod = 15;
static int size = 15;

Snake snakePixels[MAX_SNAKE_LEN];
Vector2 snakePixelsPos[MAX_SNAKE_LEN];

static int currSnakeLen = 1;

static int currScore = 0;

uint64_t frameCounter = 0;

Food food;

static int screenWidth = 450;
static int screenHeight = 450;

char warningMsg[20];
char statusMsg[512];

int main(void)
{
  snakePixels[0] = initSnake();

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

  if(IsKeyPressed(KEY_EQUAL) || IsKeyPressedRepeat(KEY_EQUAL))
  {
    if(size < 25) size++;
  }
  if(IsKeyPressed(KEY_MINUS) || IsKeyPressedRepeat(KEY_MINUS))
  {
    if(size > 5) size--;
  }

  if(!pauseGame)
  {
    // FIXME: hande if distance between screen edge and snakePixels rectangle is less than stride
    if(IsKeyPressed(KEY_H) || IsKeyPressedRepeat(KEY_H)) // towards left wall
    {
      if(snakeDir != RIGHT) snakeDir = LEFT;
    }
    else if(IsKeyPressed(KEY_L) || IsKeyPressedRepeat(KEY_L)) // towards right wall
    {
      if(snakeDir != LEFT) snakeDir = RIGHT;
    }
    else if(IsKeyPressed(KEY_J) || IsKeyPressedRepeat(KEY_J)) // towards bottom wall
    {
     if(snakeDir != UP)  snakeDir = DOWN;
    }
    else if(IsKeyPressed(KEY_K) || IsKeyPressedRepeat(KEY_K)) // towards Top wall
    {
      if(snakeDir != DOWN)  snakeDir = UP;
    }

    if(frameCounter%frameCounterMod == 0)
    {

      incrementPixPosition(0, snakeDir);

      // FIXME: add snakePixels killed state
      for(int i = 0; i < currSnakeLen; i++)
      {
        // FIXME: reset in both cases
        //1. if snakePixels hits a wall
        if(snakePixels[i].position.x < 0 || snakePixels[i].position.x >= screenWidth ||
            snakePixels[i].position.y < 0 || snakePixels[i].position.y >= screenHeight) 
        {
          snakePixels[0].position.x = screenWidth/2;
          snakePixels[0].position.y = screenHeight/2;
          currSnakeLen = 1;
          char s[] ="Snake has hit a wall";
          TraceLog(LOG_INFO, s);
        }

        //2. if snakePixels bites itself
        if( i != 0 && (abs(snakePixels[0].position.x - snakePixels[i].position.x) < size &&
              abs(snakePixels[0].position.y - snakePixels[i].position.y) < size))
        {
          snakePixels[0].position.x = screenWidth/2;
          snakePixels[0].position.y = screenHeight/2;
          currSnakeLen = 1;
          char s[] ="Snake has collided with itself";
          TraceLog(LOG_INFO, s);
        }
      }

      //if snakePixels eats the food
      if(abs(snakePixels[0].position.x - food.position.x) <= size &&
          abs(snakePixels[0].position.y - food.position.y) <= size)
      {
        addSnakePix();

        char s[128];
        sprintf(s, "Snake has eaten food at: [%lf,%lf] ", food.position.x, food.position.y);
        TraceLog(LOG_INFO, s);
        setFoodPosition();

        currScore++;
        if(currScore % 5 == 0 && frameCounterMod > 2) frameCounterMod--;
      }

      for(int i = 1; i < currSnakeLen; i++)
      {
        snakePixels[i].position = snakePixelsPos[i-1];
      }

      // update snakePixels position
      for(int i = 0; i < currSnakeLen; i++)
        snakePixelsPos[i] = snakePixels[i].position;

      char s[128];
      sprintf(s, "Snake head position: [%lf,%lf] ", snakePixels[0].position.x, snakePixels[0].position.y);
      TraceLog(LOG_DEBUG, s);
    }
    frameCounter++;
  
  }
}

void DrawGame()
{
  // Draw
  BeginDrawing();
  DrawFPS(0, 0);

  screenHeight = GetScreenHeight();
  screenWidth  = GetScreenWidth();

  ClearBackground(BLACK); // set background color

  UpdateGame();

  // for 0th position draw rounded rectangle
  Rectangle rect = {snakePixels[0].position.x,
      snakePixels[0].position.y, 
      size, size};

  DrawRectangleRounded(rect, 0.8, 5, RED);
                              
  // Draw Snake
  for(int i = 1; i < currSnakeLen; i++)
  {
    DrawRectangle(snakePixels[i].position.x,
                  snakePixels[i].position.y, 
                  size, 
                  size, 
                  snakePixels[i].color);
  }
  
  // Draw Food
  DrawRectangle(food.position.x,
          food.position.y, 
          size, 
          size, 
          food.color);

  // show status message on top half

  sprintf(statusMsg, "Current Score: %d | %s", currScore, warningMsg);
  DrawText(statusMsg, screenWidth/2 , 0, 15, WHITE);

  EndDrawing();
}

struct Snake initSnake()
{
  struct Snake snakePixels = {
    .position = {0, 0},
    .color    = DARKBLUE,
  };
  return snakePixels;
}

struct Food initFood()
{
  struct Food food = {
    .position = {0, 0},
    .color = BLUE,
  };
  return food;
}

// FIXME: check if food position collids with any snake pixel
void setFoodPosition()
{
  Vector2 newPos = {GetRandomValue(size, screenWidth-size),
    GetRandomValue(size, screenHeight-size)};
  int i = 0;
  while(i < currSnakeLen)
  {
    if(abs(snakePixelsPos[i].x - newPos.x) < size && 
        abs(snakePixelsPos[i].y - newPos.y) < size)
    {
      Vector2 tempPos =  {GetRandomValue(5, screenWidth), GetRandomValue(5, screenHeight)};
      newPos = tempPos;
      i = 0;
      continue;
    }
    i++;
  }
  food.position = newPos;

  char s[128];
  sprintf(s, "Food spawned at: [%lf, %lf]", food.position.x, food.position.y);
  TraceLog(LOG_INFO, s);
}

void incrementPixPosition(int pixIdx, Directions snakeDir)
{
  switch (snakeDir)
  {
    case UP:
      snakePixels[pixIdx].position.y -= size;
      break;
    case DOWN:
      snakePixels[pixIdx].position.y += size;
      break;
    case LEFT:
      snakePixels[pixIdx].position.x -= size;
      break;
    case RIGHT:
      snakePixels[pixIdx].position.x += size;
      break;
  }
}

void addSnakePix()
{
  snakePixels[currSnakeLen] = initSnake();
  snakePixels[currSnakeLen].position = snakePixels[currSnakeLen-1].position;

  currSnakeLen++;
}
