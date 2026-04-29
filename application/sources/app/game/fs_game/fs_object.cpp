#include "fs_object.h"

#include "app_dbg.h"
#include "fs_bitmap.h"
#include "fs_config.h"

using namespace FsGame;

#define FS_OBJECT_OUT_RANGE 1
#define FS_VERIFY_COOR(coor)                                       \
    (((coor).x < 0 || (coor).x >= MAX_LCD_WIDTH || (coor).y < 0 || \
      (coor).y >= MAX_LCD_HEIGHT)                                  \
         ? FS_OBJECT_OUT_RANGE                                     \
         : 0)
/*******************************
 * Object class implementation
 *******************************/
FsObject::FsObject(ObjectInfo objInfo) : mInfo(objInfo) { mSpeed = Slow; }

FsObject::~FsObject() {}

Coordinate FsObject::getCoordinate() const { return this->mInfo.coordinate; }

void FsObject::setVisible(Visibility visible) { this->mInfo.visible = visible; }

Visibility FsObject::isVisible() const { return this->mInfo.visible; }

int FsObject::setDir(Direction dir) {
    if (dir < LeftToRight || dir > DownToUp) {
        return -1;
    }
    mInfo.dir = dir;
    return 0;
}

Direction FsObject::getDir() const { return mInfo.dir; }

int FsObject::move(Coordinate newCoordinate) {
    if (newCoordinate.x > MAX_LCD_WIDTH || newCoordinate.x < 0 ||
        newCoordinate.y > MAX_LCD_HEIGHT || newCoordinate.y < 0) {
        mInfo.visible = Invisible;
        APP_PRINT("Out of range Screen");
        return 0;
    }
    this->mInfo.coordinate = newCoordinate;
    APP_DBG("Update coordinate {%d, %d}", this->mInfo.coordinate.x,
            this->mInfo.coordinate.y);
    return 0;
}

int FsObject::move() {
    if (mInfo.visible != Visible) {
        return -1;
    }
    if (mInfo.coordinate.x > MAX_LCD_WIDTH || mInfo.coordinate.x < 0 ||
        mInfo.coordinate.y > MAX_LCD_HEIGHT || mInfo.coordinate.y < 0) {
        mInfo.visible = Invisible;
        APP_PRINT("Out of range Screen");
        return 0;
    }
    switch (mInfo.dir) {
        case LeftToRight:
            mInfo.coordinate.x += mSpeed;
            break;
        case RightToLeft:
            mInfo.coordinate.x -= mSpeed;
            break;
        case UpToDown:
            mInfo.coordinate.y += mSpeed;
            break;
        case DownToUp:
            mInfo.coordinate.y -= mSpeed;
            break;
        default:
            return -1;
    }
    return 0;
}

int FsObject::changeCharacter(const unsigned char* newBitmap) {
    if (newBitmap == NULL) {
        return -1;
    }
    this->mInfo.bitmap = newBitmap;
    return 0;
}

int FsObject::setSpeed(Speed speed) {
    if (speed > Fast || speed < Slow) {
        return -1;
    }
    this->mSpeed = speed;
    return 0;
}

Speed FsObject::getSpeed() const { return mSpeed; }

int FsObject::render() { return 0; }

/*******************************
 * Obstacle class implementation
 *******************************/

FsObstacle::FsObstacle(const ObstacleInfo newObstacle[])
    : FsObject({nullptr, newObstacle[0].coordinate, Visibility::Visible,
                Direction::LeftToRight}),
      type(newObstacle[0].type),
      score(newObstacle[0].score) {}

/*******************************
 * Plane class implementation
 *******************************/

/*******************************
 * Screen class implementation
 *******************************/

FsScreen::FsScreen(RenderFunc renderFunc) : renderFunc(renderFunc) {
    listObject.clear();
}

FsScreen::~FsScreen() {
    renderFunc = nullptr;
    listObject.clear();
    listObject.shrink_to_fit();
}

int FsScreen::getObject(vector<FsObject*> &obj, ObjectType type) {
    for (auto it = listObject.begin(); it != listObject.end(); ++it) {
        ObjectEntry entry = *it;
        if (entry.type == type) {
            obj.push_back(entry.obj);
        }
    }
    if (obj.size() > 0) {
        return 0;
    }
    return -1;
}

int FsScreen::addObject(ObjectEntry objectInfo) {
    listObject.push_back(objectInfo);
    return 0;
}

// int FsScreen::removeObject(FsObject obj) { // TODO:
//     for (auto it = listObject.begin(); it != listObject.end(); ++it) {
//         if (it->obj == obj) {
//             listObject.erase(it);
//             return 0;
//         }
//     }
//     return -1;
// }

int FsScreen::render() {
    for (auto it = listObject.begin(); it != listObject.end();) {
        ObjectEntry object = *it;
        if (object.obj->isVisible() == Visibility::Visible) {
            Speed speed = object.obj->getSpeed();
            uint8_t w = 0, h = 0;
            switch (object.type) {
                case ObjectType::Plane:
                    w = PLANE_ICON_WIDTH;
                    h = PLANE_ICON_HEIGHT;
                    break;
                case ObjectType::Missile:
                    w = MISSLE_ICON_WIDTH;
                    h = MISSLE_ICON_HEIGHT;

                    break;
                case ObjectType::Explosion:
                    w = EXPLOSION_ICON_WIDTH;
                    h = EXPLOSION_ICON_HEIGHT;
                    break;
                case ObjectType::Obstacle:
                    w = MINE_ICON_WIDTH;
                    h = MINE_ICON_HEIGHT;
                    break;
                case ObjectType::Boss:
                    // TODO: set width and height for boss
                    break;
                default:
                    break;
            }

            if (w <= 0 || h <= 0) {
                ++it;
                continue;
            }
            if (object.obj->getBitmap() != NULL && renderFunc != NULL) {
                object.obj->move();
                Coordinate coor = object.obj->getCoordinate();
                if (FS_VERIFY_COOR(coor) == FS_OBJECT_OUT_RANGE) {
                    delete object.obj;
                    it = listObject.erase(it);
                    continue;
                }
                renderFunc(coor.x, coor.y, object.obj->getBitmap(), w, h, 1);
            }
        }
        ++it;
    }
    return 0;
}
