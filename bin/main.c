#include "raylib.h"
#include <box2d/collision.h>
#include <box2d/id.h>
#include <box2d/math_functions.h>
#include <box2d/types.h>
#include <iso646.h>
#include <math.h>
#include <stdio.h>
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

#define WALLS_WIDTH 10.f

#define PHYSICS_SUBSTEP_COUNT 4

#define BALL_COLLISION_DISTANCE 3.f

#define PIXELS_PER_METER 50.0f
#define METERS_PER_PIXEL (1.0f / PIXELS_PER_METER)

#define WORLD_TO_PIXELS(x) ((x) * PIXELS_PER_METER)
#define PIXELS_TO_WORLD(x) ((x) * METERS_PER_PIXEL)

typedef struct Ball {
  Color color;
  float radius;
  float max_speed;
  float min_speed_multiplier;
  b2Vec2 initial_velocity;
  b2BodyId body_id;
} Ball;

typedef struct Brick {
  Vector2 position;
  bool active;
  b2BodyId body_id;
} Brick;

typedef struct Player {
  float movement_speed;
  float width;
  float height;
  Color color;
  b2BodyId body_id;
  b2Polygon collider;
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
    velocity.x = -PIXELS_TO_WORLD(player->movement_speed);
  }

  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_K)) {
    velocity.x = PIXELS_TO_WORLD(player->movement_speed);
  }

#ifdef DEBUGGING
  if (IsKeyPressed(KEY_SPACE)) {
    b2Vec2 restart_position =
        (b2Vec2){PIXELS_TO_WORLD((float)WIDTH / 2), PIXELS_TO_WORLD(HEIGHT - (float)HEIGHT / 2)};
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

void ClampPlayerMovement(Player *player) {
  b2Vec2 position = b2Body_GetPosition(player->body_id);

  float half_width = PIXELS_TO_WORLD(player->width / 2);

  if (position.x - half_width < 0.f) {
    position.x = half_width;
    b2Body_SetTransform(player->body_id, position, b2Rot_identity);
  }
  if (position.x + half_width > PIXELS_TO_WORLD(WIDTH)) {
    position.x = PIXELS_TO_WORLD(WIDTH) - half_width;
    b2Body_SetTransform(player->body_id, position, b2Rot_identity);
  }
}

void ClampBallMovement(Game *game) {
  b2Vec2 velocity = b2Body_GetLinearVelocity(game->ball.body_id);
  float current_speed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);

  if (current_speed > game->ball.max_speed) {
    float scale = game->ball.max_speed / current_speed;
    b2Vec2 clamped_velocity = {velocity.x * scale, velocity.y * scale};
    b2Body_SetLinearVelocity(game->ball.body_id, clamped_velocity);
  } else if (current_speed > 0.1f && current_speed < game->ball.max_speed * 0.5f) {
    float target_speed = game->ball.max_speed * game->ball.min_speed_multiplier;
    float scale = target_speed / current_speed;
    b2Vec2 boosted_velocity = {velocity.x * scale, velocity.y * scale};
    b2Body_SetLinearVelocity(game->ball.body_id, boosted_velocity);
  }
}

void DrawBricks(Game *game) {
  for (int i = 0; i < game->BRICKS_IN_ROW; i++) {
    for (int j = 0; j < ROWS_NUMBER; j++) {
      int brick_index = i + j * game->BRICKS_IN_ROW;
      Vector2 *position = &game->bricks[brick_index].position;

      int color_index = j % ROWS_NUMBER;

      if (game->bricks[brick_index].active) {
        DrawRectangleV(*position, (Vector2){game->brick_width, game->brick_height},
                       game->rows_colors[color_index]);
      }
    }
  }
}

void DrawPlayer(Game *game) {
  Player *player = &game->player;
  b2Vec2 position = b2Body_GetPosition(player->body_id);

  float screen_x = WORLD_TO_PIXELS(position.x) - player->width / 2;
  float screen_y = WORLD_TO_PIXELS(position.y) - player->height / 2;

  DrawRectangle(screen_x, screen_y, player->width, player->height, player->color);
}

void DrawBall(Game *game) {
  b2Vec2 position = b2Body_GetPosition(game->ball.body_id);

  float screen_x = WORLD_TO_PIXELS(position.x);
  float screen_y = WORLD_TO_PIXELS(position.y);

  DrawCircle(screen_x, screen_y, game->ball.radius, game->ball.color);
}

void CreatePlayer(Game *game) {
  b2BodyDef player_body_def = b2DefaultBodyDef();
  player_body_def.type = b2_kinematicBody;
  player_body_def.position =
      (b2Vec2){PIXELS_TO_WORLD((float)WIDTH / 2), PIXELS_TO_WORLD(HEIGHT - (float)HEIGHT / 5)};
  game->player.body_id = b2CreateBody(game->world_id, &player_body_def);

  b2Polygon player_collider =
      b2MakeBox(PIXELS_TO_WORLD(game->player.width / 2), PIXELS_TO_WORLD(game->player.height / 2));
  b2ShapeDef player_shape_def = b2DefaultShapeDef();
  player_shape_def.material.friction = 0.0f;
  player_shape_def.material.restitution = 1.2f;
  b2CreatePolygonShape(game->player.body_id, &player_shape_def, &player_collider);

  game->player.collider = player_collider;
}

void CreateBall(Game *game) {
  b2BodyDef ball_body_def = b2DefaultBodyDef();
  ball_body_def.type = b2_dynamicBody;
  ball_body_def.position =
      (b2Vec2){PIXELS_TO_WORLD((float)WIDTH / 2), PIXELS_TO_WORLD(HEIGHT - (float)HEIGHT / 2)};
  ball_body_def.linearDamping = 0.0f;
  ball_body_def.angularDamping = 0.0f;

  game->ball.body_id = b2CreateBody(game->world_id, &ball_body_def);

  b2Circle ball_collider = {{0, 0}, PIXELS_TO_WORLD(game->ball.radius)};
  b2ShapeDef ball_shape_def = b2DefaultShapeDef();
  ball_shape_def.material.restitution = 1.0f;
  ball_shape_def.material.friction = 0.0f;
  b2CreateCircleShape(game->ball.body_id, &ball_shape_def, &ball_collider);
}

void CreateWalls(Game *game) {
  b2BodyDef wall_body_def = b2DefaultBodyDef();
  wall_body_def.type = b2_staticBody;

  b2ShapeDef wall_shape_def = b2DefaultShapeDef();
  wall_shape_def.material.restitution = 1.0f;
  wall_shape_def.material.friction = 0.0f;

  wall_body_def.position =
      (b2Vec2){PIXELS_TO_WORLD(-WALLS_WIDTH), PIXELS_TO_WORLD((float)HEIGHT / 2)};
  b2BodyId left_wall = b2CreateBody(game->world_id, &wall_body_def);
  b2Polygon left_wall_collider =
      b2MakeBox(PIXELS_TO_WORLD(WALLS_WIDTH), PIXELS_TO_WORLD((float)HEIGHT / 2));
  b2CreatePolygonShape(left_wall, &wall_shape_def, &left_wall_collider);

  wall_body_def.position =
      (b2Vec2){PIXELS_TO_WORLD(WIDTH + WALLS_WIDTH), PIXELS_TO_WORLD((float)HEIGHT / 2)};
  b2BodyId right_wall = b2CreateBody(game->world_id, &wall_body_def);
  b2Polygon right_wall_collider =
      b2MakeBox(PIXELS_TO_WORLD(WALLS_WIDTH), PIXELS_TO_WORLD((float)HEIGHT / 2));
  b2CreatePolygonShape(right_wall, &wall_shape_def, &right_wall_collider);

  wall_body_def.position =
      (b2Vec2){PIXELS_TO_WORLD((float)WIDTH / 2), PIXELS_TO_WORLD(-WALLS_WIDTH)};
  b2BodyId up_wall = b2CreateBody(game->world_id, &wall_body_def);
  b2Polygon up_wall_collider =
      b2MakeBox(PIXELS_TO_WORLD((float)WIDTH / 2), PIXELS_TO_WORLD(WALLS_WIDTH));
  b2CreatePolygonShape(up_wall, &wall_shape_def, &up_wall_collider);
}

void CreateBricks(Game *game) {
  CalculateBrickDimensions(game);

  b2BodyDef brick_body_def = b2DefaultBodyDef();
  brick_body_def.type = b2_staticBody;

  b2ShapeDef brick_shape_def = b2DefaultShapeDef();
  brick_shape_def.material.restitution = 1.0f;
  brick_shape_def.material.friction = 0.0f;

  Brick *bricks = game->bricks;

  for (int i = 0; i < game->BRICKS_IN_ROW; i++) {
    for (int j = 0; j < ROWS_NUMBER; j++) {
      int brick_index = i + j * game->BRICKS_IN_ROW;
      Vector2 *position = &bricks[brick_index].position;

      bricks[brick_index].active = true;

      position->x = BRICKS_MARGIN + i * (game->brick_width + BRICKS_PADDING);
      position->y = BRICKS_MARGIN + j * (game->brick_height + BRICKS_PADDING);

      brick_body_def.position = (b2Vec2){PIXELS_TO_WORLD(position->x + game->brick_width / 2),
                                         PIXELS_TO_WORLD(position->y + game->brick_height / 2)};

      bricks[brick_index].body_id = b2CreateBody(game->world_id, &brick_body_def);

      b2Polygon brick_collider = b2MakeBox(PIXELS_TO_WORLD(game->brick_width / 3),
                                           PIXELS_TO_WORLD(game->brick_height / 3));
      b2CreatePolygonShape(bricks[brick_index].body_id, &brick_shape_def, &brick_collider);
    }
  }
}

void CheckBallBrickCollisions(Game *game) {
  b2Vec2 ball_position = b2Body_GetPosition(game->ball.body_id);

  float ball_screen_x = WORLD_TO_PIXELS(ball_position.x);
  float ball_screen_y = WORLD_TO_PIXELS(ball_position.y);

  for (int i = 0; i < game->BRICKS_IN_ROW; i++) {
    for (int j = 0; j < ROWS_NUMBER; j++) {
      int brick_index = i + j * game->BRICKS_IN_ROW;

      if (!game->bricks[brick_index].active) {
        continue;
      }

      Vector2 *position = &game->bricks[brick_index].position;

      if (CheckCollisionCircleRec(
              (Vector2){ball_screen_x, ball_screen_y}, game->ball.radius + 1.f,
              (Rectangle){position->x, position->y, game->brick_width, game->brick_height})) {
        if (!B2_IS_NULL(game->bricks[brick_index].body_id)) {
          b2DestroyBody(game->bricks[brick_index].body_id);
          game->bricks[brick_index].body_id = b2_nullBodyId;
        }

        game->bricks[brick_index].active = false;

        b2Vec2 ball_vel = b2Body_GetLinearVelocity(game->ball.body_id);
        b2Vec2 new_vel = {ball_vel.x, -ball_vel.y};
        b2Body_SetLinearVelocity(game->ball.body_id, new_vel);

        return;
      }
    }
  }
}

void DestroyAllBricks(Game *game) {
  for (int i = 0; i < game->BRICKS_IN_ROW * ROWS_NUMBER; i++) {
    if (!B2_IS_NULL(game->bricks[i].body_id)) {
      b2DestroyBody(game->bricks[i].body_id);
      game->bricks[i].body_id = b2_nullBodyId;
    }
  }
}

int main(void) {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = b2Vec2_zero;
  world_def.enableContinuous = true;
  b2WorldId world_id = b2CreateWorld(&world_def);

  Game game = {.BRICKS_IN_ROW = 8,
               .rows_colors = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK, MAROON},
               .background_color = BLACK,
               .player =
                   {
                       .movement_speed = 400.f,
                       .width = 100,
                       .height = 15,
                       .color = DARKBLUE,
                   },
               .ball =
                   {
                       .color = WHITE,
                       .radius = 8.f,
                       .max_speed = PIXELS_TO_WORLD(400.f),
                       .min_speed_multiplier = 0.8f,
                       .initial_velocity = {PIXELS_TO_WORLD(150.f), PIXELS_TO_WORLD(300.f)},
                   },
               .bricks = malloc(sizeof(Brick) * game.BRICKS_IN_ROW * ROWS_NUMBER),
               .world_id = world_id};

  CreatePlayer(&game);
  CreateBall(&game);
  CreateWalls(&game);
  CreateBricks(&game);

  InitWindow(WIDTH, HEIGHT, "Breakout - Fixed Brick Collision");
  SetTargetFPS(60);

  b2Body_SetLinearVelocity(game.ball.body_id, game.ball.initial_velocity);

  while (!WindowShouldClose()) {
    ProcessInput(&game);

    CheckBallBrickCollisions(&game);

    float frame_time = GetFrameTime();
    int steps = (int)(frame_time / (1.0f / 120.0f)) + 1;
    float time_step = frame_time / steps;

    for (int i = 0; i < steps; i++) {
      b2World_Step(game.world_id, time_step, 8);
    }

    ClampPlayerMovement(&game.player);
    ClampBallMovement(&game);

    b2Vec2 bp = b2Body_GetPosition(game.ball.body_id);

    if (bp.y >= PIXELS_TO_WORLD(HEIGHT)) {
      b2Vec2 restart_position =
          (b2Vec2){PIXELS_TO_WORLD((float)WIDTH / 2), PIXELS_TO_WORLD(HEIGHT - (float)HEIGHT / 2)};
      b2Body_SetTransform(game.ball.body_id, restart_position, b2Rot_identity);
      b2Body_SetLinearVelocity(game.ball.body_id, game.ball.initial_velocity);
    }

    BeginDrawing();
    ClearBackground(game.background_color);

    DrawBricks(&game);
    DrawPlayer(&game);
    DrawBall(&game);

#ifdef DEBUGGING
    b2Vec2 ball_vel = b2Body_GetLinearVelocity(game.ball.body_id);
    float screen_x = WORLD_TO_PIXELS(bp.x);
    float screen_y = WORLD_TO_PIXELS(bp.y);

    DrawLine(screen_x, screen_y, screen_x + ball_vel.x * DEBUG_LINE_LENGTH,
             screen_y + ball_vel.y * DEBUG_LINE_LENGTH, RED);

    DrawFPS(10, HEIGHT - 30.f);
    DrawText(TextFormat("Ball Vel: (%.2f, %.2f)", ball_vel.x, ball_vel.y), 10, 10, 20, WHITE);
    DrawText(TextFormat("Ball Pos: (%.1f, %.1f)", screen_x, screen_y), 10, 35, 20, WHITE);
#endif

    EndDrawing();
  }

  DestroyAllBricks(&game);
  b2DestroyWorld(world_id);
  free(game.bricks);
  CloseWindow();
  return 0;
}
