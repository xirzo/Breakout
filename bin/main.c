#include "raylib.h"
#include <stdlib.h>
#include <threads.h>

/*
 Remove #define DEBUGGING and #define DEBUG_LINE_LENGTH 10.f
*/
#define DEBUGGING
#define DEBUG_LINE_LENGTH 50.f

#define WIDTH 1280
#define HEIGHT 720

#define ROWS_NUMBER 8
#define BRICKS_PADDING 15.f
#define BRICKS_MARGIN 10.f
#define BRICKS_AREA_HEIGHT 300.f

typedef struct Ball {
  Color color;
  float radius;
  float speed;
  Vector2 position;
  Vector2 velocity;
} Ball;

typedef struct Brick {
  Vector2 position;
} Brick;

typedef struct Player {
  float movement_speed;
  float width;
  float height;
  Color color;
  Vector2 position;
} Player;

typedef struct Game {
  const unsigned short BRICKS_IN_ROW;

  float brick_width;
  float brick_height;

  Color background_color;

  const Color rows_colors[ROWS_NUMBER];
  Player player;
  Ball ball;

  Brick *bricks;
} Game;

void ProcessInput(Game *game) {
  Player *player = &game->player;

  if (IsKeyDown(KEY_ESCAPE)) {
    CloseWindow();
  }

  if (IsKeyDown(KEY_A) || IsKeyDown(KEY_J)) {
    player->position.x -= player->movement_speed * GetFrameTime();
  }

  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_K)) {
    player->position.x += player->movement_speed * GetFrameTime();
  }
}

void CalculateBrickDimensions(Game *game) {
  float available_width = WIDTH - BRICKS_MARGIN * 2;
  float horizontal_padding = (game->BRICKS_IN_ROW - 1) * BRICKS_PADDING;
  game->brick_width = (available_width - horizontal_padding) / game->BRICKS_IN_ROW;

  float available_height = BRICKS_AREA_HEIGHT;
  float vertical_padding = (ROWS_NUMBER - 1) * BRICKS_PADDING;
  game->brick_height = (available_height - vertical_padding) / ROWS_NUMBER;
}

void InitBricksPositions(Game *game) {
  Brick *bricks = game->bricks;

  for (int i = 0; i < game->BRICKS_IN_ROW; i++) {
    for (int j = 0; j < ROWS_NUMBER; j++) {
      Vector2 *position = &bricks[i + j * game->BRICKS_IN_ROW].position;

      position->x = BRICKS_MARGIN + i * (game->brick_width + BRICKS_PADDING);
      position->y = BRICKS_MARGIN + j * (game->brick_height + BRICKS_PADDING);
    }
  }
}

void ClampPlayerMovement(Player *player) {
  if (player->position.x < 0.f) {
    player->position.x = 0.f;
  }
  if (player->position.x + player->width > WIDTH) {
    player->position.x = WIDTH - player->width;
  }
}

void DrawBricks(Game *game) {
  for (int i = 0; i < game->BRICKS_IN_ROW; i++) {
    for (int j = 0; j < ROWS_NUMBER; j++) {
      Vector2 *position = &game->bricks[i + j * game->BRICKS_IN_ROW].position;

      int color_index = j % ROWS_NUMBER;

      DrawRectangleV(*position, (Vector2){game->brick_width, game->brick_height},
                     game->rows_colors[color_index]);
    }
  }
}

void DrawBall(Game *game) {
  Vector2 *position = &game->ball.position;
  DrawCircleV(*position, game->ball.radius, game->ball.color);
}

void MoveBall(Game *game) {
  Ball *ball = &game->ball;
  Vector2 *position = &game->ball.position;
  Vector2 *velocity = &game->ball.velocity;

  position->x += velocity->x * game->ball.speed * GetFrameTime();
  position->y += velocity->y * game->ball.speed * GetFrameTime();

  if (ball->position.x - ball->radius <= 0 || ball->position.x + ball->radius >= WIDTH) {
    ball->velocity.x *= -1;
  }

  if (ball->position.y + ball->radius <= 0) {
    ball->velocity.y *= -1;
  }
}

int main(void) {
  Game game = {
      .BRICKS_IN_ROW = 8,
      .rows_colors = {RED, RED, BROWN, BROWN, GREEN, GREEN, YELLOW, YELLOW},
      .background_color = BLACK,
      .player = {.movement_speed = 400.f,
                 .width = 80,
                 .height = 10,
                 .color = DARKBLUE,
                 .position = {(float)WIDTH / 2 - (game.player.width / 2),
                              (float)HEIGHT - (float)HEIGHT / 5}},
      .ball = {.color = WHITE,
               .radius = 8.f,
               .speed = 300.f,
               .position = {(float)WIDTH / 2, (float)HEIGHT - (float)HEIGHT / 2},
               .velocity = {0.0f, 1.0f}},
      .bricks = malloc(sizeof(Brick) * game.BRICKS_IN_ROW * ROWS_NUMBER),
  };

  Player *player = &game.player;

  CalculateBrickDimensions(&game);

  InitBricksPositions(&game);

  InitWindow(WIDTH, HEIGHT, "Game");

  SetTargetFPS(60);

  HideCursor();
  DisableCursor();

  while (!WindowShouldClose()) {
    ClampPlayerMovement(&game.player);
    MoveBall(&game);

    if (game.ball.position.y - game.ball.radius * 2 >= HEIGHT) {
      // TODO: DEATH (-1 LIFE)
    }

    ProcessInput(&game);
    /*
        if (CheckCollisionRecs(
                (Rectangle){game.player.position.x, game.player.position.y,
       80, 10}, (Rectangle){0, (float)HEIGHT / 2, 500, 100, BLACK})) {
        }
    */

    BeginDrawing();
    ClearBackground(game.background_color);

    DrawBricks(&game);
    DrawRectangleV(player->position, (Vector2){player->width, player->height}, player->color);
    DrawBall(&game);

#ifdef DEBUGGING
    Vector2 *ball_pos = &game.ball.position;

    DrawLine(ball_pos->x, ball_pos->y, ball_pos->x, ball_pos->y + DEBUG_LINE_LENGTH, RED);

    DrawFPS(10, HEIGHT - 30.f);
#endif

    EndDrawing();
  }

  free(game.bricks);
  CloseWindow();
  return 0;
}

// TODO: resizing of game
