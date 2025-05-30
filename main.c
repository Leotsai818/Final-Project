#define ALLEGRO_NO_MAGIC_MAIN
#include "GameWindow.h"
#include "scene/sceneManager.h"
int real_main(int argc, char **argv)
{
    Game *game = New_Game();
    create_scene(Menu_L);  // ✅ 啟動時建立初始場景（選單）
    game->execute(game);
    game->game_destroy(game);
	return 0;
}
int main(int argc, char **argv)
{
    return al_run_main(argc, argv, real_main);
}