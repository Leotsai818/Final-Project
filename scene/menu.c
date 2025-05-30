#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "menu.h"
#include <stdbool.h> 
bool key_up_locked = false;
bool key_down_locked = false;
bool key_enter_locked = false;
const char *levels[] = {"Level 1", "Level 2", "Level 3", "Level 4", "CLOSE"};
float get_pixel_text_width(const char *text, float scale);
void draw_pixel_text(ALLEGRO_BITMAP *font, const char *text, int x, int y, float scale);
int selected = 0;
int last_level_selected = 0;
/*
   [Menu function]
*/
Scene *New_Menu(int label)
{
    Menu *pDerivedObj = (Menu *)malloc(sizeof(Menu));
    Scene *pObj = New_Scene(label);
    // setting derived object member
    pDerivedObj->font = al_load_ttf_font("assets/font/pirulen.ttf", 30, 0);
    // Load sound
    pDerivedObj->song = al_load_sample("assets/sound/menu.mp3");
    pDerivedObj->bg = al_load_bitmap("assets/image/menu.jpg");
    al_reserve_samples(20);
    pDerivedObj->sample_instance = al_create_sample_instance(pDerivedObj->song);
    pDerivedObj->title_x = WIDTH / 2;
    pDerivedObj->title_y = HEIGHT / 2;
    // Loop the song until the display closes
    al_set_sample_instance_playmode(pDerivedObj->sample_instance, ALLEGRO_PLAYMODE_LOOP);
    al_restore_default_mixer();
    al_attach_sample_instance_to_mixer(pDerivedObj->sample_instance, al_get_default_mixer());
    // set the volume of instance
    al_set_sample_instance_gain(pDerivedObj->sample_instance, 1.0);
    pObj->pDerivedObj = pDerivedObj;
    // setting derived object function
    pObj->Update = menu_update;
    pObj->Draw = menu_draw;
    pObj->Destroy = menu_destroy;
    pDerivedObj->btn_close = al_load_bitmap("assets/image/Close.png");
    pDerivedObj->pixel_font = al_load_bitmap("assets/image/Text (Black) (8x10).png");
    pDerivedObj->btn_levels[0] = al_load_bitmap("assets/image/01.png");
    pDerivedObj->btn_levels[1] = al_load_bitmap("assets/image/02.png");
    pDerivedObj->btn_levels[2] = al_load_bitmap("assets/image/03.png");
    pDerivedObj->btn_levels[3] = al_load_bitmap("assets/image/50.png");
    return pObj;
}
void menu_update(Scene *self)
{
    static bool key_up_locked = false;
    static bool key_down_locked = false;
    static bool key_left_locked = false;
    static bool key_right_locked = false;
    static bool key_enter_locked = false;

    // === 上下鍵控制「關卡群 <-> EXIT」切換 ===
    if (key_state[ALLEGRO_KEY_DOWN]) {
        if (!key_down_locked && selected < 4) {
            last_level_selected = selected;
            selected = 4;
            key_down_locked = true;
        }
    } else key_down_locked = false;

    if (key_state[ALLEGRO_KEY_UP]) {
        if (!key_up_locked && selected == 4) {
            selected = last_level_selected;
            key_up_locked = true;
        }
    } else key_up_locked = false;

    // === 左右鍵控制「關卡群」內切換 ===
    if (selected < 4) {
        if (key_state[ALLEGRO_KEY_RIGHT]) {
            if (!key_right_locked) {
                selected = (selected + 1) % 4;
                key_right_locked = true;
            }
        } else key_right_locked = false;

        if (key_state[ALLEGRO_KEY_LEFT]) {
            if (!key_left_locked) {
                selected = (selected + 3) % 4;  // 相當於 -1 + 4
                key_left_locked = true;
            }
        } else key_left_locked = false;
    }

    // === Enter 選擇執行 ===
    if (key_state[ALLEGRO_KEY_ENTER]) {
        if (!key_enter_locked) {
            self->scene_end = true;
            if (selected == 4)
                window = -1;
            else
                window = selected + 1;
            key_enter_locked = true;
        }
    } else key_enter_locked = false;
}
void menu_draw(Scene *self)
{
    const char *hint_texts[5] = {
        "PLAY LEVEL ONE",
        "PLAY LEVEL TWO",
        "PLAY LEVEL THREE",
        "PLAY BOSS LEVEL",
        "EXIT"
    };

    Menu *Obj = ((Menu *)(self->pDerivedObj));
    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_draw_scaled_bitmap(Obj->bg, 0, 0,
        al_get_bitmap_width(Obj->bg), al_get_bitmap_height(Obj->bg),
        0, 0, WIDTH, HEIGHT, 0);

    // ✅ 每個按鈕的手動位置（可調整）
    int btns_x[5] = {780, 1038, 1288, 1538, 1163};
    int btns_y[5] = {825, 825, 825, 825, 1000};

    for (int i = 0; i < 5; i++) {
        ALLEGRO_BITMAP *btn_img = NULL;
        if (i < 4) btn_img = Obj->btn_levels[i];
        else       btn_img = Obj->btn_close;

        if (!btn_img) continue;

        int w = al_get_bitmap_width(btn_img);
        int h = al_get_bitmap_height(btn_img);
        int x = btns_x[i] - w / 2;
        int y = btns_y[i] - h / 2;

        // ✅ 選中時亮框
        if (i == selected) {
            ALLEGRO_COLOR highlight = al_map_rgb(255, 255, 0);
            al_draw_filled_rectangle(x - 10, y - 10, x + w + 10, y + h + 10, highlight);
        }

        al_draw_bitmap(btn_img, x, y, 0);
    }

    // ✅ 統一提示文字畫在畫面底部中央
    if (selected >= 0 && selected < 5) {
        float scale = 4.5;
        const char *text = hint_texts[selected];
        float text_width = get_pixel_text_width(text, scale);
        float center_x = WIDTH / 2.0f - text_width / 2.0f-35;
        float bottom_y = HEIGHT - 150;
        draw_pixel_text(Obj->pixel_font, text, center_x, bottom_y, scale);
    }
}  

void menu_destroy(Scene *self)
{
    Menu *Obj = ((Menu *)(self->pDerivedObj));
    al_destroy_font(Obj->font);
    al_destroy_sample(Obj->song);
    al_destroy_sample_instance(Obj->sample_instance);
    al_destroy_bitmap(Obj->bg);
    al_destroy_bitmap(Obj->btn_close);
    al_destroy_bitmap(Obj->pixel_font);
    for (int i = 0; i < 4; i++) {
    al_destroy_bitmap(Obj->btn_levels[i]);
}
    free(Obj);
    free(self);
}
