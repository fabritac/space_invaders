#include "include/raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ALIENS 10
#define MAX_BULLETS 5

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

    int timer;

    Vector2 target;
} Entity;

void update_player(Entity *player, Entity *player_bullets, int* player_bullet_count);

void draw_player(Entity *player);

void draw_player_bullets(Entity *player_bullets, int *player_bullet_count);

void draw_aliens(Entity *aliens, int *alive_alien_count);

void update_aliens(Entity* aliens, int* alive_alien_count, Entity* alien_bullets, int* alien_bullet_count);

void update_alien_bullets(Entity *alien_bullets, int *alien_bullet_count);

void draw_alien_bullets(Entity *alien_bullets, int *alien_bullet_count);

void update_player_bullets(Entity *player_bullets, int *player_bullet_count, Entity *alive_aliens, int *alive_alien_count, Entity *dead_aliens, int *dead_alien_count);

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders");

    // Setup player
    Entity player = {
        .kind = PLAYER,
        .pos = (Vector2){250, 250},
        .vel = (Vector2){0, 0},
        .shape = (Rectangle){250, 250, 10, 10}
    };

    // Setup aliens
    Entity alive_aliens[MAX_ALIENS];
    int alive_alien_count = 0;
    Entity dead_aliens[256]; // Temporal hasta saber como reutilizar este array
    int dead_alien_count = 0;
    for (int i = 0; i < MAX_ALIENS; i++) {
        Entity alien = {
            .kind = ALIEN,
            .pos = (Vector2){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT/2},
            .vel = (Vector2){1, 1},
            .shape = (Rectangle){i * 10, i * 10, 10, 10},
            .timer = 1 + rand() % 60, // Numero random entre 1 y 10 (inclusive)
            .target = (Vector2){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT}
        };
        alive_aliens[alive_alien_count] = alien;
        alive_alien_count++;
    }

    // Setup bullets
    Entity player_bullets[MAX_BULLETS];
    Entity alien_bullets[MAX_BULLETS];
    //    Entity dead_bullets[256]; // <-- Buscar manera de reutilizar este array, temporal el 256
    int player_bullet_count = 0;
    int alien_bullet_count = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(WHITE);

        update_player(&player, player_bullets, &player_bullet_count);

        update_player_bullets(player_bullets, &player_bullet_count, alive_aliens, &alive_alien_count, dead_aliens, &dead_alien_count);

        update_aliens(alive_aliens, &alive_alien_count, alien_bullets, &alien_bullet_count);

        update_alien_bullets(alien_bullets, &alien_bullet_count);


        draw_player(&player);
        draw_player_bullets(player_bullets, &player_bullet_count);

        draw_aliens(alive_aliens, &alive_alien_count);
        draw_alien_bullets(alien_bullets, &alien_bullet_count);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void update_player(Entity *player, Entity *player_bullets, int* player_bullet_count) {
    if(IsKeyDown(KEY_RIGHT)) player->vel.x += 1;
    if(IsKeyDown(KEY_LEFT)) player->vel.x -= 1;
    if(IsKeyDown(KEY_UP)) player->vel.y -= 1;
    if(IsKeyDown(KEY_DOWN)) player->vel.y += 1;

    if (IsKeyPressed(KEY_SPACE) && *player_bullet_count < MAX_BULLETS) {
        player_bullets[*player_bullet_count].kind = BULLET;
        player_bullets[*player_bullet_count].pos = (Vector2){player->pos.x, player->pos.y};
        player_bullets[*player_bullet_count].vel = (Vector2){0, -5};
        player_bullets[*player_bullet_count].shape = (Rectangle){player->pos.x, player->pos.y, 10, 10};
        (*player_bullet_count)++;
    }

    player->pos.x += player->vel.x;
    player->pos.y += player->vel.y;

    if (player->pos.x < 0) {
        player->pos.x = 0;
        player->vel.x = 0;
    } else if (player->pos.x + player->shape.width > SCREEN_WIDTH) {
        player->pos.x = SCREEN_WIDTH - player->shape.width;
        player->vel.x = 0;
    }

    if (player->pos.y < 0) {
        player->pos.y = 0;
        player->vel.y = 0;
    } else if (player->pos.y + player->shape.height > SCREEN_HEIGHT) {
        player->pos.y = SCREEN_HEIGHT - player->shape.height;
        player->vel.y = 0;
    }

    player->shape = (Rectangle){player->pos.x, player->pos.y, 10, 10};
}

void update_aliens(Entity* alive_aliens, int* alive_alien_count,
        Entity* alien_bullets, int* alien_bullet_count) {

    for (int i = 0; i < *alive_alien_count; i++) {
        alive_aliens[i].target = (Vector2){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT/2};

        if (alive_aliens[i].timer < 0) {

            if(*alien_bullet_count < MAX_BULLETS) {
                alien_bullets[*alien_bullet_count].kind = BULLET;
                alien_bullets[*alien_bullet_count].pos = (Vector2){alive_aliens[i].pos.x, alive_aliens[i].pos.y};
                alien_bullets[*alien_bullet_count].vel = (Vector2){0, 5};
                alien_bullets[*alien_bullet_count].shape = (Rectangle){alive_aliens[i].pos.x, alive_aliens[i].pos.y, 10, 10};
                (*alien_bullet_count)++;
            }

            int dx = alive_aliens[i].target.x - alive_aliens[i].pos.x;
            int dy = alive_aliens[i].target.y - alive_aliens[i].pos.y;
            float norm = sqrt(dx*dx + dy*dy);

            if (norm > 0.0f) {
                float speed = 1.0f;
                float fdx = dx/norm;
                float fdy = dy/norm;

                alive_aliens[i].vel.x = speed * fdx;
                alive_aliens[i].vel.y = speed * fdy;

                alive_aliens[i].timer = 60;
            }
        }

        alive_aliens[i].pos.x += alive_aliens[i].vel.x;
        alive_aliens[i].pos.y += alive_aliens[i].vel.y;

        alive_aliens[i].shape = (Rectangle){alive_aliens[i].pos.x, alive_aliens[i].pos.y, 10, 10};

        alive_aliens[i].timer--;
    }
}

void update_alien_bullets(Entity *alien_bullets, int *alien_bullet_count) {
    for (int i = 0; i < *alien_bullet_count; i++) {
        alien_bullets[i].pos.y += alien_bullets[i].vel.y;
        alien_bullets[i].shape.y = alien_bullets[i].pos.y;

        if (alien_bullets[i].pos.y + alien_bullets[i].shape.height > SCREEN_HEIGHT) {
            for (int j = i; j < *alien_bullet_count - 1; j++) {
                alien_bullets[j] = alien_bullets[j + 1];
            }
            (*alien_bullet_count)--;
            i--;
        }
    }
}

void draw_alien_bullets(Entity *alien_bullets, int *alien_bullet_count) {
    for (int i = 0; i < *alien_bullet_count; i++) {
        DrawRectangleRec(alien_bullets[i].shape, BLUE);
    }
}

void update_player_bullets(Entity *player_bullets, int *player_bullet_count,
        Entity *alive_aliens, int *alive_alien_count,
        Entity *dead_aliens, int *dead_alien_count) {
    for (int i = 0; i < *player_bullet_count; i++) {
        player_bullets[i].pos.y += player_bullets[i].vel.y;
        player_bullets[i].shape.y = player_bullets[i].pos.y;

        if (player_bullets[i].pos.y + player_bullets[i].shape.height < 0) {
            for (int j = i; j < *player_bullet_count - 1; j++) {
                player_bullets[j] = player_bullets[j + 1];
            }
            (*player_bullet_count)--;
            i--;
        }

        for (int j = 0; j < *alive_alien_count; j++) {
            bool collision = CheckCollisionRecs(player_bullets[i].shape, alive_aliens[j].shape);
            if (collision) {
                dead_aliens[*dead_alien_count] = alive_aliens[j];
                (*dead_alien_count)++;

                for (int k = j; k < *alive_alien_count; k++) {
                    alive_aliens[k] = alive_aliens[k + 1];
                }
                (*alive_alien_count)--;
                j--;
            }
        }
    }
}

void draw_player(Entity *player) {
    DrawRectangleRec(player->shape, RED);
}

void draw_player_bullets(Entity *player_bullets, int *player_bullet_count) {
    for (int i = 0; i < *player_bullet_count; i++) {
        DrawRectangleRec(player_bullets[i].shape, BLUE);
    }
}

void draw_aliens(Entity *aliens, int *alive_alien_count) {
    for (int i = 0; i < *alive_alien_count; i++) {
        DrawRectangleRec(aliens[i].shape, GREEN);
    }
}
