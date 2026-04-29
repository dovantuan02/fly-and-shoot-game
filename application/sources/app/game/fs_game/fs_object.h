#ifndef FS_OBJECT_H
#define FS_OBJECT_H

#include <cstdint>
#include <vector>

namespace FsGame {

struct Coordinate {
    int8_t x;
    int8_t y;
};

struct ObstacleInfo {
    uint8_t type;
    uint8_t score;
    Coordinate coordinate;
};

enum Visibility { Visible, Invisible };
enum Direction { LeftToRight, RightToLeft, UpToDown, DownToUp };
enum MissileType { Normal, Split };
enum ObjectType { Plane, Missile, Explosion, Obstacle, Boss };
enum Speed { Slow = 3, Medium, Fast };

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
    virtual int render();
};

class FsPlane : public FsObject {
   public:
    FsPlane(const unsigned char* bitmap, Coordinate firstCoordinate) : FsObject({bitmap, firstCoordinate, Visibility::Visible, Direction::UpToDown}) {};
    ~FsPlane() override = default;
};

class FsMissile : public FsObject {
   private:
    MissileType mType = MissileType::Normal;
    Direction mDir = Direction::LeftToRight;
    uint8_t mSpeed = 1;
    uint8_t mDamage = 1;

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
    uint8_t version;

   public:
    FsExplosion(Coordinate firstCoordinate);
    ~FsExplosion() override;
};

class FsObstacle : public FsObject {
   private:
    uint8_t type;
    uint8_t score;

   public:
    FsObstacle(const ObstacleInfo newObstacle[]);
    ~FsObstacle() override;

    int add(ObstacleInfo newObstacle);
    int remove();
};

class FsBoss : public FsExplosion, public FsMissile {
   private:
    uint8_t type;
    uint8_t score;

   public:
    FsBoss(Coordinate firstCoordinate);
    ~FsBoss() override;
};

typedef struct {
    ObjectType type;
    FsObject *obj;
} ObjectEntry;

class FsScreen {
    typedef void (*RenderFunc)(int16_t x, int16_t y, const uint8_t* bitmap,
                               int16_t w, int16_t h, uint16_t color);

   private:
    RenderFunc renderFunc;
    std::vector<ObjectEntry> listObject;

   public:
    FsScreen(RenderFunc renderFunc);
    ~FsScreen();
    int getObject(std::vector<FsObject*> &obj, ObjectType type);
    int addObject(ObjectEntry);
    // int removeObject(FsObject obj);
    int render();
};

}  // namespace FsGame

#endif