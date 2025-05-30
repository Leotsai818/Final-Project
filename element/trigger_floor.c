#include "trigger_floor.h"
#include "charater.h"
#include "../scene/gamescene.h" // for element label
#include "../scene/sceneManager.h" // for scene variable
#include <stdio.h>
#include <allegro5/allegro_primitives.h>

#include "../global.h"
#define ANIM_FRAMES 20
#define FRAME_DURATION 0.1

ALLEGRO_BITMAP *trigger_floor_anim[ANIM_FRAMES];

void Load_TriggerFloor_Images(ALLEGRO_BITMAP **anim_array) {
    char path[128];
    for (int i = 0; i < ANIM_FRAMES; i++) {
        sprintf(path, "assets/image/floor_anim/%d.png", i);
        trigger_floor_anim[i] = al_load_bitmap(path);
    }
}

Elements *New_TriggerFloor(int label)
{
    TriggerFloor *pDerivedObj = (TriggerFloor *)malloc(sizeof(TriggerFloor));
    Elements *pObj = New_Elements(label);

    pDerivedObj->img = al_load_bitmap("assets/image/gravel.jpg");
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img) * 2 / 3;
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img) * 2 / 3;
    pDerivedObj->x = 0;
    pDerivedObj->y = 0;

    // animation control
    memset(pDerivedObj->disappearing, 0, sizeof(pDerivedObj->disappearing));
    memset(pDerivedObj->trigger_time_map, 0, sizeof(pDerivedObj->trigger_time_map));
    memset(pDerivedObj->on_trigger_floor, 0, sizeof(pDerivedObj->on_trigger_floor));

    Load_TriggerFloor_Images(trigger_floor_anim);

    pObj->inter_obj[pObj->inter_len++] = Character_L;
    pObj->pDerivedObj = pDerivedObj;
    pObj->Draw = TriggerFloor_draw;
    pObj->Update = TriggerFloor_update;
    pObj->Interact = TriggerFloor_interact;
    pObj->Destroy = TriggerFloor_destory;
    return pObj;
}

void TriggerFloor_update(Elements *ele)
{
    TriggerFloor *floor = (TriggerFloor *)(ele->pDerivedObj);
    double now = al_get_time();
    for (int i = 0; i < map_block_height; i++) {
        for (int j = 0; j < map_block_width; j++) {
            if (floor->disappearing[i][j]) {
                double elapsed = now - floor->trigger_time_map[i][j];
                int frame = (int)(elapsed / FRAME_DURATION);
                if (frame >= ANIM_FRAMES) {
                    map_data[i][j] = 0;  // remove tile
                    floor->disappearing[i][j] = false;
                }
            }
        }
    }
}

void TriggerFloor_interact(Elements *self)
{
    ElementVec labelEle = _Get_label_elements(scene, Character_L);
    for (int i = 0; i < labelEle.len; i++) {
        _TriggerFloor_interact_Character(self, labelEle.arr[i]);
    }
}

void _TriggerFloor_interact_Character(Elements *self, Elements *tar)
{
    
    Character *chara = (Character *)(tar->pDerivedObj);
    TriggerFloor *triggerfloor = (TriggerFloor *)(self->pDerivedObj);
    int chara_bottom = chara->y + chara->height;

    for (int i = 0; i < map_block_height; i++) {
        for (int j = 0; j < map_block_width; j++) {
            if (map_data[i][j]==2) {
                int tile_x = triggerfloor->x + j * 50;
                int tile_y = triggerfloor->y + i * 50;
                bool collide_x = (chara->x + chara->width > tile_x) &&
                                 (chara->x < tile_x + 50);
                bool collide_y = (chara_bottom <= tile_y) &&
                                 (chara_bottom + chara->vy >= tile_y);
                if ( collide_x && collide_y) {
                    if (!triggerfloor->disappearing[i][j]) {
                        triggerfloor->disappearing[i][j] = true;
                        triggerfloor->trigger_time_map[i][j] = al_get_time();
                    }
                }
            }
        }
    }
    
}

void TriggerFloor_draw(Elements *self)
{
    TriggerFloor *Obj = ((TriggerFloor *)(self->pDerivedObj));
    double now = al_get_time();
    for (int i = 0; i < map_block_height; i++) {
        for (int j = 0; j < map_block_width; j++) {
            if (map_data[i][j]==2) {
                ALLEGRO_BITMAP *img = Obj->img;

                if (Obj->disappearing[i][j]) {
                    double elapsed = now - Obj->trigger_time_map[i][j];
                    int frame = (int)(elapsed / FRAME_DURATION);
                    if (frame >= ANIM_FRAMES)
                        continue;
                    img = trigger_floor_anim[frame];
                }

                al_draw_scaled_bitmap(img, 
                                      0, 0,
                                      al_get_bitmap_width(img), al_get_bitmap_height(img),
                                      Obj->x + j * Obj->width, Obj->y + i * Obj->height,
                                      Obj->width, Obj->height,
                                      0);
            }
        }
    }
}

void TriggerFloor_destory(Elements *self)
{
    TriggerFloor *Obj = ((TriggerFloor *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    for (int i = 0; i < ANIM_FRAMES; i++) {
        if (trigger_floor_anim[i])
            al_destroy_bitmap(trigger_floor_anim[i]);
    }
    free(Obj);
    free(self);
}