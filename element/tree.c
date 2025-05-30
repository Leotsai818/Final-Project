#include "tree.h"
#include "../shapes/Rectangle.h"

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "../element/projectile.h"
#include "../element/rocket.h"
#include "../algif5/algif.h"
double last_shot_time = 0;
#include "../scene/scene.h"
extern Scene *scene;  
/*
   [tree function]
*/
Elements *New_Tree(int label)
{
    Tree *pDerivedObj = (Tree *)malloc(sizeof(Tree));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->img = al_load_bitmap("assets/image/tree.png");
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img)*2/3;
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img)*2/3;
    pDerivedObj->x = 585;
    pDerivedObj->y = HEIGHT - pDerivedObj->height;
    pDerivedObj->hitbox = New_Rectangle(pDerivedObj->x + pDerivedObj->width / 3,
                                        pDerivedObj->y + pDerivedObj->height / 3,
                                        pDerivedObj->x + 2 * pDerivedObj->width / 3,
                                        pDerivedObj->y + 2 * pDerivedObj->height / 3);
    //新增被擊中後反應
    pDerivedObj->react_gif = algif_new_gif("assets/image/rocketpuncher.gif", -1);
    ALLEGRO_BITMAP *frame = algif_get_bitmap(pDerivedObj->react_gif, al_get_time() );
    pDerivedObj->gif_width = al_get_bitmap_width(frame)*2/3;
    pDerivedObj->gif_height = al_get_bitmap_height(frame)*2/3;
    pDerivedObj->is_hit = false;
    pDerivedObj->hit_time = 0;
    pDerivedObj->react_time = 0;
    //...............

    // load effective sound
    ALLEGRO_SAMPLE *sample = al_load_sample("assets/sound/rocketpuncher.wav");
    pDerivedObj->atk_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(pDerivedObj->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->atk_Sound, al_get_default_mixer());

    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Tree_update;
    pObj->Interact = Tree_interact;
    pObj->Draw = Tree_draw;
    pObj->Destroy = Tree_destory;
    return pObj;
}
void Tree_update(Elements *self) {
    Tree *Obj = (Tree *)(self->pDerivedObj);
    if (Obj->is_hit) {
        // 隔幾秒發射攻擊 (例：2秒)
        if (al_get_time() - Obj->hit_time >= 0.5) {
            if(Obj->is_hit_dir){
                Elements *atk = New_Rocket(Rocket_L, Obj->x + Obj->width, Obj->y+112, +7);
                _Register_elements(scene, atk);
            }else{
                Elements *atk = New_Rocket(Rocket_L, Obj->x - 100, Obj->y+112, -7);
                _Register_elements(scene, atk);
            }
            Obj->hit_time = al_get_time();
        }
        if (al_get_time() - Obj->react_time >= 0.9) {
            Obj->is_hit = false;
        }
    }
}
void Tree_interact(Elements *self) {
    
}
void Tree_draw(Elements *self)
{
    Tree *Obj = ((Tree *)(self->pDerivedObj));
    if (Obj->is_hit && Obj->react_gif) {
        ALLEGRO_BITMAP *frame = algif_get_bitmap(Obj->react_gif, al_get_time() );
        if (frame) {
            al_draw_scaled_bitmap(
                frame,
        	    0, 0,
                al_get_bitmap_width(frame), al_get_bitmap_height(frame),
                Obj->x + Obj->width - Obj->gif_width , Obj->y + Obj->height - Obj->gif_height,
                Obj->gif_width, Obj->gif_height,
                0
            );
            al_play_sample_instance(Obj->atk_Sound);
        }
    } else {
        al_draw_scaled_bitmap(
                Obj->img,
        	    0, 0,
                al_get_bitmap_width(Obj->img), al_get_bitmap_height(Obj->img),
                Obj->x, Obj->y,
                Obj->width, Obj->height,
                0
            );
    }
}
void Tree_destory(Elements *self)
{
    Tree *Obj = ((Tree *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    if (Obj->react_gif) algif_destroy_animation(Obj->react_gif);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}
