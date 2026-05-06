#include "fs_config.h"

#include "fs_core.h"

#include "app_dbg.h"
#include "fs_bitmap.h"

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
        case ObjectType::Boss:
            *width = BOSS_WIDTH;
            *height = BOSS_HEIGHT;
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

FsObject::~FsObject() {
    mInfo.visible = Invisible;
}

Coordinate FsObject::getCoordinate() const { return this->mInfo.coordinate; }

void FsObject::setCoordinate(Coordinate coordinate) { this->mInfo.coordinate = coordinate; }

void FsObject::setVisible(Visibility visible) { this->mInfo.visible = visible; }

Visibility FsObject::isVisible() const { return this->mInfo.visible; }

int FsObject::setDir(Direction dir) {
    if (dir < 0 || dir > Direction::DirectionMax) {
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
        case RandomY:
            if (rand() % 2 == 0) {
                mInfo.coordinate.y += mSpeed;
            } else {
                mInfo.coordinate.y -= mSpeed;
            }
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
 * Plane class implementation
 *******************************/
FsPlane::FsPlane(const unsigned char* bitmap, Coordinate firstCoordinate, GameLevel level)
    : FsObject({bitmap, firstCoordinate, Visibility::Visible, Direction::UpToDown}) {
    mLevel = level;
    mFrame = 0;
};

int FsPlane::render() {
    if (isVisible() != Visibility::Visible) {
        return -1;
    }
    if (mFrame > 2) { // TODO:
        mFrame = 0;
    }
    if (mLevel == GameLevel::Easy) {
        if (mFrame == 2) {
            move();
        }
    }
    ++mFrame;
    return 0;
}

/*******************************
 * Missle class implementation
 *******************************/
FsMissile::FsMissile(const unsigned char* bitmap, Coordinate firstCoordinate, MissileOwner owner, Direction dir)
    : FsObject({bitmap, firstCoordinate, Visibility::Visible, dir}) {
    mOwner = owner;
};

int FsMissile::setMode(MissileType newType) {
    this->mType = newType;
    return 0;
}
int FsMissile::getMode() const {
    return mType;
}

MissileOwner FsMissile::getOwner() const {
    return mOwner;
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
 * Boss class implementation
 *******************************/
FsBoss::FsBoss(BossInfo bossInfo)
    : FsObject({bossInfo.bossBitmap[0], bossInfo.coordinate, Visibility::Visible, Direction::RightToLeft}),
      mScore(bossInfo.score),
      mHp(bossInfo.hp),
      mFrame(0),
      mMissileBitmap(bossInfo.missileBitmap) {
    if (bossInfo.bossBitmap[0] == NULL || bossInfo.missileBitmap == NULL) {
        APP_DBG("Invalid boss info, boss bitmap or missile bitmap is null\n");
        mBossBitmap[0] = NULL;
        mBossBitmap[1] = NULL;
    } else {
        for (int i = 0; i < FS_BOSS_MAX_BITMAP_FRAME; ++i) {
            mBossBitmap[i] = bossInfo.bossBitmap[i];
        }
    }
    
    mLevel = bossInfo.level;
    setSpeed(Slow);
}

FsBoss::~FsBoss() {
    setVisible(Invisible);
}

ObstacleType FsBoss::getType() const {
    return ObstacleType::BossObstacle;
}

uint8_t FsBoss::getScore() const {
    return mScore;
}

int FsBoss::getHp() const {
    return mHp;
}

int FsBoss::decreaseHp() {
    if (mHp > 0) {
        --mHp;
    }
    APP_DBG("-----> Boss decrease HP, current HP=%d\n", mHp);
    return 0;
}

bool FsBoss::needMissile(Coordinate *missle) {
    if (missle == NULL) {
        return false;
    }

    if (mBossAppear == false) {
        return false;
    }

    if (mHp <= 0) {
        return false;
    }

    if (mFrame == mLevel) {
        missle->x = getCoordinate().x;
        missle->y = getCoordinate().y + BOSS_HEIGHT / 2 - MISSLE_ICON_HEIGHT / 2;
        return true;
    }
    return false;
}

const unsigned char* FsBoss::getBossBitmap() const { 
    return mBossBitmap[mFrame]; 
}

const unsigned char* FsBoss::getMissileBitmap() const { 
    return mMissileBitmap; 
}

int FsBoss::render() {
    if (isVisible() != Visibility::Visible) {
        return -1;
    }
    
    // NOTE: coordidate + boss = max lcd width => Boss appear 
    Coordinate coor = getCoordinate();
    if (!mBossAppear) {
        if (coor.x + (BOSS_WIDTH * 2) >= MAX_LCD_WIDTH) {
            int ret = move();
            APP_DBG("Boss is moving => ret: %d\n", ret);
            return ret;
        }
        mBossAppear = true;
        setDir(RandomY);
    } else {
        move();
    }
    /**
     * Animate Boss
     */
    uint8_t idx = 0;
    if (mFrame >= mLevel) {
        mFrame = 0;
    }
    // TODO:
    if (mFrame % mLevel == 0) {
        idx = 1;
    } else {
        idx = 0;
    }
    const unsigned char* bossBitmap = mBossBitmap[idx];
    if (bossBitmap == NULL) {
        APP_DBG("Boss bitmap is null, skip render\n");
        return -1;
    }
    changeCharacter(bossBitmap);
    ++mFrame;    
    return 0;
}

/*******************************
 * TunnelWall class implementation
 *******************************/
FsTunnelWall::FsTunnelWall(const unsigned char* bitmap, int16_t xTop)
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
 * Core class implementation
 *******************************/

FsCore::FsCore(RenderFunc render, GameLevel level)
    : mRenderFunc(render),
      mCrashedPlane(nullptr),
      mPlaneCrashBlinkTick(0),
      mPlaneCrashBlinking(false),
      mTotalScore(0),
      mBossAppear(false) {
    mListObject.clear();
}

FsCore::~FsCore() {
    mRenderFunc = nullptr;
    mCrashedPlane = nullptr;
    mListObject.clear();
    mListObject.shrink_to_fit();
}

int FsCore::getObject(vector<FsObject*> &obj, ObjectType type) {
    for (auto it = mListObject.begin(); it != mListObject.end(); ++it) {
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

int FsCore::addObject(ObjectEntry objectInfo) {
    if (objectInfo.type == ObjectType::Missile && setupMissile(objectInfo.obj) != 0) {
        delete objectInfo.obj;
        return -1;
    }

    Coordinate coor = objectInfo.obj->getCoordinate();
    APP_DBG("Add object {%d, %d} - type: %s\n", coor.x, coor.y, getType(objectInfo.type));
    mListObject.push_back(objectInfo);
    return 0;
}

const char *FsCore::getType(ObjectType type) {
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

int FsCore::computePlaneCrash(FsObject* plane, FsObject* wall) {
    if (plane == NULL || wall == NULL || plane->getBitmap() == NULL ||
        wall->getBitmap() == NULL) {
        return CrashType::Error;
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
        return CrashType::NoCrash;
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

    return CrashType::NoCrash;
}

int FsCore::computePlaneCrash(FsObject* plane, FsObject* something, uint16_t w, uint16_t h) {
    if (plane == NULL || something == NULL || plane->getBitmap() == NULL ||
        something->getBitmap() == NULL ||
        plane->isVisible() != Visibility::Visible ||
        something->isVisible() != Visibility::Visible) {
        return CrashType::NoCrash;
    }

    Coordinate planeCoor = plane->getCoordinate();
    Coordinate obstacleCoor = something->getCoordinate();

    if (!_rect_overlap(planeCoor, PLANE_ICON_WIDTH, PLANE_ICON_HEIGHT,
                         obstacleCoor, w, h)) {
        return CrashType::NoCrash;
    }

    int16_t overlapLeft = std::max(planeCoor.x, obstacleCoor.x);
    int16_t overlapTop = std::max(planeCoor.y, obstacleCoor.y);
    int16_t overlapRight = std::min(planeCoor.x + PLANE_ICON_WIDTH,
                                    obstacleCoor.x + w);
    int16_t overlapBottom = std::min(planeCoor.y + PLANE_ICON_HEIGHT,
                                     obstacleCoor.y + h);

    for (int16_t screenY = overlapTop; screenY < overlapBottom; ++screenY) {
        for (int16_t screenX = overlapLeft; screenX < overlapRight; ++screenX) {
            int16_t planeX = screenX - planeCoor.x;
            int16_t planeY = screenY - planeCoor.y;
            int16_t obstacleX = screenX - obstacleCoor.x;
            int16_t obstacleY = screenY - obstacleCoor.y;

            bool planePixelOn = FS_BITMAP_PIXEL_ON(plane->getBitmap(), PLANE_ICON_WIDTH, planeX, planeY);
            bool obstaclePixelOn = FS_BITMAP_PIXEL_ON(something->getBitmap(), w, obstacleX, obstacleY);

            if (planePixelOn && obstaclePixelOn) {
                return CrashType::PlaneCrash;
            }
        }
    }

    return CrashType::NoCrash;
}

int FsCore::computeMissileCrash(FsObject* missile, FsObject* something, uint16_t w, uint16_t h) {
    if (missile == NULL || something == NULL ||
        missile->isVisible() != Visibility::Visible ||
        something->isVisible() != Visibility::Visible) {
        return CrashType::NoCrash;
    }

    Coordinate missileCoor = missile->getCoordinate();
    Coordinate somethingCoor = something->getCoordinate();
    ObstacleType obsType = something->getType();
    
    if (!_rect_overlap(missileCoor, MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT, somethingCoor, w, h)) {
        return CrashType::NoCrash;
    }

    Coordinate explosionCoor = {
        static_cast<int16_t>(somethingCoor.x - 1),
        static_cast<int16_t>(somethingCoor.y - 1),
    };
    ObjectEntry explosionEntry;
    explosionEntry.type = ObjectType::Explosion;
    explosionEntry.obj = new FsExplosion(explosionCoor);
    if (explosionEntry.obj != NULL) {
        mListObject.push_back(explosionEntry);
    }
    else {
        APP_WRN("Create Explosion failed !!\n");
    }

    missile->setVisible(Visibility::Invisible);
    something->setVisible(Visibility::Invisible);

    if (obsType == ObstacleType::BossObstacle) {
        FsBoss *boss = (FsBoss *)something;
        // APP_DBG("Boss Coor {%d, %d}, Missle Coor {%d, %d}\n", somethingCoor.x, somethingCoor.y, missileCoor.x, missileCoor.y);
        boss->decreaseHp();
        something->setVisible(Visibility::Visible); // NOTE: need Boss disapear
        if (boss->getHp() > 0) {
            return CrashType::NoCrash;
        }
    }
    
    APP_DBG("Missile crash obstacle: missile {%d, %d}, something {%d, %d}\n", missileCoor.x, missileCoor.y, somethingCoor.x, somethingCoor.y);
    
    mTotalScore += something->getScore();
    switch (obsType)
    {
    case ObstacleType::Boom:
        return CrashType::BoomCrash;
    case ObstacleType::MineI:
        return CrashType::MineICrash;
    case ObstacleType::MineII:
        return CrashType::MineIICrash;
    case ObstacleType::BossObstacle:
        return CrashType::BossCrash;
    default:
        break;
    }
    APP_DBG("Unknown something type: %d\n", something->getType());
    return CrashType::Error;
}

int FsCore::setupMissile(FsObject* missile) {
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

void FsCore::beginPlaneCrash(FsObject* plane) {
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

CrashType FsCore::renderPlaneCrashBlink() {
    bool showPlane = (mPlaneCrashBlinkTick % 2) == 0;

    for (auto it = mListObject.begin(); it != mListObject.end(); ++it) {
        ObjectEntry object = *it;
        uint8_t w = 0, h = 0;

        if (!_get_object_size(object.type, &w, &h) ||
            object.obj == NULL ||
            object.obj->getBitmap() == NULL ||
            mRenderFunc == NULL) {
            continue;
        }

        if (object.obj == mCrashedPlane && !showPlane) {
            continue;
        }

        Coordinate coor = object.obj->getCoordinate();
        mRenderFunc(coor.x, coor.y, object.obj->getBitmap(), w, h, 1);
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

CrashType FsCore::calculateCrash() {
    /**
     * Plane: Tunnel Wall, Obstacle (bom, min I, mine II), missle when Boss appear
     * Missle: Obstacle (bom, min I, mine II)
     *  - Plane Missle
     *  - Boss Missle
     */
    std::vector<FsObject*> vPlane;
    std::vector<FsObject*> vWall;
    std::vector<FsObject*> vMissile;
    std::vector<FsObject*> vObstacle;
    std::vector<FsObject*> vMissleBoss;
    std::vector<FsObject*> vBoss;

    if (getMissle(vMissile, MissileOwner::PlaneOwner) == 0 &&
        (getObject(vObstacle, ObjectType::Obstacle) == 0 || getObject(vBoss, ObjectType::Boss) == 0)) {
        for (auto missile : vMissile) {
            for (auto obstacle : vObstacle) {
                computeMissileCrash(missile, obstacle, MINE_ICON_WIDTH, MINE_ICON_HEIGHT);
            }

            for (auto boss : vBoss) {
                computeMissileCrash(missile, boss, BOSS_WIDTH, BOSS_HEIGHT);
            }
        }
    }

    if (getObject(vPlane, ObjectType::Plane) != 0) {
        return CrashType::NoCrash;
    }

    for (auto plane : vPlane) {
        if (plane->isVisible() != Visibility::Visible) {
            continue;
        }

        if (getObject(vWall, ObjectType::TunnelWall) == 0) {
            for (auto wall : vWall) {
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

        if (getObject(vObstacle, ObjectType::Obstacle) == 0) {
            for (auto obstacle : vObstacle) {
                if (computePlaneCrash(plane, obstacle, MINE_ICON_WIDTH, MINE_ICON_HEIGHT) == CrashType::PlaneCrash) {
                    Coordinate planeCoor = plane->getCoordinate();
                    Coordinate obstacleCoor = obstacle->getCoordinate();

                    APP_DBG("Plane crash obstacle: plane {%d, %d}, obstacle {%d, %d}\n", planeCoor.x, planeCoor.y, obstacleCoor.x, obstacleCoor.y);
                    beginPlaneCrash(plane);
                    return CrashType::NoCrash;
                }
            }
        }

        if (getMissle(vMissleBoss, MissileOwner::BossOwner) == 0)  {
            for (auto missleBoss : vMissleBoss) {
                if (computePlaneCrash(plane, missleBoss, MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT) == CrashType::PlaneCrash) {
                    Coordinate planeCoor = plane->getCoordinate();
                    Coordinate missleCoor= missleBoss->getCoordinate();

                    APP_DBG("Plane crash missle: plane {%d, %d}, missle's boss {%d, %d}\n", planeCoor.x, planeCoor.y, missleCoor.x, missleCoor.y);
                    beginPlaneCrash(plane);
                    return CrashType::NoCrash;
                }
            }
        }
    }

    return CrashType::NoCrash;
}

CrashType FsCore::render() {
    if (mPlaneCrashBlinking) {
        return renderPlaneCrashBlink();
    }

    if (mBossAppear) {
        bossBattle();
    }

    for (auto it = mListObject.begin(); it != mListObject.end();) {
        ObjectEntry object = *it;
        uint8_t w = 0, h = 0;

        if (!_get_object_size(object.type, &w, &h)) {
            ++it;
            continue;
        }
        if (object.obj->getBitmap() != NULL && mRenderFunc != NULL) {
            if (object.type == ObjectType::TunnelWall) {
                object.obj->updateWall();
            } else if (object.type == ObjectType::Explosion 
                || object.type == ObjectType::Boss
                || object.type == ObjectType::Plane) {
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
                    it = mListObject.erase(it);
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
                APP_DBG("Delete Object: %s {%d, %d} - Remain Object: %d\n", getType(object.type), coor.x, coor.y, mListObject.size() - 1);
                delete object.obj;
                it = mListObject.erase(it);
                continue;
            }
            if (object.type != ObjectType::TunnelWall && FS_VERIFY_COOR(coor) == FS_OBJECT_OUT_RANGE) {
                if (object.type == ObjectType::Plane) {
                    APP_DBG("Plane crash out of range\n");
                    beginPlaneCrash(object.obj);
                    return CrashType::NoCrash;
                }
                APP_DBG("Delete Object: %s {%d, %d} - Remain Object: %d\n", getType(object.type), coor.x, coor.y, mListObject.size() - 1);
                delete object.obj;
                it = mListObject.erase(it);
                continue;
            }
            mRenderFunc(coor.x, coor.y, object.obj->getBitmap(), w, h, 1);
        }
        
        ++it;
    }
    return calculateCrash();
}

int FsCore::getScore() const {
    return mTotalScore;
}

bool FsCore::needBossAppear() {
    if (mTotalScore >= FS_BOSS_APPEAR_SCORE
        && mBossAppear == false) {
        mBossAppear = true;
        clearObstacle();
        return mBossAppear;
    }
    return false;
}

int FsCore::clearObstacle() {
    for (auto it = mListObject.begin(); it != mListObject.end();) {
        ObjectEntry object = *it;
        if (object.type == ObjectType::Obstacle) {
            APP_DBG("Delete Obstacle: %s\n", getType(object.type));
            delete object.obj;
            it = mListObject.erase(it);
        } else {
            ++it;
        }
    }
    return 0;
}

int FsCore::getMissle(std::vector<FsObject*>& missles, MissileOwner owner) {
    for (auto it = mListObject.begin(); it != mListObject.end(); ++it) {
        ObjectEntry entry = *it;
        if (entry.type == ObjectType::Missile) {
            FsMissile* missile = (FsMissile*)(entry.obj);
            if (missile != NULL && missile->getOwner() == owner) {
                missles.push_back(entry.obj);
            }
        }
    }
    if (missles.size() > 0) {
        return 0;
    }
    return -1;
}

int FsCore::bossBattle() {
    // NOTE: BOSS auto generate missle 
    std::vector<FsObject*> bosses;
    if (getObject(bosses, ObjectType::Boss) == 0) {
        for (auto boss : bosses) {
            if (boss->isVisible() != Visibility::Visible) {
                continue;
            }

            FsBoss* fsBoss = (FsBoss*)(boss);
            if (fsBoss == NULL) {
                continue;
            }
            if (fsBoss->getHp() <= 0) {
                fsBoss->setDir(Direction::LeftToRight); // NOTE: disapear Boss
                continue;
            }
            Coordinate bossCoor = boss->getCoordinate();
            if (fsBoss->needMissile(&bossCoor) == true) {
                ObjectEntry missileEntry;
                missileEntry.type = ObjectType::Missile;
                missileEntry.obj = new FsMissile(fsBoss->getMissileBitmap(), bossCoor, MissileOwner::BossOwner, Direction::RightToLeft);
                if (missileEntry.obj == NULL) {
                    APP_WRN("Create Missle for boss failed !!\n");
                    continue;
                }
                mListObject.push_back(missileEntry);
                // APP_DBG("Boss launch missile: boss {%d, %d}, missile {%d, %d}\n", bossCoor.x, bossCoor.y, bossCoor.x, bossCoor.y);
            }
        }
    }

    std::vector<FsObject*> misslesPlane;
    std::vector<FsObject*> misslesBoss;
    if (getMissle(misslesPlane, MissileOwner::PlaneOwner) != 0 
        || getMissle(misslesBoss, MissileOwner::BossOwner) != 0) {
        // NOTE: Missle's Boss or Missle's Plane dont available
        return 0;
    }


    for (auto misslePlane : misslesPlane) {
        Coordinate misslePlaneCoor = misslePlane->getCoordinate();
        for (auto missleBoss : misslesBoss) {
            Coordinate missleBossCoor = missleBoss->getCoordinate();
            if (_rect_overlap(misslePlaneCoor, MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT,
                                missleBossCoor, MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT)) {
                APP_DBG("Crash Missile: plane {%d, %d}, boss {%d, %d}\n", 
                    misslePlaneCoor.x, misslePlaneCoor.y, missleBossCoor.x, missleBossCoor.y);
                
                Coordinate explosionCoor = {
                    static_cast<int16_t>(missleBossCoor.x - 1),
                    static_cast<int16_t>(missleBossCoor.y - 1),
                };
                ObjectEntry explosionEntry;
                explosionEntry.type = ObjectType::Explosion;
                explosionEntry.obj = new FsExplosion(explosionCoor);
                if (explosionEntry.obj == NULL) {
                    APP_WRN("Create Explosion failed !!\n");
                    misslePlane->setVisible(Visibility::Invisible);
                    missleBoss->setVisible(Visibility::Invisible);
                    continue;
                }
                mListObject.push_back(explosionEntry);

                misslePlane->setVisible(Visibility::Invisible);
                missleBoss->setVisible(Visibility::Invisible);
            }
            
        }
    }

    
    return 0;
}