#include "include/raylib.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_ALIENS 10
#define MAX_BULLETS 5 

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

// TODO: Separar las bullets del jugador con las bullets de los aliens

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

    int timer;
} Entity;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");

    // Setup player
    Entity player = {
        .kind = PLAYER,
        .pos = (Vector2){250, 250},
        .vel = (Vector2){0, 0},
        .shape = (Rectangle){250, 250, 50, 50}
    };

    // Setup aliens
    Entity alive_aliens[MAX_ALIENS];
    int alive_alien_count = 0;
    Entity dead_aliens[256]; // Temporal hasta saber como reutilizar este array
    int dead_alien_count = 0;
    for (int i = 0; i < MAX_ALIENS; i++) {
        Entity alien = {
            .kind = ALIEN,
            .pos = (Vector2){i * 10, i * 10},
            .vel = (Vector2){1 + rand() % 10, rand() % 10},
            .shape = (Rectangle){i * 10, i * 10, 10, 10},
            .timer = 1 + rand() % 10 // Numero random entre 1 y 10 (inclusive)
        };
        alive_aliens[alive_alien_count] = alien;
        alive_alien_count++;
    }

    // Setup bullets
    Entity alive_bullets[MAX_BULLETS];
    Entity dead_bullets[256]; // <-- Buscar manera de reutilizar este array, temporal el 256
    int alive_bullet_count = 0;
    int dead_bullet_count = 0;

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
        for (int i = 0; i < alive_alien_count; i++) {
            alive_aliens[i].pos.x += alive_aliens[i].vel.x;

            if (alive_aliens[i].pos.x < 0) {
                alive_aliens[i].pos.x = 0;
                alive_aliens[i].vel.x *= -1;
            } else if (alive_aliens[i].pos.x + alive_aliens[i].shape.width > SCREEN_WIDTH) {
                alive_aliens[i].vel.x *= -1;
            }
            alive_aliens[i].shape = (Rectangle){alive_aliens[i].pos.x, alive_aliens[i].pos.y, 10, 10};
            
            // Alien bullet logic
            if (alive_aliens[i].timer < 0 && alive_bullet_count < MAX_BULLETS) {
                alive_bullets[alive_bullet_count] = (Entity) {
                    .kind = BULLET,
                    .pos = (Vector2){alive_aliens[i].pos.x, alive_aliens[i].pos.y},
                    .vel = (Vector2){0, 5},
                    .shape = (Rectangle){alive_aliens[i].pos.x, alive_aliens[i].pos.y, 10, 10}
                };
                alive_bullet_count++;
                alive_aliens[i].timer = 3;
            }
            alive_aliens[i].timer--;

        }

        // Player bullet logic
        if (IsKeyPressed(KEY_SPACE) && alive_bullet_count < MAX_BULLETS) {
            alive_bullets[alive_bullet_count] = (Entity) {
                .kind = BULLET,
                .pos = (Vector2){player.pos.x, player.pos.y},
                .vel = (Vector2){0, -5},
                .shape = (Rectangle){player.pos.x, player.pos.y, 10, 10}
            };
            alive_bullet_count++;
        }

        // Update position of the bullets
        for (int i = 0; i < alive_bullet_count; i++) {
            alive_bullets[i].pos.y += alive_bullets[i].vel.y;
            alive_bullets[i].shape.y = alive_bullets[i].pos.y;
        }

        // Check collision of the bullets with the aliens
        for (int i = 0; i < alive_bullet_count; i++) {
            for (int j = 0; j < alive_alien_count; j++) {
                bool collision = CheckCollisionRecs(alive_bullets[i].shape, alive_aliens[j].shape);
                if (collision) {
                    dead_aliens[dead_alien_count] = alive_aliens[j];
                    dead_alien_count++;

                    for (int k = j; k < alive_alien_count; k++) {
                        alive_aliens[k] = alive_aliens[k + 1];
                    }
                    alive_alien_count--;
                    j--;
                }
            }
        }

        // Eliminate bullets outside screen
        for (int i = 0; i < alive_bullet_count; i++) {
            if (alive_bullets[i].pos.y + alive_bullets[i].shape.height < 0 ||
                    alive_bullets[i].pos.y + alive_bullets[i].shape.height > SCREEN_HEIGHT) {
                dead_bullets[dead_bullet_count] = alive_bullets[i];
                dead_bullet_count++;

                for (int j = i; j < alive_bullet_count - 1; j++) {
                    alive_bullets[j] = alive_bullets[j + 1];
                }
                alive_bullet_count--;
                i--;
            }
        }

        // --- Draw things

        // Draw player
        DrawRectangleRec(player.shape, RED);

        // Draw aliens
        for (int i = 0; i < alive_alien_count; i++) {
            DrawRectangleRec(alive_aliens[i].shape, GREEN);
        }

        // Draw bullets
        for (int i = 0; i < alive_bullet_count; i++) {
            DrawRectangleRec(alive_bullets[i].shape, BLUE);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
