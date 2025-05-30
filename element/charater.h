#ifndef CHARATER_H_INCLUDED
#define CHARATER_H_INCLUDED
#include <allegro5/allegro_audio.h>
#include "element.h"
#include "../shapes/Shape.h"
#include "../algif5/algif.h"
#include <stdbool.h>
#include "../global.h"
/*
   [character object]
*/
typedef enum CharacterType
{
    STOP = 0,
    MOVE,
    ATK,
    JUMP
} CharacterType;
typedef struct _Character
{
    int x, y;

    //重力
    int vx,vy;                      // 移動速度（水平、垂直）
    bool on_ground;                 // 是否站在地上
    //..........

    //被擊中
    bool got_hit;
    bool got_hit_dir;
    ALGIF_ANIMATION *got_hit_gif;
    int got_hit_gif_width;
    int got_hit_gif_height;
    int jump_remain;  // 剩餘可跳次數
    bool jumped_from_ground;// 是否從地上起跳
    double jump_start_time;
    bool is_holding_jump;
    double got_hit_hit_time,got_hit_react_time;
    //.........

    int width, height;              // the width and height of image
    bool dir;                       // true: face to right, false: face to left
    int state;                      // the state of character
    ALGIF_ANIMATION *gif_status[4]; // gif for each state. 0: stop, 1: move, 2:attack,jump:3
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    ALLEGRO_SAMPLE_INSTANCE *scream_Sound;
    int anime;      // counting the time of animation
    int anime_time; // indicate how long the animation
    bool new_proj;
    Shape *hitbox; // the hitbox of object
} Character;
Elements *New_Character(int label);
void Character_update(Elements *self);
void Gravity_update(Elements *self);
void Character_interact(Elements *self);
void _Character_interact_Rocket(Elements *self, Elements *tar);
void Character_draw(Elements *self);
void Character_destory(Elements *self);
void _Character_update_position(Elements *self, int dx, int dy);

#endif
