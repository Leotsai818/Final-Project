#ifndef GAMEOVER_H_INCLUDED
#define GAMEOVER_H_INCLUDED

#include "../scene/scene.h"
Scene *New_GameOver(int label);
void gameover_draw(Scene *self);
void gameover_update(Scene *self);
void gameover_destroy(Scene *self);

#endif
