#include "fs_object.h"

#include "app_dbg.h"
#include "fs_bitmap.h"
#include "fs_config.h"

using namespace FsGame;

#define FS_OBJECT_OUT_RANGE 1
#define FS_OBJECT_CRASH 1
#define FS_EXPLOSION_LIFE_TIME 6

#define FS_VERIFY_COOR(coor)                                       \
    (((coor).x < 0 || (coor).x >= MAX_LCD_WIDTH || (coor).y < 0 || \
      (coor).y >= MAX_LCD_HEIGHT)                                  \
         ? FS_OBJECT_OUT_RANGE                                     \
         : 0)

#define FS_BITMAP_PIXEL_ON(bitmap, width, x, y)                       \
    (((bitmap) != NULL && (x) >= 0 && (y) >= 0)                       \
         ? (pgm_read_byte((bitmap) + (y) * (((width) + 7) / 8) +      \
                          (x) / 8) &                                  \
            (128 >> ((x) & 7)))                                       \
         : 0)

static bool fs_rect_overlap(Coordinate firstCoor, int16_t firstWidth,
                            int16_t firstHeight, Coordinate secondCoor,
                            int16_t secondWidth, int16_t secondHeight) {
    return firstCoor.x < secondCoor.x + secondWidth &&
           firstCoor.x + firstWidth > secondCoor.x &&
           firstCoor.y < secondCoor.y + secondHeight &&
           firstCoor.y + firstHeight > secondCoor.y;
}

/*******************************
 * Object class implementation
 *******************************/
FsObject::FsObject(ObjectInfo objInfo) : mInfo(objInfo) { mSpeed = Slow; }

FsObject::~FsObject() {}

Coordinate FsObject::getCoordinate() const { return this->mInfo.coordinate; }

void FsObject::setCoordinate(Coordinate coordinate) { this->mInfo.coordinate = coordinate; }

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

int FsObject::updateWall() {
    return 0;
}

int FsObject::render() { return 0; }

/*******************************
 * Obstacle class implementation
 *******************************/

FsObstacle::FsObstacle(const unsigned char* bitmap, ObstacleInfo newObstacle) 
    : FsObject ({bitmap, newObstacle.coordinate, Visibility::Visible, Direction::RightToLeft}) {
    mType = newObstacle.type;
    mScore = newObstacle.score;
}

static const unsigned char* fs_get_wall_bitmap(int16_t wallIndex) {
    return (wallIndex % 2 == 0) ? map_I : map_II;
}

FsTunnelWall::FsTunnelWall(int16_t xTop, int16_t xBot)
    : FsObject ({fs_get_wall_bitmap(xBot), Coordinate{xTop, 0}, Visibility::Visible, Direction::RightToLeft}) {
    setSpeed(Slow);
}

int FsTunnelWall::updateWall() {
    Coordinate coordinate = getCoordinate();
    coordinate.x -= getSpeed();

    if (coordinate.x <= -MAP_WIDTH) {
        coordinate.x += MAP_WIDTH * 2;
    }

    setCoordinate(coordinate);
    return 0;
}

/*******************************
 * Plane class implementation
 *******************************/

FsExplosion::FsExplosion(Coordinate firstCoordinate)
    : FsObject({explosion_I_icon, firstCoordinate, Visibility::Visible,
                Direction::RightToLeft}),
      mFrame(0),
      mLifeTime(FS_EXPLOSION_LIFE_TIME) {
    setSpeed(Medium);
}

FsExplosion::~FsExplosion() {
    setVisible(Invisible);
}

int FsExplosion::render() {
    if (isVisible() != Visibility::Visible) {
        return -1;
    }

    if (mLifeTime == 0) {
        setVisible(Invisible);
        return 0;
    }

    const unsigned char* explosionBitmap =
        (mFrame % 2 == 0) ? explosion_I_icon : explosion_II_icon;

    changeCharacter(explosionBitmap);
    ++mFrame;
    --mLifeTime;
    return 0;
}

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
    if (objectInfo.type == ObjectType::Missile && setupMissile(objectInfo.obj) != 0) {
        delete objectInfo.obj;
        return -1;
    }

    APP_DBG("Add object {%d, %d} - type: %d\n", objectInfo.obj->getCoordinate().x, objectInfo.obj->getCoordinate().y, objectInfo.type);
    listObject.push_back(objectInfo);
    return 0;
}

const char *FsScreen::getType(ObjectType type) {
    switch (type)
    {
    case ObjectType::Boss:
        return "Boss";
    case ObjectType::Plane:
        return "Plane";
    case ObjectType::Missile:
        return "Missile";
    case ObjectType::Explosion:
        return "Explosion";
    case ObjectType::Obstacle:
        return "Obstacle";
    case ObjectType::TunnelWall:
        return "TunnelWall";
    default:
        return NULL;
    }
}

int FsScreen::computePlaneCrash(FsObject* plane, FsObject* wall) {
    if (plane == NULL || wall == NULL || plane->getBitmap() == NULL ||
        wall->getBitmap() == NULL) {
        return 0;
    }

    Coordinate planeCoor = plane->getCoordinate();
    Coordinate wallCoor = wall->getCoordinate();
    const unsigned char* planeBitmap = plane->getBitmap();
    const unsigned char* wallBitmap = wall->getBitmap();

    int16_t overlapLeft = std::max(planeCoor.x, wallCoor.x);
    int16_t overlapTop = std::max(planeCoor.y, wallCoor.y);
    int16_t overlapRight = std::min(planeCoor.x + PLANE_ICON_WIDTH, wallCoor.x + MAP_WIDTH);
    int16_t overlapBottom = std::min(planeCoor.y + PLANE_ICON_HEIGHT, wallCoor.y + MAP_HEIGHT);

    if (overlapLeft >= overlapRight || overlapTop >= overlapBottom) {
        return 0;
    }

    for (int16_t screenY = overlapTop; screenY < overlapBottom; ++screenY) {
        for (int16_t screenX = overlapLeft; screenX < overlapRight; ++screenX) {
            int16_t planeX = screenX - planeCoor.x;
            int16_t planeY = screenY - planeCoor.y;
            int16_t wallX = screenX - wallCoor.x;
            int16_t wallY = screenY - wallCoor.y;

            bool planePixelOn = FS_BITMAP_PIXEL_ON(planeBitmap, PLANE_ICON_WIDTH, planeX, planeY);
            bool wallPixelOn = FS_BITMAP_PIXEL_ON(wallBitmap, MAP_WIDTH, wallX, wallY);

            if (planePixelOn && wallPixelOn) {
                return FS_OBJECT_CRASH;
            }
        }
    }

    return 0;
}

int FsScreen::computeMissileCrash(FsObject* missile, FsObject* obstacle) {
    if (missile == NULL || obstacle == NULL ||
        missile->isVisible() != Visibility::Visible ||
        obstacle->isVisible() != Visibility::Visible) {
        return 0;
    }

    Coordinate missileCoor = missile->getCoordinate();
    Coordinate obstacleCoor = obstacle->getCoordinate();

    if (!fs_rect_overlap(missileCoor, MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT,
                         obstacleCoor, MINE_ICON_WIDTH, MINE_ICON_HEIGHT)) {
        return 0;
    }

    Coordinate explosionCoor = {
        static_cast<int16_t>(obstacleCoor.x - 1),
        static_cast<int16_t>(obstacleCoor.y - 1),
    };
    ObjectEntry explosionEntry;
    explosionEntry.type = ObjectType::Explosion;
    explosionEntry.obj = new FsExplosion(explosionCoor);
    listObject.push_back(explosionEntry);

    missile->setVisible(Visibility::Invisible);
    obstacle->setVisible(Visibility::Invisible);
    APP_DBG("Missile crash obstacle: missile {%d, %d}, obstacle {%d, %d}\n", missileCoor.x, missileCoor.y, obstacleCoor.x, obstacleCoor.y);

    return FS_OBJECT_CRASH;
}

int FsScreen::setupMissile(FsObject* missile) {
    if (missile == NULL) {
        return -1;
    }

    std::vector<FsObject*> planes;
    std::vector<FsObject*> walls;

    if (getObject(planes, ObjectType::Plane) != 0 ||
        getObject(walls, ObjectType::TunnelWall) != 0) {
        return -1;
    }

    Coordinate planeCoor = planes[0]->getCoordinate();
    Coordinate missileCoor = {
        static_cast<int16_t>(planeCoor.x + PLANE_ICON_WIDTH),
        static_cast<int16_t>(planeCoor.y + (PLANE_ICON_HEIGHT - MISSLE_ICON_HEIGHT) / 2),
    };

    missile->move(missileCoor);

    for (auto wall : walls) {
        if (wall->isVisible() != Visibility::Visible ||
            wall->getBitmap() == NULL) {
            continue;
        }

        Coordinate wallCoor = wall->getCoordinate();
        int16_t overlapLeft = std::max(missileCoor.x, wallCoor.x);
        int16_t overlapTop = std::max(missileCoor.y, wallCoor.y);
        int16_t overlapRight = std::min(missileCoor.x + MISSLE_ICON_WIDTH, wallCoor.x + MAP_WIDTH);
        int16_t overlapBottom = std::min(missileCoor.y + MISSLE_ICON_HEIGHT, wallCoor.y + MAP_HEIGHT);

        if (overlapLeft >= overlapRight || overlapTop >= overlapBottom) {
            continue;
        }

        for (int16_t screenY = overlapTop; screenY < overlapBottom; ++screenY) {
            for (int16_t screenX = overlapLeft; screenX < overlapRight; ++screenX) {
                int16_t wallX = screenX - wallCoor.x;
                int16_t wallY = screenY - wallCoor.y;

                if (FS_BITMAP_PIXEL_ON(wall->getBitmap(), MAP_WIDTH, wallX, wallY)) {
                    missile->setVisible(Visibility::Invisible);
                    return -1;
                }
            }
        }
    }

    return 0;
}

int FsScreen::calculateCrash() {
    /**
     * Plane: Tunnel Wall, Obstacle (bom, min I, mine II), missle when Boss appear
     * Missle: Obstacle (bom, min I, mine II)
     *  - Plane Missle
     *  - Boss Missle
     */
    std::vector<FsObject*> planes;
    std::vector<FsObject*> walls;
    std::vector<FsObject*> missiles;
    std::vector<FsObject*> obstacles;

    if (getObject(missiles, ObjectType::Missile) == 0 &&
        getObject(obstacles, ObjectType::Obstacle) == 0) {
        for (auto missile : missiles) {
            for (auto obstacle : obstacles) {
                if (computeMissileCrash(missile, obstacle)) {
                    return FS_OBJECT_CRASH;
                }
            }
        }
    }

    if (getObject(planes, ObjectType::Plane) != 0 ||
        getObject(walls, ObjectType::TunnelWall) != 0) {
        return 0;
    }

    for (auto plane : planes) {
        if (plane->isVisible() != Visibility::Visible) {
            continue;
        }

        for (auto wall : walls) {
            if (wall->isVisible() != Visibility::Visible) {
                continue;
            }

            if (computePlaneCrash(plane, wall)) {
                Coordinate planeCoor = plane->getCoordinate();
                Coordinate wallCoor = wall->getCoordinate();

                APP_DBG("Plane crash tunnel wall: plane {%d, %d}, wall {%d, %d}\n", planeCoor.x, planeCoor.y, wallCoor.x, wallCoor.y);
                plane->setVisible(Visibility::Invisible);
                return FS_OBJECT_CRASH;
            }
        }
    }

    return 0;
}

int FsScreen::render() {
    for (auto it = listObject.begin(); it != listObject.end();) {
        ObjectEntry object = *it;
        if (object.obj->isVisible() == Visibility::Visible) {
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
                case ObjectType::TunnelWall:
                    w = MAP_WIDTH;
                    h = MAP_HEIGHT;
                    break;
                default:
                    break;
            }

            if (w <= 0 || h <= 0) {
                ++it;
                continue;
            }
            if (object.obj->getBitmap() != NULL && renderFunc != NULL) {
                if (object.type == ObjectType::TunnelWall) {
                    object.obj->updateWall();
                } else if (object.type == ObjectType::Explosion) {
                    object.obj->render();
                } else {
                    object.obj->move();
                }
                Coordinate coor = object.obj->getCoordinate();
                if (object.obj->isVisible() != Visibility::Visible) {
                    APP_DBG("Delete Object: %s {%d, %d} - Remain Object: %d\n", getType(object.type), coor.x, coor.y, listObject.size() - 1);
                    delete object.obj;
                    it = listObject.erase(it);
                    continue;
                }
                if (object.type != ObjectType::TunnelWall && FS_VERIFY_COOR(coor) == FS_OBJECT_OUT_RANGE) {
                    APP_DBG("Delete Object: %s {%d, %d} - Remain Object: %d\n", getType(object.type), coor.x, coor.y, listObject.size() - 1);
                    delete object.obj;
                    it = listObject.erase(it);
                    continue;
                }
                renderFunc(coor.x, coor.y, object.obj->getBitmap(), w, h, 1);
            }
        }
        ++it;
    }
    return calculateCrash();
}
