#include "gameover.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "../scene/sceneManager.h"
#include <allegro5/allegro_primitives.h>
static bool up_key_locked = false;
static bool down_key_locked = false;
static bool enter_key_locked = false;
void draw_pixel_text(ALLEGRO_BITMAP *font, const char *text, int x, int y, float scale);
float get_pixel_text_width(const char *text, float scale);

typedef struct {
    ALLEGRO_FONT *font;
    ALLEGRO_BITMAP *img;  // ✅ 新增：Game Over 圖片
    ALLEGRO_BITMAP *background;
    ALLEGRO_BITMAP *btn_restart;
    ALLEGRO_BITMAP *btn_levels;
    ALLEGRO_BITMAP *btn_close;
    ALLEGRO_BITMAP *font_bitmap;
    int selected;  // 0: Restart, 1: Levels, 2: Close
} GameOver;

Scene *New_GameOver(int label) {
    GameOver *pDerivedObj = (GameOver *)malloc(sizeof(GameOver));
    Scene *pObj = New_Scene(label);

    pDerivedObj->font = al_load_ttf_font("assets/font/pirulen.ttf", 36, 0);
    pDerivedObj->img = al_load_bitmap("assets/image/gameover.png");
    pDerivedObj->background = al_load_bitmap("assets/image/崩潰貓.jpg");
    pDerivedObj->btn_restart = al_load_bitmap("assets/image/Restart.png");
    pDerivedObj->btn_levels  = al_load_bitmap("assets/image/Levels.png");
    pDerivedObj->btn_close   = al_load_bitmap("assets/image/Close.png");
    pDerivedObj->font_bitmap = al_load_bitmap("assets/image/Text (Black) (8x10).png");
    pDerivedObj->selected = 0;  // 預設選第一個

    pObj->pDerivedObj = pDerivedObj;
    pObj->Draw = gameover_draw;
    pObj->Update = gameover_update;
    pObj->Destroy = gameover_destroy;
    return pObj;
}

void gameover_update(Scene *self)
{
    GameOver *Obj = ((GameOver *)(self->pDerivedObj));

    // ↓ 鍵盤左右鍵處理（加 key lock）
    if (key_state[ALLEGRO_KEY_RIGHT]) {
        if (!down_key_locked) {
            Obj->selected = (Obj->selected + 1) % 3;
            down_key_locked = true;
        }
    } else {
        down_key_locked = false;
    }

    if (key_state[ALLEGRO_KEY_LEFT]) {
        if (!up_key_locked) {
            Obj->selected = (Obj->selected + 2) % 3;
            up_key_locked = true;
        }
    } else {
        up_key_locked = false;
    }

    // ⏎ ENTER 鍵確認選擇（也加入 key lock）
    if (key_state[ALLEGRO_KEY_ENTER]) {
        if (!enter_key_locked) {
            switch (Obj->selected)
            {
            case 0:
                window = GameScene_L;
                break;
            case 1:
                window = Menu_L;
                break;
            case 2:
                window = -1;
                break;
            }
            self->scene_end = true;
            enter_key_locked = true;
        }
    } else {
        enter_key_locked = false;
    }
}

void gameover_draw(Scene *self) {
    GameOver *Obj = ((GameOver *)(self->pDerivedObj));

    if (Obj->background) {
        al_draw_scaled_bitmap(Obj->background,
            0, 0,
            al_get_bitmap_width(Obj->background), al_get_bitmap_height(Obj->background),
            0, 0,
            WIDTH, HEIGHT,
            0);
    }

    if (Obj->img) {
        int img_w = al_get_bitmap_width(Obj->img);
        int img_h = al_get_bitmap_height(Obj->img);
        al_draw_bitmap(Obj->img, WIDTH/2 - img_w/2 + 700, HEIGHT/2 - img_h/2 - 380, 0);
    }

    // ✅ 各按鈕個別 x, y 位置調整（自訂）
    int btns_x[3] = {WIDTH / 2 - 500, WIDTH / 2 , WIDTH / 2 +500};
    int btns_y[3] = {1150, 1150, 1150};  // Restart, Levels, Close 的 Y 位置

    ALLEGRO_BITMAP *btns[3] = {
        Obj->btn_restart,
        Obj->btn_levels,
        Obj->btn_close
    };
    const char *hint_texts[3] = {
        "RESTART",
        "MENU",
        "EXIT"
    };

    for (int i = 0; i < 3; i++) {
        int w = al_get_bitmap_width(btns[i]);
        int h = al_get_bitmap_height(btns[i]);
        int x = btns_x[i];
        int y = btns_y[i];

        if (i == Obj->selected) {
            al_draw_filled_rectangle(x - 10, y - 10, x + w + 10, y + h + 10, al_map_rgb(255, 255, 0));
            float text_width = get_pixel_text_width(hint_texts[i], 2.5);
            float center_x = x + w / 2.0f - text_width / 2.0f;
            draw_pixel_text(Obj->font_bitmap, hint_texts[i], center_x, y + h + 30, 2.5);
        }

        al_draw_bitmap(btns[i], x, y, 0);
    }
   
}
float get_pixel_text_width(const char *text, float scale) {
    int char_w = 8;
    int width = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if ((text[i] >= 'A' && text[i] <= 'Z') || text[i] == ' ')
            width += char_w;
    }
    return width * scale;
}
void draw_pixel_text(ALLEGRO_BITMAP *font, const char *text, int x, int y, float scale) {
    int char_w = 8;
    int char_h = 10;

    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];

        if (c >= 'A' && c <= 'Z') {
            int index = c - 'A';
            int src_x = (index % 10) * char_w;
            int src_y = (index / 10) * char_h;

            al_draw_scaled_bitmap(font,
                src_x, src_y,
                char_w, char_h,
                x + i * char_w * scale,
                y,
                char_w * scale,
                char_h * scale,
                0);
        }

        // 加：支援空白
        else if (c == ' ') {
            // 空白間距
            x += char_w * scale;
        }

        //（可加：數字、標點等延伸功能）
    }
}

void gameover_destroy(Scene *self) {
    GameOver *Obj = ((GameOver *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    al_destroy_font(Obj->font);
    al_destroy_bitmap(Obj->background);
    al_destroy_bitmap(Obj->btn_restart);
    al_destroy_bitmap(Obj->btn_levels);
    al_destroy_bitmap(Obj->btn_close);
    al_destroy_bitmap(Obj->font_bitmap);
    free(Obj);
    free(self);
}
