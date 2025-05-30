#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "charater.h"
#include "projectile.h"
#include "rocket.h"
#include "floor.h"
#include "trigger_floor.h"
#include "../scene/sceneManager.h"
#include "../shapes/Rectangle.h"
#include "../algif5/algif.h"
#include "../scene/gamescene.h"
#include <stdio.h>
#include <stdbool.h>

#include "../global.h"
#include "../shapes/Circle.h"
#include "../shapes/Shape.h"
#define GRAVITY 1.0
#define MAX_FALL_SPEED 20
#define JUMP_SPEED -12 //數字越小(絕對值越大)跳越高
#define MOVE_SPEED 5
#define FLOOR_HEIGHT 50
#define FLOOR_WIDTH 50
extern Scene *scene;  
bool jump_key_locked = false;

/*
   [Character function]
*/
Elements *New_Character(int label)
{
    Character *pDerivedObj = (Character *)malloc(sizeof(Character));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    // load character images

    //重力
    pDerivedObj->vy = 0;
    pDerivedObj->on_ground = false;
    //...........
    pDerivedObj->jump_remain = 2;
    pDerivedObj->jumped_from_ground = false;

    //新增被擊中後反應
    pObj->inter_obj[pObj->inter_len++] = Rocket_L;
    pDerivedObj->got_hit_gif = algif_new_gif("assets/image/scared_winter.gif", -1);
    ALLEGRO_BITMAP *frame = algif_get_bitmap(pDerivedObj->got_hit_gif, al_get_time());
    pDerivedObj->got_hit_gif_width = al_get_bitmap_width(frame)*2/3;
    pDerivedObj->got_hit_gif_height = al_get_bitmap_height(frame)*2/3;
    pDerivedObj->got_hit = false;
    pDerivedObj->got_hit_hit_time = 0;
    pDerivedObj->got_hit_react_time = 0;
    pDerivedObj->vx = 0;

    ALLEGRO_SAMPLE *scream = al_load_sample("assets/sound/winter.wossle.wav");
    pDerivedObj->scream_Sound = al_create_sample_instance(scream);
    al_set_sample_instance_playmode(pDerivedObj->scream_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->scream_Sound, al_get_default_mixer());
    //...............

    char state_string[3][10] = {"stop", "move", "attack"};
    for (int i = 0; i < 3; i++)
    {
        char buffer[50];
        sprintf(buffer, "assets/image/chara_%s.gif", state_string[i]);
        pDerivedObj->gif_status[i] = algif_new_gif(buffer, -1);
    }
    pDerivedObj->gif_status[JUMP] = algif_new_gif("assets/image/chara_jump.gif", -1);
    // load effective sound
    ALLEGRO_SAMPLE *sample = al_load_sample("assets/sound/wahhh.wav");
    pDerivedObj->atk_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(pDerivedObj->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->atk_Sound, al_get_default_mixer());
    
    // initial the geometric information of character
    pDerivedObj->width = pDerivedObj->gif_status[0]->width*2/3;
    pDerivedObj->height = pDerivedObj->gif_status[0]->height*2/3;
    pDerivedObj->x = 288;  // 旗子 (250) + 寬度 (64) + 間隔 (10)
    pDerivedObj->y = HEIGHT - 50 - pDerivedObj->height;
    pDerivedObj->hitbox = New_Rectangle(pDerivedObj->x,
                                        pDerivedObj->y,
                                        pDerivedObj->x + pDerivedObj->width,
                                        pDerivedObj->y + pDerivedObj->height);
    pDerivedObj->dir = true; // true: face to right, false: face to left
    // initial the animation component
    pDerivedObj->state = STOP;
    pDerivedObj->new_proj = false;
    pObj->pDerivedObj = pDerivedObj;
    // setting derived object function
    pObj->Draw = Character_draw;
    pObj->Update = Character_update;
    pObj->Interact = Character_interact;
    pObj->Destroy = Character_destory;
    return pObj;
}


void Gravity_update(Elements *self) {
    Character *chara = ((Character *)(self->pDerivedObj));
    int chara_bottom = chara->y + chara->height;
    int chara_top = chara->y;
    int chara_left = chara->x;
    int chara_right = chara->x + chara->width;

    for (int i = 0; i < map_block_height; i++) {
        for (int j = 0; j < map_block_width; j++) {
            int tile_x = j * FLOOR_WIDTH;
            int tile_y = i * FLOOR_HEIGHT;

            // 基本條件：只處理有磚塊的格子
            if (map_data[i][j] != 1 && map_data[i][j] != 2)
                continue;

            // ========== 地面碰撞 ==========
            bool collide_x = (chara_right > tile_x) && (chara_left < tile_x + FLOOR_WIDTH);
            bool collide_y_down = (chara_bottom <= tile_y) && (chara_bottom + chara->vy >= tile_y);
            if (collide_x && collide_y_down) {
                chara->on_ground = true;
                chara->jump_remain = 2;  // ✅ 回到地面後重設跳次
                chara->jumped_from_ground = false;
                return;
            }

            // ========== 頭頂碰撞 ==========
            bool collide_y_up = (chara_top >= tile_y + FLOOR_HEIGHT) &&
                                (chara_top + chara->vy <= tile_y + FLOOR_HEIGHT);
            if (collide_x && collide_y_up && chara->vy < 0) {
                chara->vy = 0;
            }

            // ========== 左右碰撞（角色撞牆） ==========
            bool collide_y_whole = (chara_bottom > tile_y) && (chara_top < tile_y + FLOOR_HEIGHT);

            // 撞到牆的右側（角色向左移動）
            bool collide_left = (chara_left >= tile_x + FLOOR_WIDTH) &&
                                (chara_left + chara->vx <= tile_x + FLOOR_WIDTH);

            // 撞到牆的左側（角色向右移動）
            bool collide_right = (chara_right <= tile_x) &&
                                 (chara_right + chara->vx >= tile_x);

            if (collide_y_whole && collide_left && chara->vx < 0) {
                chara->vx = 0;
            }
            if (collide_y_whole && collide_right && chara->vx > 0) {
                chara->vx = 0;
            }
        }
    }

    // 如果沒踩到地板，保持自由落體狀態
if (!chara->on_ground) {
    chara->jumped_from_ground = true;  // ✅ 空中狀態視為不是從地面起跳
}
chara->on_ground = false;
}
void Character_jump(Character *chara) {
    if (chara->jump_remain > 0) {
        chara->vy = JUMP_SPEED;
        chara->jump_start_time = al_get_time();   // 記錄起跳時間
        chara->is_holding_jump = true;            // 啟用持續跳躍狀態
        chara->on_ground = false;
        chara->state = JUMP;
        chara->gif_status[JUMP]->done = false;
        chara->gif_status[JUMP]->display_index = 0;

        // ✅ 如果是從地面跳第一次
        if (!chara->jumped_from_ground) {
            chara->jumped_from_ground = true;  // 標記已從地面跳起
            chara->jump_remain = 1;            // 保留一次二段跳
        } else {
            chara->jump_remain = 0;            // 已是空中，不能再跳了
        }
    }
}

void Character_update(Elements *self)
{
    // use the idea of finite state machine to deal with different state
    Character *chara = ((Character *)(self->pDerivedObj));
   
    chara->hitbox = New_Rectangle(chara->x,
                                    chara->y,
                                    chara->x + chara->width,
                                    chara->y + chara->height);

    Gravity_update(self);
    // ==== [即時控制 vx] ====
if (key_state[ALLEGRO_KEY_A]) {
    chara->vx = -MOVE_SPEED;
    chara->dir = false;
} else if (key_state[ALLEGRO_KEY_D]) {
    chara->vx = MOVE_SPEED;
    chara->dir = true;
} else {
    chara->vx = 0;
}

    // 重力與碰撞邏輯（包括左右碰撞）
// 🌟 重力只處理 vy
if (!chara->on_ground) {
    // ✅ 模擬長按跳更高
    if (chara->is_holding_jump) {
        double jump_hold_time = al_get_time() - chara->jump_start_time;
        if (key_state[ALLEGRO_KEY_W] && jump_hold_time < 0.2) {
            chara->vy += -1.2;  // ➤ 調整為你想要的額外上升速度(數字越小跳越高)
        } else {
            chara->is_holding_jump = false;
        }
    }

    // 🧲 重力與限制
    chara->vy += GRAVITY;
    if (chara->vy > MAX_FALL_SPEED) chara->vy = MAX_FALL_SPEED;

} else {
    chara->vy = 0;
}

// 🌟 垂直移動（永遠做）
chara->y += chara->vy;

// 🌟 水平移動（獨立處理 vx，不管站地或空中）
int future_left = chara->x + chara->vx;
int future_right = chara->x + chara->vx + chara->width;
int top = chara->y;
int bottom = chara->y + chara->height;
bool block_on_left = false, block_on_right = false;

for (int i = 0; i < map_block_height; i++) {
    for (int j = 0; j < map_block_width; j++) {
        if (map_data[i][j] != 1 && map_data[i][j] != 2) continue;

        int tile_x = j * FLOOR_WIDTH;
        int tile_y = i * FLOOR_HEIGHT;
        bool vertical_overlap = !(bottom <= tile_y || top >= tile_y + FLOOR_HEIGHT);

        if (vertical_overlap) {
            if (future_left < tile_x + FLOOR_WIDTH &&
                future_right > tile_x + FLOOR_WIDTH &&
                chara->vx < 0) block_on_left = true;

            if (future_right > tile_x &&
                future_left < tile_x &&
                chara->vx > 0) block_on_right = true;
        }
    }
}

if ((chara->vx < 0 && !block_on_left) || (chara->vx > 0 && !block_on_right)) {
    chara->x += chara->vx;
} else {
    chara->vx = 0;
}

    //被擊中
    if (chara->got_hit) {
    double elapsed = al_get_time() - chara->got_hit_hit_time;

    // ✅ 0.1 秒內給予擊飛速度（只跑一次）
    if (elapsed <= 0.1) {
        if (chara->got_hit_dir) {
            chara->vx = 5;
        } else {
            chara->vx = -5;
        }
        chara->vy = -10;
        chara->on_ground = false;
        al_play_sample_instance(chara->scream_Sound);
    }

    // ❌ 刪除 on_ground 時 got_hit = false，避免動畫中斷

    // ✅ 播完動畫後切到 Game Over（你可以調整時間 1.0 秒）
    if (elapsed > 1.0) {
        window = GameOver_L;               // 切換到 Game Over
        scene->scene_end = true;   // 觸發場景切換
    }
}
    //............

    if (chara->state == STOP && !chara->got_hit)
    {
        if (!key_state[ALLEGRO_KEY_A] && !key_state[ALLEGRO_KEY_D]) {
        chara->vx = 0;
        chara->state = STOP;
    }
        if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = ATK;
        }
        else if (key_state[ALLEGRO_KEY_A])
        {
            chara->dir = false;
            chara->state = MOVE;
        }
        else if (key_state[ALLEGRO_KEY_D])
        {
            chara->dir = true;
            chara->state = MOVE;
        }
        //跳躍落下
        /*else if (key_state[ALLEGRO_KEY_W] && chara->on_ground) {
            chara->vy = JUMP_SPEED;
            chara->on_ground = false;
            chara->state = JUMP; 
            chara->gif_status[JUMP]->done = false;
    chara->gif_status[JUMP]->display_index = 0;
        }*/
        //
        else{
            chara->vx = 0;
             chara->state = STOP;
        }
    }
    else if (chara->state == MOVE && !chara->got_hit)
    {
        if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = ATK;
        }
        else if (key_state[ALLEGRO_KEY_A])
        {
            chara->dir = false;
            chara->vx = -MOVE_SPEED;
            chara->state = MOVE;
        }
        else if (key_state[ALLEGRO_KEY_D])
        {
            chara->dir = true;
            chara->vx = MOVE_SPEED;
            chara->state = MOVE;
        }
        //跳躍落下
        /*if (key_state[ALLEGRO_KEY_W] && chara->on_ground) {
            chara->vy = JUMP_SPEED;
            chara->on_ground = false;
            chara->state = JUMP;
            chara->gif_status[JUMP]->done = false;
    chara->gif_status[JUMP]->display_index = 0;
        }
        //*/
        if (chara->gif_status[chara->state]->done)
            chara->state = STOP;
    }
    else if (chara->state == ATK && !chara->got_hit)
    {
        if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = ATK;
        }
        else if (key_state[ALLEGRO_KEY_A])
        {
            chara->dir = false;
             chara->vx = -MOVE_SPEED;
            chara->state = MOVE;
        }
        else if (key_state[ALLEGRO_KEY_D])
        {
            chara->dir = true;
            chara->vx = MOVE_SPEED; 
            chara->state = MOVE;
        }
        //跳躍落下
        /*if (key_state[ALLEGRO_KEY_W] && chara->on_ground) {
            chara->vy = JUMP_SPEED;
            chara->on_ground = false;
            chara->state = JUMP;
            chara->gif_status[JUMP]->done = false;
    chara->gif_status[JUMP]->display_index = 0;
        }*/
        //
        if (chara->gif_status[chara->state]->done)
        {
            chara->state = STOP;
            chara->new_proj = false;
        }
        if (chara->gif_status[ATK]->display_index == 2 && chara->new_proj == false)
        {
            Elements *pro;
            if (chara->dir)
            {
                pro = New_Projectile(Projectile_L,
                                     chara->x + chara->width,
                                     chara->y + 10,
                                     5);
            }
            else
            {
                pro = New_Projectile(Projectile_L,
                                     chara->x - 50,
                                     chara->y + 10,
                                     -5);
            }
            _Register_elements(scene, pro);
            chara->new_proj = true;
        }
    }
    if (key_state[ALLEGRO_KEY_W]) {
    if (!jump_key_locked) {
        Character_jump(chara);          // ✅ 正確跳躍處理已集中到這裡
        jump_key_locked = true;
    }
} else {
    jump_key_locked = false;
}
    if (chara->state == JUMP && chara->gif_status[JUMP]->done) {
    chara->state = STOP;
}
}
void Character_draw(Elements *self)
{
    // with the state, draw corresponding image
    Character *chara = ((Character *)(self->pDerivedObj));
    ALLEGRO_BITMAP *frame = algif_get_bitmap(chara->gif_status[chara->state], al_get_time());
    if (frame && !chara->got_hit)
    {
        //............
        int frame_w = al_get_bitmap_width(frame) * 2/3/*scale*/;
        int frame_h = al_get_bitmap_height(frame) * 2/3/*scale*/;

        int draw_x = chara->x  ;
        int draw_y = chara->y  ;

        al_draw_scaled_bitmap(frame,
             0, 0,
             al_get_bitmap_width(frame), al_get_bitmap_height(frame),
             draw_x, draw_y ,
             frame_w, frame_h,
             (!chara->dir ? ALLEGRO_FLIP_HORIZONTAL : 0)
        );
        //..................
    }
    if (chara->state == ATK && chara->gif_status[chara->state]->display_index == 2 && !chara->got_hit)
    {
        al_play_sample_instance(chara->atk_Sound);
    }

    //被擊中....
    ALLEGRO_BITMAP *Got_Hit_Gif = algif_get_bitmap(chara->got_hit_gif, al_get_time());
    if(chara->got_hit){
        int frame_w = al_get_bitmap_width(Got_Hit_Gif) * 2/3/*scale*/;
        int frame_h = al_get_bitmap_height(Got_Hit_Gif) * 2/3/*scale*/;
        int draw_x = chara->x  ;
        int draw_y = chara->y  ;
        al_draw_scaled_bitmap(Got_Hit_Gif,
             0, 0,
             al_get_bitmap_width(Got_Hit_Gif), al_get_bitmap_height(Got_Hit_Gif),
             draw_x, draw_y ,
             frame_w, frame_h,
             (!chara->dir ? ALLEGRO_FLIP_HORIZONTAL : 0)
        );
    }
    //.........
}
void Character_destory(Elements *self)
{
    Character *Obj = ((Character *)(self->pDerivedObj));
    al_destroy_sample_instance(Obj->atk_Sound);
    algif_destroy_animation(Obj->got_hit_gif);
    algif_destroy_animation(Obj->gif_status[JUMP]);
    for (int i = 0; i < 3; i++)
        algif_destroy_animation(Obj->gif_status[i]);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}

void _Character_update_position(Elements *self, int dx, int dy)
{
    Character *chara = ((Character *)(self->pDerivedObj));
    chara->x += dx;
    chara->y += dy;
    //Shape *hitbox = chara->hitbox;
    //hitbox->update_center_x(hitbox, dx);
    //hitbox->update_center_y(hitbox, dy);
}

void Character_interact(Elements *self) {
    for (int j = 0; j < self->inter_len; j++)
    {
        int inter_label = self->inter_obj[j];
        ElementVec labelEle = _Get_label_elements(scene, inter_label);
        for (int i = 0; i < labelEle.len; i++)
        {
            if (inter_label == Rocket_L)
            {
                _Character_interact_Rocket(self, labelEle.arr[i]);
            }
        }
    }
}

void _Character_interact_Rocket(Elements *self, Elements *tar)
{
    Character *chara = ((Character *)(self->pDerivedObj));
    Rocket *roc = ((Rocket *)(tar->pDerivedObj));
    if (chara->hitbox->overlap(chara->hitbox,roc->hitbox)) {
        if (!chara->got_hit) {
            if(roc->v>0){
                chara->got_hit_dir = true;
            }else{
                chara->got_hit_dir = false;
            }
            chara->got_hit = true;
            chara->got_hit_hit_time = al_get_time();
            chara->got_hit_react_time = al_get_time();
        }
        tar->dele = true;
    }
}
