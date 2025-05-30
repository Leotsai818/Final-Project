#include "projectile.h"
#include "tree.h"
#include "../shapes/Circle.h"
#include "../scene/gamescene.h" // for element label
#include "../scene/sceneManager.h" // for scene variable

#include "../algif5/algif.h"
/*
   [Projectile function]
*/
Elements *New_Projectile(int label, int x, int y, int v)
{
    Projectile *pDerivedObj = (Projectile *)malloc(sizeof(Projectile));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->gif = algif_new_gif("assets/image/spinning_winter(small).gif", -1);
    pDerivedObj->width = pDerivedObj->gif->width*2/3;;
    pDerivedObj->height = pDerivedObj->gif->height*2/3;
    pDerivedObj->x = x;
    pDerivedObj->y = y;
    pDerivedObj->v = v;
    pDerivedObj->hitbox = New_Circle(pDerivedObj->x + pDerivedObj->width / 2,
                                     pDerivedObj->y + pDerivedObj->height / 2,
                                     min(pDerivedObj->width, pDerivedObj->height) / 2);
    // setting the interact object
    pObj->inter_obj[pObj->inter_len++] = Tree_L;
    pObj->inter_obj[pObj->inter_len++] = Floor_L;
    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Projectile_update;
    pObj->Interact = Projectile_interact;
    pObj->Draw = Projectile_draw;
    pObj->Destroy = Projectile_destory;

    return pObj;
}
void Projectile_update(Elements *self)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    _Projectile_update_position(self, Obj->v, 0);
}
void _Projectile_update_position(Elements *self, int dx, int dy)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    Obj->x += dx;
    Obj->y += dy;
    Shape *hitbox = Obj->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}
void Projectile_interact(Elements *self)
{
    for (int j = 0; j < self->inter_len; j++)
    {
        int inter_label = self->inter_obj[j];
        ElementVec labelEle = _Get_label_elements(scene, inter_label);
        for (int i = 0; i < labelEle.len; i++)
        {
            if (inter_label == Floor_L)
            {
                _Projectile_interact_Floor(self, labelEle.arr[i]);
            }
            else if (inter_label == Tree_L)
            {
                _Projectile_interact_Tree(self, labelEle.arr[i]);
            }
        }
    }
}
void _Projectile_interact_Floor(Elements *self, Elements *tar)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    if (Obj->x < 0 - Obj->width)
        self->dele = true;
    else if (Obj->x > WIDTH + Obj->width)
        self->dele = true;
}
void _Projectile_interact_Tree(Elements *self, Elements *tar)
{
    Projectile *proj = ((Projectile *)(self->pDerivedObj));
    Tree *tree = ((Tree *)(tar->pDerivedObj));

    if (tree->hitbox->overlap(tree->hitbox, proj->hitbox)) {
        if (!tree->is_hit) {
            if(proj->v < 0){
                tree->is_hit_dir = true;
            }else{
                tree->is_hit_dir = false;
            }
            tree->is_hit = true;
            tree->hit_time = al_get_time();
            tree->react_time = al_get_time();
        }
        self->dele = true;
    }
}
void Projectile_draw(Elements *self)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    ALLEGRO_BITMAP *frame = algif_get_bitmap(Obj->gif, al_get_time());
    if (frame)
    {
        al_draw_scaled_bitmap(
            frame,
        	0, 0,
            al_get_bitmap_width(frame), al_get_bitmap_height(frame),
            Obj->x, Obj->y,
            al_get_bitmap_width(frame)*2/3, al_get_bitmap_height(frame)*2/3,
            (Obj->v > 0 ? ALLEGRO_FLIP_HORIZONTAL : 0)
        );
    }
}
void Projectile_destory(Elements *self)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    algif_destroy_animation(Obj->gif);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}
