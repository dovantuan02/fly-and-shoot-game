#include "fs_object.h"

#include "app_dbg.h"
#include "fs_bitmap.h"
#include "fs_config.h"

using namespace FsGame;

#define FS_OBJECT_OUT_RANGE 1

#define FS_EXPLOSION_LIFE_TIME 6
#define FS_PLANE_CRASH_BLINK_COUNT 5
#define FS_PLANE_CRASH_BLINK_TICKS (FS_PLANE_CRASH_BLINK_COUNT * 2)

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

static bool _rect_overlap(Coordinate firstCoor, int16_t firstWidth,
                            int16_t firstHeight, Coordinate secondCoor,
                            int16_t secondWidth, int16_t secondHeight) {
    return firstCoor.x < secondCoor.x + secondWidth &&
           firstCoor.x + firstWidth > secondCoor.x &&
           firstCoor.y < secondCoor.y + secondHeight &&
           firstCoor.y + firstHeight > secondCoor.y;
}

static bool _get_object_size(ObjectType type, uint8_t* width, uint8_t* height) {
    switch (type) {
        case ObjectType::Plane:
            *width = PLANE_ICON_WIDTH;
            *height = PLANE_ICON_HEIGHT;
            return true;
        case ObjectType::Missile:
            *width = MISSLE_ICON_WIDTH;
            *height = MISSLE_ICON_HEIGHT;
            return true;
        case ObjectType::Explosion:
            *width = EXPLOSION_ICON_WIDTH;
            *height = EXPLOSION_ICON_HEIGHT;
            return true;
        case ObjectType::Obstacle:
            *width = MINE_ICON_WIDTH;
            *height = MINE_ICON_HEIGHT;
            return true;
        case ObjectType::TunnelWall:
            *width = MAP_WIDTH;
            *height = MAP_HEIGHT;
            return true;
        default:
            *width = 0;
            *height = 0;
            return false;
    }
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
    if (mInfo.visible != Visible) {
        APP_DBG("Object is not visible, cannot move\n");
        return -1;
    }
    this->mInfo.coordinate = newCoordinate;
    APP_DBG("Update coordinate {%d, %d}\n", mInfo.coordinate.x, mInfo.coordinate.y);
    if (this->mInfo.coordinate.x > MAX_LCD_WIDTH || this->mInfo.coordinate.x < 0 ||
        this->mInfo.coordinate.y > MAX_LCD_HEIGHT || this->mInfo.coordinate.y < 0) {
        mInfo.visible = Invisible;
        APP_DBG("Object move out of range, set invisible\n");
        return -1;
    }
    return 0;
}

int FsObject::move() {
    if (mInfo.visible != Visible) {
        return -1;
    }
    if (mInfo.coordinate.x > MAX_LCD_WIDTH || mInfo.coordinate.x < 0 ||
        mInfo.coordinate.y > MAX_LCD_HEIGHT || mInfo.coordinate.y < 0) {
        mInfo.visible = Invisible;
        return -2;
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
            return -3;
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

/*******************************
 * Obstacle class implementation
 *******************************/

FsObstacle::FsObstacle(const unsigned char* bitmap, ObstacleInfo newObstacle) 
    : FsObject ({bitmap, newObstacle.coordinate, Visibility::Visible, Direction::RightToLeft}) {
    mType = newObstacle.type;
    mScore = newObstacle.score;
}

ObstacleType FsObstacle::getType() const {
    return mType;
}

uint8_t FsObstacle::getScore() const {
    return mScore;
}

/*******************************
 * TunnelWall class implementation
 *******************************/
FsTunnelWall::FsTunnelWall(const unsigned char* bitmap, int16_t xTop, int16_t xBot)
    : FsObject ({bitmap, Coordinate{xTop, 0}, Visibility::Visible, Direction::RightToLeft}) {
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
 * Explosion class implementation
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

    const unsigned char* explosionBitmap =  (mFrame % 2 == 0) ? explosion_I_icon : explosion_II_icon;

    changeCharacter(explosionBitmap);
    ++mFrame;
    --mLifeTime;
    return 0;
}

/*******************************
 * Screen class implementation
 *******************************/

FsScreen::FsScreen(RenderFunc renderFunc)
    : renderFunc(renderFunc),
      mCrashedPlane(nullptr),
      mPlaneCrashBlinkTick(0),
      mPlaneCrashBlinking(false) {
    listObject.clear();
}

FsScreen::~FsScreen() {
    renderFunc = nullptr;
    mCrashedPlane = nullptr;
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
                return CrashType::PlaneCrash;
            }
        }
    }

    return 0;
}

int FsScreen::computePlaneObstacleCrash(FsObject* plane, FsObject* obstacle) {
    if (plane == NULL || obstacle == NULL || plane->getBitmap() == NULL ||
        obstacle->getBitmap() == NULL ||
        plane->isVisible() != Visibility::Visible ||
        obstacle->isVisible() != Visibility::Visible) {
        return CrashType::NoCrash;
    }

    Coordinate planeCoor = plane->getCoordinate();
    Coordinate obstacleCoor = obstacle->getCoordinate();

    if (!_rect_overlap(planeCoor, PLANE_ICON_WIDTH, PLANE_ICON_HEIGHT,
                         obstacleCoor, MINE_ICON_WIDTH, MINE_ICON_HEIGHT)) {
        return CrashType::NoCrash;
    }

    int16_t overlapLeft = std::max(planeCoor.x, obstacleCoor.x);
    int16_t overlapTop = std::max(planeCoor.y, obstacleCoor.y);
    int16_t overlapRight = std::min(planeCoor.x + PLANE_ICON_WIDTH,
                                    obstacleCoor.x + MINE_ICON_WIDTH);
    int16_t overlapBottom = std::min(planeCoor.y + PLANE_ICON_HEIGHT,
                                     obstacleCoor.y + MINE_ICON_HEIGHT);

    for (int16_t screenY = overlapTop; screenY < overlapBottom; ++screenY) {
        for (int16_t screenX = overlapLeft; screenX < overlapRight; ++screenX) {
            int16_t planeX = screenX - planeCoor.x;
            int16_t planeY = screenY - planeCoor.y;
            int16_t obstacleX = screenX - obstacleCoor.x;
            int16_t obstacleY = screenY - obstacleCoor.y;

            bool planePixelOn = FS_BITMAP_PIXEL_ON(plane->getBitmap(), PLANE_ICON_WIDTH, planeX, planeY);
            bool obstaclePixelOn = FS_BITMAP_PIXEL_ON(obstacle->getBitmap(), MINE_ICON_WIDTH, obstacleX, obstacleY);

            if (planePixelOn && obstaclePixelOn) {
                return CrashType::PlaneCrash;
            }
        }
    }

    return CrashType::NoCrash;
}

int FsScreen::computeMissileCrash(FsObject* missile, FsObject* obstacle) {
    if (missile == NULL || obstacle == NULL ||
        missile->isVisible() != Visibility::Visible ||
        obstacle->isVisible() != Visibility::Visible) {
        return CrashType::NoCrash;
    }

    Coordinate missileCoor = missile->getCoordinate();
    Coordinate obstacleCoor = obstacle->getCoordinate();

    if (!_rect_overlap(missileCoor, MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT, obstacleCoor, MINE_ICON_WIDTH, MINE_ICON_HEIGHT)) {
        return CrashType::NoCrash;
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
    
    mTotalScore += obstacle->getScore();
    switch (obstacle->getType())
    {
    case ObstacleType::Boom:
        return CrashType::BoomCrash;
    case ObstacleType::MineI:
        return CrashType::MineICrash;
    case ObstacleType::MineII:
        return CrashType::MineIICrash;
    default:
        break;
    }
    APP_DBG("Unknown obstacle type: %d\n", obstacle->getType());
    return CrashType::NoCrash;
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

    if (planes.size() == 0) {
        return -1;
    }

    if (walls.size() == 0) {
        return -1;
    }

    if (planes[0]->isVisible() != Visibility::Visible) {
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

void FsScreen::beginPlaneCrash(FsObject* plane) {
    if (plane == NULL || mPlaneCrashBlinking) {
        return;
    }
    plane->setVisible(Visibility::Invisible);
    Coordinate planeCoor = plane->getCoordinate();
    if (planeCoor.x < 0) {
        planeCoor.x = 0;
    } else if (planeCoor.x > MAX_LCD_WIDTH - PLANE_ICON_WIDTH) {
        planeCoor.x = MAX_LCD_WIDTH - PLANE_ICON_WIDTH;
    }

    if (planeCoor.y < 0) {
        planeCoor.y = 0;
    } else if (planeCoor.y > MAX_LCD_HEIGHT - PLANE_ICON_HEIGHT) {
        planeCoor.y = MAX_LCD_HEIGHT - PLANE_ICON_HEIGHT;
    }
    mCrashedPlane = plane;
    mPlaneCrashBlinkTick = 0;
    mPlaneCrashBlinking = true;
    mCrashedPlane->move(planeCoor);
}

CrashType FsScreen::renderPlaneCrashBlink() {
    bool showPlane = (mPlaneCrashBlinkTick % 2) == 0;

    for (auto it = listObject.begin(); it != listObject.end(); ++it) {
        ObjectEntry object = *it;
        uint8_t w = 0, h = 0;

        if (!_get_object_size(object.type, &w, &h) ||
            object.obj == NULL ||
            object.obj->getBitmap() == NULL ||
            renderFunc == NULL) {
            continue;
        }

        if (object.obj == mCrashedPlane && !showPlane) {
            continue;
        }

        Coordinate coor = object.obj->getCoordinate();
        renderFunc(coor.x, coor.y, object.obj->getBitmap(), w, h, 1);
    }

    ++mPlaneCrashBlinkTick;
    if (mPlaneCrashBlinkTick >= FS_PLANE_CRASH_BLINK_TICKS) {
        mPlaneCrashBlinking = false;
        if (mCrashedPlane != NULL) {
            mCrashedPlane->setVisible(Visibility::Invisible);
        }
        return CrashType::PlaneCrash;
    }

    return CrashType::NoCrash;
}

CrashType FsScreen::calculateCrash() {
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
                computeMissileCrash(missile, obstacle);
            }
        }
    }

    if (getObject(planes, ObjectType::Plane) != 0) {
        return CrashType::NoCrash;
    }

    for (auto plane : planes) {
        if (plane->isVisible() != Visibility::Visible) {
            continue;
        }

        if (getObject(walls, ObjectType::TunnelWall) == 0) {
            for (auto wall : walls) {
                if (wall->isVisible() != Visibility::Visible) {
                    continue;
                }

                if (computePlaneCrash(plane, wall) == CrashType::PlaneCrash) {
                    Coordinate planeCoor = plane->getCoordinate();
                    Coordinate wallCoor = wall->getCoordinate();

                    APP_DBG("Plane crash tunnel wall: plane {%d, %d}, wall {%d, %d}\n", planeCoor.x, planeCoor.y, wallCoor.x, wallCoor.y);
                    beginPlaneCrash(plane);
                    return CrashType::NoCrash;
                }
            }
        }

        if (getObject(obstacles, ObjectType::Obstacle) == 0) {
            for (auto obstacle : obstacles) {
                if (computePlaneObstacleCrash(plane, obstacle) == CrashType::PlaneCrash) {
                    Coordinate planeCoor = plane->getCoordinate();
                    Coordinate obstacleCoor = obstacle->getCoordinate();

                    APP_DBG("Plane crash obstacle: plane {%d, %d}, obstacle {%d, %d}\n", planeCoor.x, planeCoor.y, obstacleCoor.x, obstacleCoor.y);
                    beginPlaneCrash(plane);
                    return CrashType::NoCrash;
                }
            }
        }
    }

    return CrashType::NoCrash;
}

CrashType FsScreen::render() {
    if (mPlaneCrashBlinking) {
        return renderPlaneCrashBlink();
    }

    for (auto it = listObject.begin(); it != listObject.end();) {
        ObjectEntry object = *it;
        uint8_t w = 0, h = 0;

        if (!_get_object_size(object.type, &w, &h)) {
            ++it;
            continue;
        }
        if (object.obj->getBitmap() != NULL && renderFunc != NULL) {
            if (object.type == ObjectType::TunnelWall) {
                object.obj->updateWall();
            } else if (object.type == ObjectType::Explosion) {
                object.obj->render();
            } else {
                if (object.obj->move() != 0) {
                    if (object.type == ObjectType::Plane) {
                        APP_DBG("Plane crash out of range\n");
                        beginPlaneCrash(object.obj);
                        return CrashType::NoCrash;
                    }
                    APP_DBG("Delete Object: %s\n", getType(object.type));
                    delete object.obj;
                    it = listObject.erase(it);
                    continue;
                }

            }
            Coordinate coor = object.obj->getCoordinate();
            if (object.obj->isVisible() != Visibility::Visible) {
                if (object.type == ObjectType::Plane) {
                    APP_DBG("Plane crash out of range\n");
                    beginPlaneCrash(object.obj);
                    return CrashType::NoCrash;
                }
                APP_DBG("Delete Object: %s {%d, %d} - Remain Object: %d\n", getType(object.type), coor.x, coor.y, listObject.size() - 1);
                delete object.obj;
                it = listObject.erase(it);
                continue;
            }
            if (object.type != ObjectType::TunnelWall && FS_VERIFY_COOR(coor) == FS_OBJECT_OUT_RANGE) {
                if (object.type == ObjectType::Plane) {
                    APP_DBG("Plane crash out of range\n");
                    beginPlaneCrash(object.obj);
                    return CrashType::NoCrash;
                }
                APP_DBG("Delete Object: %s {%d, %d} - Remain Object: %d\n", getType(object.type), coor.x, coor.y, listObject.size() - 1);
                delete object.obj;
                it = listObject.erase(it);
                continue;
            }
            renderFunc(coor.x, coor.y, object.obj->getBitmap(), w, h, 1);
        }
        
        ++it;
    }
    return calculateCrash();
}

int FsScreen::getScore() const {
    return mTotalScore;
}