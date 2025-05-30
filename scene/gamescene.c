#include <allegro5/allegro_audio.h>
#include "gamescene.h"
#include "../element/element.h"
#include "../element/charater.h"
#include "../element/floor.h"
#include "../element/teleport.h"
#include "../element/tree.h"
#include "../element/trigger_floor.h"
#include "../element/projectile.h"
#include "../element/rocket.h"
#include <allegro5/allegro_acodec.h>
#include "../global.h"
#include "../algif5/algif.h"

#define map_block_width 48
#define map_block_height 27
static ALGIF_ANIMATION *start_flag_anim = NULL; 

/*
   [GameScene function]
*/

//............. 
ALLEGRO_SAMPLE *bgm = NULL;
ALLEGRO_SAMPLE_ID bgm_id;
void Init_Audio() {
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(1);
    bgm = al_load_sample("assets/sound/bgm.wav");
    al_play_sample(bgm, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgm_id);
}

void Destroy_Audio() {
    al_destroy_sample(bgm);
}
//............

void _load_map()
{
    FILE *data;
    data = fopen("assets/map/gamescene_map.txt", "r");
    for (int i = 0; i < map_block_height; i++)
    {
        for (int j = 0; j < map_block_width; j++)
        {
            fscanf(data, "%d", &map_data[i][j]);
        }
    }
    fclose(data);
}

Scene *New_GameScene(int label)
{
    Init_Audio();
    _load_map();
    GameScene *pDerivedObj = (GameScene *)malloc(sizeof(GameScene));
    Scene *pObj = New_Scene(label);
    // setting derived object member
    pDerivedObj->background = al_load_bitmap("assets/image/stage_long.jpg");
    if (!start_flag_anim)
    start_flag_anim = algif_new_gif("assets/image/start_flag.gif", -1);
    pObj->pDerivedObj = pDerivedObj;
    // register element
    _Register_elements(pObj, New_Floor(Floor_L));
    _Register_elements(pObj, New_TriggerFloor(TriggerFloor_L));
    _Register_elements(pObj, New_Teleport(Teleport_L));
    _Register_elements(pObj, New_Tree(Tree_L));
    _Register_elements(pObj, New_Character(Character_L));
    // setting derived object function
    pObj->Update = game_scene_update;
    pObj->Draw = game_scene_draw;
    pObj->Destroy = game_scene_destroy;
    return pObj;
}
void game_scene_update(Scene *self)
{
    // update every element
    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        ele->Update(ele);
    }
    // run interact for every element
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        ele->Interact(ele);
    }
    // remove element
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        if (ele->dele)
            _Remove_elements(self, ele);
    }
}
void game_scene_draw(Scene *self)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    GameScene *gs = ((GameScene *)(self->pDerivedObj));

    /*
    al_draw_bitmap(gs->background, 0, 0, 0);
    */
   
    al_draw_scaled_bitmap(gs->background,
                      0, 0,
                      al_get_bitmap_width(gs->background), al_get_bitmap_height(gs->background), // 原圖大小
                      0, 0,
                      WIDTH, HEIGHT, // 螢幕大小
                      0);
    ALLEGRO_BITMAP *frame = algif_get_bitmap(start_flag_anim, al_get_time());
if (frame) {
    int flag_height = al_get_bitmap_height(frame);

    int draw_x = 250;  // 你希望旗子的位置（水平可自調）
    int ground_y = HEIGHT - 50;

    int draw_y = ground_y - flag_height;

    al_draw_bitmap(frame, draw_x, draw_y, 0);
}

    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        ele->Draw(ele);
    }
}
void game_scene_destroy(Scene *self)
{
    Destroy_Audio();
    GameScene *Obj = ((GameScene *)(self->pDerivedObj));
    ALLEGRO_BITMAP *background = Obj->background;
    al_destroy_bitmap(background);
    if (start_flag_anim) {
    algif_destroy_animation(start_flag_anim);
    start_flag_anim = NULL;
}
    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        ele->Destroy(ele);
    }
    free(Obj);
    free(self);
}
