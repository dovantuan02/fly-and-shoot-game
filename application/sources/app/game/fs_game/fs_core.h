#ifndef FS_CORE_H
#define FS_CORE_H

#include <cstdint>
#include <vector>

// #define BLACK 0
// #define WHITE 1

namespace FsGame {

enum GameLevel { Easy = 1, Standard , Challenge };
enum Visibility { Visible, Invisible, Blink};
enum Direction { LeftToRight, RightToLeft, UpToDown, DownToUp , RandomX, RandomY, Random, DirectionMax };
enum MissileType { Normal, Split };
enum MissileOwner { PlaneOwner, BossOwner };
enum ObjectType { Plane, Missile, Explosion, Obstacle, Boss, TunnelWall };
enum Speed { Slow = 3, Medium, Fast };
enum ObstacleType { None, Boom, MineI, MineII, BossObstacle };
enum CrashType { Error, NoCrash, PlaneCrash, BoomCrash, MineICrash, MineIICrash, BossCrash };
enum ColorType { Black, White };
enum LifeState { Die, Alive };
struct Coordinate {
    int16_t x;
    int16_t y;
};

struct ObstacleInfo {
    ObstacleType type;
    uint8_t score;
    Coordinate coordinate;
};

struct ObjectInfo {
    const unsigned char* bitmap;
    struct Coordinate coordinate;
    enum Visibility visible;
    enum Direction dir;
};

class FsObject {
   private:
    struct ObjectInfo mInfo;
    Speed mSpeed;
    ColorType mColor;
    uint8_t mFrame; // NOTE: use to Blink object
    uint8_t mBlinkOut = 5 * 2;
    LifeState mLifeState = Alive;
   protected:
    void setCoordinate(Coordinate coordinate);

   public:
    FsObject(ObjectInfo);
    virtual ~FsObject();

    Coordinate getCoordinate() const;
    int move();
    int move(Coordinate newCoordinate);
    ColorType getColor() const;
    uint8_t setColor(ColorType);

    Visibility getVisible() const;
    void setVisible(Visibility);
    int setSpeed(Speed speed);
    Speed getSpeed() const;
    int setDir(Direction);
    Direction getDir() const;
    int changeCharacter(const unsigned char* newBitmap);
    const unsigned char* getBitmap() const { return this->mInfo.bitmap; }
    virtual int updateWall() { return 0; };
    virtual int render() { return 0; };
    virtual ObstacleType getType() const { return ObstacleType::None; }
    virtual uint8_t getScore() const { return 0; }
    void setLifeState(LifeState);
    LifeState getAlive() const;
    uint8_t getBlinkCount() const;
};

class FsPlane : public FsObject {
    GameLevel mLevel;
    uint8_t mFrame;
   public:
    FsPlane(const unsigned char* bitmap, Coordinate firstCoordinate, GameLevel level = GameLevel::Easy);
    ~FsPlane() override = default;
    void setDie();
    int getDie() const;
    int render() override;
};

class FsMissile : public FsObject {
   private:
    MissileType mType = MissileType::Normal;
    MissileOwner mOwner = MissileOwner::PlaneOwner;
   public:
    FsMissile(const unsigned char* bitmap, Coordinate firstCoordinate = {0, 0}, MissileOwner owner = MissileOwner::PlaneOwner, Direction dir = Direction::LeftToRight);
    ~FsMissile() override = default;

    int setMode(MissileType newType);
    int getMode() const;
    MissileOwner getOwner() const;
};

class FsExplosion : public FsObject {
   private:
    uint8_t mFrame;
    uint8_t mLifeTime;

   public:
    FsExplosion(Coordinate firstCoordinate);
    ~FsExplosion() override;
    int render() override;
};

class FsObstacle : public FsObject {
   private:
    ObstacleType mType;
    uint8_t mScore;

   public:
    FsObstacle(const unsigned char* bitmap, ObstacleInfo newObstacle);
    ~FsObstacle() override = default;

    ObstacleType getType() const override;
    uint8_t getScore() const override;
};

class FsBoss : public FsObject {
   public:
    enum BossLevel { Easy = 5, Normal, Hard };
    struct BossInfo {
        BossLevel level;
        uint8_t score;
        uint8_t hp;
        Coordinate coordinate;
        const unsigned char* missileBitmap;
        const unsigned char* bossBitmap[FS_BOSS_MAX_BITMAP_FRAME];
    };

   private:
    uint8_t mScore;
    uint8_t mHp;
    uint8_t mFrame;
    BossLevel mLevel = BossLevel::Easy;
    bool mBossAppear;
    const unsigned char* mMissileBitmap;
    const unsigned char* mBossBitmap[FS_BOSS_MAX_BITMAP_FRAME];

   public:
    FsBoss(BossInfo bossInfo);
    ~FsBoss() override;
    ObstacleType getType() const override;
    uint8_t getScore() const override;
    int getHp() const;
    int decreaseHp();
    bool needMissile(Coordinate *missle);
    const unsigned char* getBossBitmap() const;
    const unsigned char* getMissileBitmap() const;
    int render() override;
};

class FsTunnelWall : public FsObject {
   public:
    FsTunnelWall(const unsigned char* bitmap, int16_t xTop);
    ~FsTunnelWall() override = default;
    int updateWall() override;
};

typedef struct {
    ObjectType type;
    FsObject* obj;
} ObjectEntry;

class FsCore {
    typedef void (*RenderFunc)(int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, uint16_t color);

   private:
    const uint8_t mMinMissle = 4;
    RenderFunc mRenderFunc;
    std::vector<ObjectEntry> mListObject;
    uint16_t mTotalScore;
    bool mBossAppear;
    bool mPlaneDie;
    int mStepScoreBossApear;
    uint8_t mMissle;
    GameLevel mLevel;

   private:
    int computePlaneCrash(FsObject* plane, FsObject* wall);
    int computePlaneCrash(FsObject* plane, FsObject* something, uint16_t w, uint16_t h);
    int computeMissileCrash(FsObject* missile, FsObject* something, uint16_t w, uint16_t h);
    int setupMissile(FsObject* missile);
    CrashType calculateCrash();
    const char* getType(ObjectType);

    int setupBattle();
    int getMissle(std::vector<FsObject*>& missles, MissileOwner owner);
    int bossBattle();
    CrashType planeCrash();
    std::vector<ObjectEntry>::iterator deleteObject(std::vector<ObjectEntry>::iterator it);

   public:
    FsCore(RenderFunc mRenderFunc, GameLevel level = GameLevel::Easy);
    ~FsCore();
    int getObject(std::vector<FsObject*>& obj, ObjectType type);
    int addObject(ObjectEntry);
    CrashType render();
    int getScore() const;
    bool needBossAppear();
    int getMisslePlane() const;
};

}  // namespace FsGame

#endif
