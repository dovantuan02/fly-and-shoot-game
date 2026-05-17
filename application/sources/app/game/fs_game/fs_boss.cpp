#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"
#include "buzzer.h"

#include "fs_config.h"
#include "fs_bitmap.h"
#include "fs_core.h"

using namespace FsGame;

static FsBoss::BossLevel fs_game_calculate_level(int score) {
	if (score >= 30) {
		return FsBoss::BossLevel::Hard;
	}
	else if (score >= 20) {
		return FsBoss::BossLevel::Normal;
	}
	else {
		return FsBoss::BossLevel::Easy;
	}
}

void task_fs_boss_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case FS_GAME_BOSS_SETUP_SIG: {
		APP_DBG_SIG("FS_GAME_BOSS_SETUP_SIG\n");
		break;
	}
	case FS_GAME_BOSS_APPEAR_SIG: {
		APP_DBG_SIG("FS_GAME_BOSS_APPEAR_SIG\n");
		int currScore;
		memcpy(&currScore, get_data_common_msg(msg), sizeof(int));
		FsBoss::BossInfo bossInfo = {
			.level = fs_game_calculate_level(currScore),
			.score = FS_BOSS_MAX_SCORE,
			.hp = 10 + (currScore / 2),
			.coordinate = {MAX_LCD_WIDTH, MAP_HEIGHT / 2 - BOSS_ICON_HEIGHT / 2},
			.missileBitmap = missle_icon,
			.bossBitmap = {boss_I, boss_II},
		};
		FsBoss *boss = new FsBoss(bossInfo);
		if (boss == NULL){
			APP_WRN("Create Boss failed !!\n");
			timer_set(FS_GAME_TASK_BOSS_ID, FS_GAME_BOSS_APPEAR_SIG, AC_GAME_BOSS_RETRY_APPEAR, TIMER_ONE_SHOT);
			break;
		}
		ObjectEntry objEntry;
		objEntry.type =	ObjectType::Boss;
		objEntry.obj =	boss;
		task_post_common_msg(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_ACTIVE_OBJECT, (uint8_t*)&objEntry, sizeof(FsGame::ObjectEntry));
		timer_remove_attr(FS_GAME_TASK_OBSTACLE_ID, FS_GAME_OBSTACLE_PUSH_SIG);
		break;
	}

	default:
		APP_DBG_SIG("Unhandled signal: %d\n", msg->sig);
		break;
	}
}
