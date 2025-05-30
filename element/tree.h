#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED
#include "element.h"
#include "../scene/gamescene.h" // for element label
#include "../shapes/Shape.h"

#include "../algif5/algif.h"
#include <allegro5/allegro_audio.h>
/*
   [tree object]
*/
typedef struct _Tree
{
    int x, y;          // the position of image
    int width, height; // the width and height of image
    int gif_width, gif_height; // the width and height of gif
    ALLEGRO_BITMAP *img;
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    ALGIF_ANIMATION *react_gif; // 新增：被打後的動畫
    bool is_hit;                  // 是否已經被打過
    bool is_hit_dir; 
    double hit_time,react_time;              // 被打時間，用來決定播放多久動畫
    Shape *hitbox; // the hitbox of object
} Tree;
Elements *New_Tree(int label);
void Tree_update(Elements *self);
void Tree_interact(Elements *self);
void Tree_draw(Elements *self);
void Tree_destory(Elements *self);

#endif
