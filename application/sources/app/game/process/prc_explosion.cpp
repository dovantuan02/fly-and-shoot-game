#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

#include "prc_explosion.h"

vector<explosion_infor_t> v_explosion;
explosion_infor_t explosion = {0, 0, VER_I};
void explosion_reset() {
    if (!v_explosion.empty()) {
        v_explosion.clear();
        v_explosion.shrink_to_fit();
    }
}
void explosion_push() {
    v_explosion.push_back(explosion);
}

// ANIMATE EXPLSION AND ERASE
void explosion_show()
{
    if (!v_explosion.empty()) {
        for (size_t i = 0; i < v_explosion.size();i++) {
            if (v_explosion[i].ver == VER_I) {
                v_explosion[i].ver = VER_II;
            } else if (v_explosion[i].ver == VER_II) {
                v_explosion.erase(v_explosion.begin() + i);
            }
        }
    }
}
void task_prc_explosion_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SIG_EXPLOSION_RESET:
            explosion_reset();
            break;

        case SIG_EXPLOSION_PUSH:
            APP_DBG("EXPLOSION : X : %d , Y : %d\n", explosion.x, explosion.y );
            explosion_push();
            break;

        case SIG_EXPLOSION_UPDATE:
            explosion_show();
            break;

        case SIG_EXPLOSION_ON_TICK:
            task_post_pure_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_UPDATE);
            break;

        default:
            break;
    }
}