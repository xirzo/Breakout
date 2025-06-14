#include "raylib.h"
#include <box2d/collision.h>
#include <box2d/id.h>
#include <box2d/math_functions.h>
#include <box2d/types.h>
#include <stdlib.h>
#include <threads.h>
#include "box2d/box2d.h"

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

#define PHYSICS_TIME_STEP 1.0f / 60.0f
#define PHYSICS_SUBSTEP_COUNT 4

typedef struct Ball {
  Color color;
  float radius;
  float speed;
  b2BodyId body_id;
} Ball;

typedef struct Brick {
  Vector2 position;
} Brick;

typedef struct Player {
  float movement_speed;
  float width;
  float height;
  Color color;
  b2BodyId body_id;
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

  b2WorldId world_id;
} Game;

void ProcessInput(Game *game) {
  Player *player = &game->player;

  if (IsKeyDown(KEY_ESCAPE)) {
    CloseWindow();
  }

  b2Vec2 velocity = b2Vec2_zero;

  if (IsKeyDown(KEY_A) || IsKeyDown(KEY_J)) {
    velocity.x = -player->movement_speed;
  }

  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_K)) {
    velocity.x = player->movement_speed;
  }

#ifdef DEBUGGING
  // if (IsKeyPressed(KEY_SPACE)) {
  //   game->ball.position = (Vector2){(float)WIDTH / 2, HEIGHT - (float)HEIGHT / 2};
  //   game->ball.velocity = (Vector2){0.3f, 1.0f}; }
#endif /* ifdef MACRO */

  b2Body_SetLinearVelocity(game->player.body_id, velocity);
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
  // if (player->position.x < 0.f) {
  //   player->position.x = 0.f;
  // }
  // if (player->position.x + player->width > WIDTH) {
  //   player->position.x = WIDTH - player->width;
  // }
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

void DrawPlayer(Game *game) {
  Player *player = &game->player;
  b2Vec2 position = b2Body_GetPosition(player->body_id);
  DrawRectangle(position.x, position.y, player->width, player->height, player->color);
}

void DrawBall(Game *game) {
  b2Vec2 position = b2Body_GetPosition(game->ball.body_id);
  DrawCircle(position.x, position.y, game->ball.radius, game->ball.color);
}

// void MoveBall(Game *game) {
//   Ball *ball = &game->ball;
//   Vector2 *position = &game->ball.position;
//   Vector2 *velocity = &game->ball.velocity;
//
//   position->x += velocity->x * game->ball.speed * GetFrameTime();
//   position->y += velocity->y * game->ball.speed * GetFrameTime();
//
//   if (ball->position.x - ball->radius <= 0 || ball->position.x + ball->radius >= WIDTH) {
//     ball->velocity.x *= -1;
//   }
//
//   if (ball->position.y + ball->radius <= 0) {
//     ball->velocity.y *= -1;
//   }
//
//   if (CheckCollisionRecs((Rectangle){game->player.position.x, game->player.position.y, 80, 10},
//                          (Rectangle){game->ball.position.x, game->ball.position.y,
//                                      game->ball.radius * 2, game->ball.radius * 2})) {
//     ball->velocity.y *= -1;
//   }
// }

int main(void) {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = b2Vec2_zero;
  b2WorldId world_id = b2CreateWorld(&world_def);

  Game game = {.BRICKS_IN_ROW = 8,
               .rows_colors = {RED, RED, BROWN, BROWN, GREEN, GREEN, YELLOW, YELLOW},
               .background_color = BLACK,
               .player =
                   {
                       .movement_speed = 400.f,
                       .width = 80,
                       .height = 10,
                       .color = DARKBLUE,
                   },
               .ball =
                   {
                       .color = WHITE,
                       .radius = 8.f,
                       .speed = 300.f,
                   },
               .bricks = malloc(sizeof(Brick) * game.BRICKS_IN_ROW * ROWS_NUMBER),
               .world_id = world_id};

  Player *player = &game.player;

  b2BodyDef player_body_def = b2DefaultBodyDef();
  player_body_def.type = b2_dynamicBody;
  player_body_def.position =
      (b2Vec2){(float)WIDTH / 2 - player->width / 2, (float)HEIGHT - (float)HEIGHT / 5};
  b2BodyId player_body_id = b2CreateBody(world_id, &player_body_def);

  game.player.body_id = player_body_id;

  b2Polygon player_collider = b2MakeBox(player->width / 2, player->height / 2);
  b2ShapeDef player_shape_def = b2DefaultShapeDef();
  player_shape_def.material.friction = 0.3f;
  b2CreatePolygonShape(game.player.body_id, &player_shape_def, &player_collider);

  b2BodyDef ball_body_def = b2DefaultBodyDef();
  ball_body_def.type = b2_dynamicBody;
  ball_body_def.position = (b2Vec2){(float)WIDTH / 2, (float)HEIGHT - (float)HEIGHT / 2};
  b2BodyId ball_body_id = b2CreateBody(world_id, &ball_body_def);

  game.ball.body_id = ball_body_id;

  b2Polygon ball_collider = b2MakeBox(game.ball.radius, game.ball.radius);
  b2ShapeDef ball_shape_def = b2DefaultShapeDef();
  ball_shape_def.material.friction = 0.3f;
  b2CreatePolygonShape(game.ball.body_id, &ball_shape_def, &ball_collider);

  CalculateBrickDimensions(&game);
  InitBricksPositions(&game);

  InitWindow(WIDTH, HEIGHT, "Game");

  SetTargetFPS(60);

  HideCursor();
  DisableCursor();

  while (!WindowShouldClose()) {
    ProcessInput(&game);

    b2World_Step(game.world_id, PHYSICS_TIME_STEP, PHYSICS_SUBSTEP_COUNT);
    // ClampPlayerMovement(&game.player);
    // MoveBall(&game);

    // FIX: use box2d position
    // if (game.ball.position.y - game.ball.radius * 2 >= HEIGHT) {
    // TODO: DEATH (-1 LIFE)
    // }

    BeginDrawing();
    ClearBackground(game.background_color);

    DrawBricks(&game);
    DrawPlayer(&game);
    DrawBall(&game);

#ifdef DEBUGGING
    // Vector2 *ball_vel = &game.ball.velocity;
    // Vector2 *ball_pos = &game.ball.position;

    // DrawLine(ball_pos->x, ball_pos->y, ball_pos->x, ball_pos->y + DEBUG_LINE_LENGTH, RED);

    DrawFPS(10, HEIGHT - 30.f);
    // DrawText(TextFormat("Ball Vel: (%.2f, %.2f)", ball_vel->x, ball_vel->y), 10, 10, 20, WHITE);

    // float hit_pos = (ball_pos->x - game.player.position.x) / game.player.width;
    // DrawText(TextFormat("Hit Position: %.2f", hit_pos), 10, 35, 20, WHITE);
#endif

    EndDrawing();
  }

  free(game.bricks);
  CloseWindow();
  return 0;
}

// TODO: resizing of game
// TODO: better ball repel
// TODO: removal of bricks
// TODO: score
