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
#define DEBUG_LINE_LENGTH 0.2f

#define WIDTH 1280
#define HEIGHT 720

#define ROWS_NUMBER 8
#define BRICKS_PADDING 15.f
#define BRICKS_MARGIN 10.f
#define BRICKS_AREA_HEIGHT 300.f

#define WALLS_WIDTH 10.f

#define PHYSICS_SUBSTEP_COUNT 4

typedef struct Ball {
  Color color;
  float radius;
  b2Vec2 initial_velocity;
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
  if (IsKeyPressed(KEY_SPACE)) {
    b2Vec2 restart_position = (b2Vec2){(float)WIDTH / 2, HEIGHT - (float)HEIGHT / 2};
    b2Body_SetTransform(game->ball.body_id, restart_position, b2Rot_identity);
    b2Body_SetLinearVelocity(game->ball.body_id, game->ball.initial_velocity);
  }
#endif

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
  b2Vec2 position = b2Body_GetPosition(player->body_id);

  if (position.x < 0.f) {
    position.x = 0;
    b2Body_SetTransform(player->body_id, position, b2Rot_identity);
  }
  if (position.x + player->width > WIDTH) {
    position.x = WIDTH - player->width;
    b2Body_SetTransform(player->body_id, position, b2Rot_identity);
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

void CreatePlayer(Game *game) {
  b2BodyDef player_body_def = b2DefaultBodyDef();
  player_body_def.type = b2_kinematicBody;
  player_body_def.position =
      (b2Vec2){(float)WIDTH / 2 - game->player.width / 2, (float)HEIGHT - (float)HEIGHT / 5};
  b2BodyId player_body_id = b2CreateBody(game->world_id, &player_body_def);

  game->player.body_id = player_body_id;

  b2Polygon player_collider = b2MakeBox(game->player.width / 2, game->player.height / 2);
  b2ShapeDef player_shape_def = b2DefaultShapeDef();
  player_shape_def.material.friction = 0.3f;
  player_shape_def.material.restitution = 1.2f;
  b2CreatePolygonShape(game->player.body_id, &player_shape_def, &player_collider);
}

void CreateBall(Game *game) {
  b2BodyDef ball_body_def = b2DefaultBodyDef();
  ball_body_def.type = b2_dynamicBody;
  ball_body_def.position = (b2Vec2){(float)WIDTH / 2, (float)HEIGHT - (float)HEIGHT / 2};
  b2BodyId ball_body_id = b2CreateBody(game->world_id, &ball_body_def);

  game->ball.body_id = ball_body_id;

  b2Circle ball_collider = (b2Circle){{0, 0}, game->ball.radius};
  b2ShapeDef ball_shape_def = b2DefaultShapeDef();
  ball_body_def.linearDamping = 0.0f;
  ball_body_def.angularDamping = 0.0f;
  ball_shape_def.material.restitution = 1.0f;
  ball_shape_def.material.friction = 0.0f;
  b2CreateCircleShape(game->ball.body_id, &ball_shape_def, &ball_collider);
}

void CreateWalls(Game *game) {
  b2BodyDef wall_body_def = b2DefaultBodyDef();
  wall_body_def.type = b2_staticBody;
  b2BodyId wall_body_id = b2CreateBody(game->world_id, &wall_body_def);

  b2ShapeDef wall_shape_def = b2DefaultShapeDef();
  wall_shape_def.material.restitution = 1.0f;
  wall_shape_def.material.friction = 0.0f;

  wall_body_def.position = (b2Vec2){-WALLS_WIDTH, (float)HEIGHT / 2};
  b2BodyId left_wall = b2CreateBody(game->world_id, &wall_body_def);
  b2Polygon left_wall_collider = b2MakeBox(WALLS_WIDTH, (float)HEIGHT / 2);
  b2CreatePolygonShape(left_wall, &wall_shape_def, &left_wall_collider);

  wall_body_def.position = (b2Vec2){WIDTH + 10.f, (float)HEIGHT / 2};
  b2BodyId right_wall = b2CreateBody(game->world_id, &wall_body_def);
  b2Polygon right_wall_collider = b2MakeBox(WALLS_WIDTH, (float)HEIGHT / 2);
  b2CreatePolygonShape(right_wall, &wall_shape_def, &right_wall_collider);

  wall_body_def.position = (b2Vec2){(float)WIDTH / 2, -WALLS_WIDTH};
  b2BodyId up_wall = b2CreateBody(game->world_id, &wall_body_def);
  b2Polygon up_wall_collider = b2MakeBox((float)WIDTH / 2, WALLS_WIDTH);
  b2CreatePolygonShape(up_wall, &wall_shape_def, &up_wall_collider);
}

int main(void) {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = b2Vec2_zero;
  world_def.enableContinuous = 1;
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
                       .initial_velocity = {100.f, 500.f},
                   },
               .bricks = malloc(sizeof(Brick) * game.BRICKS_IN_ROW * ROWS_NUMBER),
               .world_id = world_id};

  CreatePlayer(&game);
  CreateBall(&game);
  CreateWalls(&game);

  CalculateBrickDimensions(&game);
  InitBricksPositions(&game);

  InitWindow(WIDTH, HEIGHT, "Game");

  SetTargetFPS(60);

  HideCursor();
  DisableCursor();

  b2Body_SetLinearVelocity(game.ball.body_id, game.ball.initial_velocity);

  while (!WindowShouldClose()) {
    ProcessInput(&game);

    b2World_Step(game.world_id, GetFrameTime(), PHYSICS_SUBSTEP_COUNT);

    ClampPlayerMovement(&game.player);
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
    b2Vec2 ball_vel = b2Body_GetLinearVelocity(game.ball.body_id);
    b2Vec2 ball_world_pos = b2Body_GetPosition(game.ball.body_id);

    DrawLine(ball_world_pos.x, ball_world_pos.y, DEBUG_LINE_LENGTH * ball_vel.x + ball_world_pos.x,
             DEBUG_LINE_LENGTH * ball_vel.y + ball_world_pos.y, RED);

    DrawFPS(10, HEIGHT - 30.f);
    DrawText(TextFormat("Ball Vel: (%.2f, %.2f)", ball_vel.x, ball_vel.y), 10, 10, 20, WHITE);
    DrawText(TextFormat("Ball Pos: (%.1f, %.1f)", (ball_world_pos.x), (ball_world_pos.y)), 10, 35,
             20, WHITE);
    DrawText(
        TextFormat("Ball Speed: %.2f", sqrtf(ball_vel.x * ball_vel.x + ball_vel.y * ball_vel.y)),
        10, 60, 20, WHITE);
    DrawText("Press SPACE to reset ball", 10, 85, 20, WHITE);
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
