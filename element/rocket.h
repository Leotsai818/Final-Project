#ifndef Rocket_H_INCLUDED
#define Rocket_H_INCLUDED
#include "element.h"
#include "../shapes/Shape.h"
#include "../algif5/algif.h"
/*
   [Rocket object]
*/

typedef struct _Rocket
{
    ALGIF_ANIMATION *gif;
    int x, y;          // the position of image
    int width, height; // the width and height of image
    int v;             // the velocity of Rocket

    Shape *hitbox; // the hitbox of object
} Rocket;
Elements *New_Rocket(int label, int x, int y, int v);
void Rocket_update(Elements *self);
void Rocket_interact(Elements *self);
void Rocket_draw(Elements *self);
void Rocket_destory(Elements *self);
void _Rocket_update_position(Elements *self, int dx, int dy);

#endif
