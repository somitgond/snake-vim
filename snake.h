/**
 * Simple snake game with vim keybindigs
 */

#include "raylib.h"

#define DEFAULT_SIZE 10

typedef struct Snake
{
  Vector2 position;
  Color color;
  int size;
  int speed;
  bool active;
} Snake;

typedef struct Food
{
  Vector2 position;
  Color color;
  int size;
} Food;

typedef enum Directions
{
  UP    = 0,
  DOWN  = 1,
  LEFT  = 2,
  RIGHT = 3,
} Directions;

void DrawGame();
void UpdateGame();

struct Snake initSnake();
struct Food initFood();
void setFoodPosition();
void incrementPixPosition(int pixIdx, Directions snakeDir);
