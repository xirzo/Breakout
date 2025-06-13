#include "raylib.h"
#include <stdlib.h>
#include <threads.h>

#define WIDTH 1280
#define HEIGHT 720

#define ROWS_NUMBER 8
#define BRICKS_PADDING 15.f
#define BRICKS_MARGIN 10.f
#define BRICKS_AREA_HEIGHT 300.f

typedef struct Brick {
  Vector2 position;
} Brick;

typedef struct Player {
  float movement_speed;
  float width;
  float height;
  Vector2 position;
} Player;

typedef struct Game {
  const unsigned short BRICKS_IN_ROW;

  float brick_width;
  float brick_height;

  const Color rows_colors[ROWS_NUMBER];
  Player player;

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

int main(void) {
  Game game = {
      .BRICKS_IN_ROW = 8,
      .rows_colors = {RED, BROWN, GREEN, YELLOW},
      .player = {.movement_speed = 200.f,
                 .width = 80,
                 .height = 10,
                 .position = {(float)WIDTH / 2, (float)HEIGHT - (float)HEIGHT / 5}},
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

    ProcessInput(&game);

    /*
        if (CheckCollisionRecs(
                (Rectangle){game.player.position.x, game.player.position.y,
       80, 10}, (Rectangle){0, (float)HEIGHT / 2, 500, 100, BLACK})) {
        }
    */

    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (int i = 0; i < game.BRICKS_IN_ROW; i++) {
      for (int j = 0; j < ROWS_NUMBER; j++) {
        Vector2 *position = &game.bricks[i + j * game.BRICKS_IN_ROW].position;

        int color_index = j % ROWS_NUMBER;

        DrawRectangle(position->x, position->y, game.brick_width, game.brick_height,
                      game.rows_colors[color_index]);
      }
    }

    DrawRectangle(player->position.x, player->position.y, player->width, player->height, BLACK);

    EndDrawing();
  }

  free(game.bricks);
  CloseWindow();
  return 0;
}

// TODO: resizing of game
