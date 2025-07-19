#include "include/raylib.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_ALIENS 10
#define MAX_BULLETS 50

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

typedef enum {
  PLAYER,
  ALIEN,
  BULLET
} Entity_kind;


typedef struct {
  Entity_kind kind;

  Vector2 pos;
  Vector2 vel;

  Rectangle shape;
} Entity;

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "test");

  // Setup player
  Entity player = {
    .kind = PLAYER,
    .pos = (Vector2){250, 250},
    .vel = (Vector2){0, 0},
    .shape = (Rectangle){250, 250, 50, 50}
  };

  // Setup aliens
  Entity aliens[MAX_ALIENS];
  for (int i = 0; i < MAX_ALIENS; i++) {
    Entity alien = {
      .kind = ALIEN,
      .pos = (Vector2){i * 10, i * 10},
      .vel = (Vector2){1 + rand() % 10, rand() % 10},
      .shape = (Rectangle){i * 10, i * 10, 10, 10}
    };
    aliens[i] = alien;
  }

  // Setup bullets
  Entity bullets[MAX_BULLETS];
  int bullet_count = 0;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(WHITE);

    // --- Update things

    // Update player

    if(IsKeyDown(KEY_RIGHT)) player.vel.x += 1;
    if(IsKeyDown(KEY_LEFT)) player.vel.x -= 1;
    if(IsKeyDown(KEY_UP)) player.vel.y -= 1;
    if(IsKeyDown(KEY_DOWN)) player.vel.y += 1;

    player.pos.x += player.vel.x;
    player.pos.y += player.vel.y;

    if (player.pos.x < 0) {
      player.pos.x = 0;
      player.vel.x = 0;
    } else if (player.pos.x + player.shape.width > SCREEN_WIDTH) {
      player.pos.x = SCREEN_WIDTH - player.shape.width;
      player.vel.x = 0;
    }

    if (player.pos.y < 0) {
      player.pos.y = 0;
      player.vel.y = 0;
    } else if (player.pos.y + player.shape.height > SCREEN_HEIGHT) {
      player.pos.y = SCREEN_HEIGHT - player.shape.height;
      player.vel.y = 0;
    }

    player.shape = (Rectangle){player.pos.x, player.pos.y, 50, 50};

    // Update aliens
    for (int i = 0; i < MAX_ALIENS; i++) {
      aliens[i].pos.x += aliens[i].vel.x;

      if (aliens[i].pos.x < 0) {
        aliens[i].pos.x = 0;
        aliens[i].vel.x *= -1;
      } else if (aliens[i].pos.x + aliens[i].shape.width > SCREEN_WIDTH) {
        aliens[i].vel.x *= -1;
      }
      aliens[i].shape = (Rectangle){aliens[i].pos.x, aliens[i].pos.y, 10, 10};
    }

    // Update bullets
    if (IsKeyPressed(KEY_SPACE) && bullet_count < MAX_BULLETS) {
      bullets[bullet_count] = (Entity) {
        .kind = BULLET,
        .pos = (Vector2){player.pos.x, player.pos.y},
        .vel = (Vector2){0, -5},
        .shape = (Rectangle){player.pos.x, player.pos.y, 10, 10}
      };
      bullet_count++;
    }

    for (int i = 0; i < bullet_count; i++) {
      bullets[i].pos.y += bullets[i].vel.y;
      bullets[i].shape.y = bullets[i].pos.y;
    }

    // --- Draw things

    // Draw player
    DrawRectangleRec(player.shape, RED);

    // Draw aliens
    for (int i = 0; i < MAX_ALIENS; i++) {
      DrawRectangleRec(aliens[i].shape, GREEN);
    }

    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
      DrawRectangleRec(bullets[i].shape, BLUE);
    }

    EndDrawing();
  }
  CloseWindow();
  return 0;
}
