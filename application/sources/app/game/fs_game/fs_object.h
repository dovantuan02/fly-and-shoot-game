#ifndef FS_OBJECT_H
#define FS_OBJECT_H

#include <cstdint>
#include <vector>

namespace FsGame {

enum Visibility { Visible, Invisible };
enum Direction { LeftToRight, RightToLeft, UpToDown, DownToUp };
enum MissileType { Normal, Split };
enum ObjectType { Plane, Missile, Explosion, Obstacle, Boss, TunnelWall };
enum Speed { Slow = 3, Medium, Fast };
enum ObstacleType { None, Boom, MineI, MineII };
enum CrashType { NoCrash, PlaneCrash, BoomCrash, MineICrash, MineIICrash }; // TODO: missle crash when boss appear

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

   protected:
    void setCoordinate(Coordinate coordinate);

   public:
    FsObject(ObjectInfo);
    virtual ~FsObject();

    Coordinate getCoordinate() const;
    int move();
    int move(Coordinate newCoordinate);

    Visibility isVisible() const;
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
};

class FsPlane : public FsObject {
   public:
    FsPlane(const unsigned char* bitmap, Coordinate firstCoordinate)
        : FsObject({bitmap, firstCoordinate, Visibility::Visible,
                    Direction::UpToDown}) {};
    ~FsPlane() override = default;
};

class FsMissile : public FsObject {
   private:
    MissileType mType = MissileType::Normal;
    Direction mDir = Direction::LeftToRight;

   public:
    FsMissile(const unsigned char* bitmap, Coordinate firstCoordinate)
        : FsObject({bitmap, firstCoordinate, Visibility::Visible,
                    Direction::LeftToRight}) {};
    ~FsMissile() override {
        this->setVisible(Invisible);
        this->move({-1, -1});
    };

    int mode(MissileType newType) {
        this->mType = newType;
        return 0;
    }
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

class FsBoss : public FsExplosion, public FsMissile {
   private:
    uint8_t type;
    uint8_t score;

   public:
    FsBoss(Coordinate firstCoordinate);
    ~FsBoss() override;
};

class FsTunnelWall : public FsObject {
   public:
    FsTunnelWall(const unsigned char* bitmap, int16_t xTop, int16_t xBot);
    ~FsTunnelWall() override = default;
    int updateWall() override;
};

typedef struct {
    ObjectType type;
    FsObject* obj;
} ObjectEntry;

class FsScreen {
    typedef void (*RenderFunc)(int16_t x, int16_t y, const uint8_t* bitmap,
                               int16_t w, int16_t h, uint16_t color);

   private:
    RenderFunc renderFunc;
    std::vector<ObjectEntry> listObject;
    FsObject* mCrashedPlane;
    uint8_t mPlaneCrashBlinkTick;
    bool mPlaneCrashBlinking;

    uint16_t mTotalScore;

   private:
    int computePlaneCrash(FsObject* plane, FsObject* wall);
    int computePlaneObstacleCrash(FsObject* plane, FsObject* obstacle);
    int computeMissileCrash(FsObject* missile, FsObject* obstacle);
    int setupMissile(FsObject* missile);
    CrashType calculateCrash();
    CrashType renderPlaneCrashBlink();
    void beginPlaneCrash(FsObject* plane);
    const char* getType(ObjectType);

   public:
    FsScreen(RenderFunc renderFunc);
    ~FsScreen();
    int getObject(std::vector<FsObject*>& obj, ObjectType type);
    int addObject(ObjectEntry);
    // int removeObject(FsObject obj);
    CrashType render();
    int getScore() const;
};

}  // namespace FsGame

#endif
