#include "include/raylib.h"
#include "include/raymath.h"
#include <stdlib.h>

#define MAX_ENTITIES 20 

typedef enum {
    PLAYER,
    ALIEN
} Entity_kind;

typedef struct {
    int handle;
    Entity_kind kind;
    
    void (*update_proc)(void*);
    void (*draw_proc)(void*);

    Vector2 pos;
    Vector2 vel;
    Vector2 acc;

    bool do_physics;

    Rectangle collision_shape;

    float think_timer;
    bool do_move;
} Entity;

typedef struct {
    int ticks;
    float game_time_elapsed;
    Entity entities[MAX_ENTITIES];
    int entity_top_count;
} Game_State;

void entity_setup(Entity* entity, Entity_kind kind);

void setup_player(Entity* entity);
void player_update(void* entity_ptr);
void player_draw(void* entity_ptr);

void setup_alien(Entity* entity);
void alien_update(void* entity_ptr);
void alien_draw(void* entity_ptr);

void physics_system(Game_State* game_state, float delta_time);
void constrain_entity_to_screen(Entity* entity, int screenWidth, int screenHeight);

const int screenWidth = 450;
const int screenHeight = 800;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - collision area");

    Game_State game_state = {60, 0, {0}}; 

    Entity player;
    entity_setup(&player, PLAYER);

    game_state.entities[game_state.entity_top_count] = player;
    game_state.entity_top_count++;


    for (int i = 0; i < MAX_ENTITIES - 1; i++) {
        Entity alien;
        entity_setup(&alien, ALIEN);
        game_state.entities[game_state.entity_top_count] = alien;
        game_state.entity_top_count++;
    }

    SetTargetFPS(game_state.ticks); 

    while (!WindowShouldClose()) 
    {
        
        float delta_time = GetFrameTime();

        //physics_system(&game_state, delta_time);

        for (int i = 0; i < game_state.entity_top_count; i++) {
            Entity* entity = &game_state.entities[i];
            if (entity->update_proc != NULL) {
                entity->update_proc(entity);
            }
        }
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < game_state.entity_top_count; i++) {
            Entity* entity = &game_state.entities[i];
            if (entity->draw_proc != NULL) {
                entity->draw_proc(entity);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}


void entity_setup(Entity* entity, Entity_kind kind) {
    entity->handle = rand() % 1000;
    entity->kind = kind;
    entity->update_proc = NULL; 
    entity->draw_proc = NULL; 
    entity->do_physics = false; 
    entity->pos = (Vector2){0, 0}; 
    entity->vel = (Vector2){0, 0}; 
    entity->acc = (Vector2){0, 0}; 
    entity->collision_shape = (Rectangle){0, 0, 20, 20};

    switch (kind) {
        case PLAYER:
            setup_player(entity);
            break;
        case ALIEN:
            setup_alien(entity);
            break;
        default:
            // Aca tendriamos que tirar algun error y crashear
            break;
    }
}

void setup_player(Entity* entity) {

    entity->pos = (Vector2){200, 100};
    entity->vel = (Vector2){10, 10};
    entity->acc = (Vector2){0.5, 0.5};

    entity->do_physics = true;

    entity->update_proc = player_update;
    entity->draw_proc = player_draw;
}

void player_update(void* entity_ptr) {
    Entity* entity = (Entity*)entity_ptr;

    if (IsKeyDown(KEY_RIGHT)) entity->pos.x += entity->vel.x;
    if (IsKeyDown(KEY_LEFT)) entity->pos.x -= entity->vel.x;
    if (IsKeyDown(KEY_UP)) entity->pos.y -= entity->vel.y;
    if (IsKeyDown(KEY_DOWN)) entity->pos.y += entity->vel.y;

    constrain_entity_to_screen(entity, screenWidth, screenHeight);

    entity->collision_shape.x = entity->pos.x;
    entity->collision_shape.y = entity->pos.y;
}

void player_draw(void* entity_ptr) {
    Entity* entity = (Entity*)entity_ptr;
    DrawRectangleRec(entity->collision_shape, BLUE);
}


void setup_alien(Entity* entity) {
    entity->pos = (Vector2){100, 100};
    entity->vel = (Vector2){1, 1};
    entity->acc = (Vector2){0, 0};

    entity->think_timer = 2.0f + (rand() % 3000) / 1000.0f;

    entity->do_physics = true;
    entity->update_proc = alien_update;
    entity->draw_proc = alien_draw;
}

void alien_update(void* entity_ptr) {
    Entity* entity = (Entity*)entity_ptr;

    float dt = GetFrameTime();
    entity->pos.x += entity->vel.x * dt;
    entity->pos.y += entity->vel.y * dt;

    constrain_entity_to_screen(entity, screenWidth, screenHeight);

    // Alien movement
    {
    // Decrease timer
    entity->think_timer -= GetFrameTime();

    // If timer runs out, pick a new direction
    if (entity->think_timer <= 0.0f) {
        float speed = 50.0f;

        // Random direction
        int dx = (rand() % 3) - 1; // -1, 0, or 1
        int dy = (rand() % 3) - 1;

        entity->vel.x = dx * speed;
        entity->vel.y = dy * speed;

        // Reset timer (2–5 sec)
        entity->think_timer = 2.0f + (rand() % 3000) / 1000.0f;
    }
    }

    entity->collision_shape.x = entity->pos.x;
    entity->collision_shape.y = entity->pos.y;
}

void alien_draw(void* entity_ptr) {
    Entity* entity = (Entity*)entity_ptr;
    DrawRectangleRec(entity->collision_shape, GREEN);
}

void constrain_entity_to_screen(Entity* entity, int screenWidth, int screenHeight){
    if (entity->pos.x < 0) {
        entity->pos.x = 0;
    } else if (entity->pos.x + entity->collision_shape.width > screenWidth) {
        entity->pos.x = screenWidth - entity->collision_shape.width;
    }

    if (entity->pos.y < 0) {
        entity->pos.y = 0;
    } else if (entity->pos.y + entity->collision_shape.height > screenHeight){
        entity->pos.y = screenHeight - entity->collision_shape.height;
    }
}

void physics_system(Game_State* game_state, float delta_time) {
    for (int i = 0; i < game_state->entity_top_count; i++) {
        if(game_state->entities[i].do_physics) {
            Entity* e = &game_state->entities[i];
            e->vel = Vector2Add(e->vel, Vector2Scale(e->acc, delta_time));
            e->pos = Vector2Add(e->pos, Vector2Scale(e->vel, delta_time));
            e->acc = (Vector2){0, 0};
        }
    }
}
