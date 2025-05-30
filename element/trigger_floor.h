#ifndef TRIGGERFLOOR_H
#define TRIGGERFLOOR_H

#include "element.h"
#include <allegro5/allegro.h>
#include <stdbool.h>

#include "../global.h"
#define TRIGGER_FLOOR_ANIM_FRAMES 10  // 動畫總幀數

typedef struct _TriggerFloor {
    int x, y;                         // 圖片位置
    int width, height;               // 單格大小
    ALLEGRO_BITMAP *img;             // 預設靜態地板圖片
    ALLEGRO_BITMAP *anim[TRIGGER_FLOOR_ANIM_FRAMES];  // 動畫圖片陣列

    double trigger_time_map[map_block_height][map_block_width]; // 每格觸發時間
    bool disappearing[map_block_height][map_block_width]; 
    bool on_trigger_floor[map_block_height][map_block_width];
} TriggerFloor;

Elements *New_TriggerFloor(int label);
void TriggerFloor_update(Elements *self);
void TriggerFloor_interact(Elements *self);
void TriggerFloor_draw(Elements *self);
void TriggerFloor_destory(Elements *self);
void Load_TriggerFloor_Images(ALLEGRO_BITMAP **anim_array);
void _TriggerFloor_interact_Character(Elements *self, Elements *tar);

// 額外新增：載入所有動畫圖片
void Load_TriggerFloor_Images(ALLEGRO_BITMAP **anim_array);

#endif
