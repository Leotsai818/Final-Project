#ifndef GAMESCENE_H_INCLUDED
#define GAMESCENE_H_INCLUDED
#include "scene.h"
#include "../global.h"

/*
   [game scene object]
*/
typedef enum EleType
{
    Floor_L,
    TriggerFloor_L,
    Teleport_L,
    Tree_L,
    Character_L,
    Projectile_L,
    Rocket_L
} EleType;
typedef struct _GameScene
{
    ALLEGRO_BITMAP *background;

} GameScene;
Scene *New_GameScene(int label);
void game_scene_update(Scene *self);
void game_scene_draw(Scene *self);
void game_scene_destroy(Scene *self);
#endif
