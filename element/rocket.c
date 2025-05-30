#include "rocket.h"
#include "tree.h"
#include "../shapes/Circle.h"
#include "../scene/gamescene.h" // for element label
#include "../scene/sceneManager.h" // for scene variable

#include "../algif5/algif.h"
/*
   [Rocket function]
*/
Elements *New_Rocket(int label, int x, int y, int v)
{
    Rocket *pDerivedObj = (Rocket *)malloc(sizeof(Rocket));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->gif = algif_new_gif("assets/image/rocket.gif", -1);
    pDerivedObj->width = pDerivedObj->gif->width;;
    pDerivedObj->height = pDerivedObj->gif->height;
    pDerivedObj->x = x;
    pDerivedObj->y = y;
    pDerivedObj->v = v;
    pDerivedObj->hitbox = New_Circle(pDerivedObj->x + pDerivedObj->width / 2,
                                     pDerivedObj->y + pDerivedObj->height / 2,
                                     min(pDerivedObj->width, pDerivedObj->height) / 2);
    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Rocket_update;
    pObj->Interact = Rocket_interact;
    pObj->Draw = Rocket_draw;
    pObj->Destroy = Rocket_destory;

    return pObj;
}
void Rocket_update(Elements *self)
{
    Rocket *Obj = ((Rocket *)(self->pDerivedObj));
    _Rocket_update_position(self, Obj->v, 0);
}
void _Rocket_update_position(Elements *self, int dx, int dy)
{
    Rocket *Obj = ((Rocket *)(self->pDerivedObj));
    Obj->x += dx;
    Obj->y += dy;
    Shape *hitbox = Obj->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}
void Rocket_interact(Elements *self){}
void Rocket_draw(Elements *self)
{
    Rocket *Obj = ((Rocket *)(self->pDerivedObj));
    ALLEGRO_BITMAP *frame = algif_get_bitmap(Obj->gif, al_get_time());
    if (frame)
    {
        al_draw_bitmap(
            frame,
            Obj->x,
            Obj->y,
            (Obj->v > 0 ? ALLEGRO_FLIP_HORIZONTAL : 0)
        );
    }
}
void Rocket_destory(Elements *self)
{
    Rocket *Obj = ((Rocket *)(self->pDerivedObj));
    algif_destroy_animation(Obj->gif);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}
