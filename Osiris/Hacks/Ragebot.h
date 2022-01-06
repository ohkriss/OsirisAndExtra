#pragma once
#include "../UniqueVector.h"

struct UserCmd;
class Entity;

namespace Ragebot
{
    struct AnimationBackup {
        Vector origin{};
        Vector absOrigin{};
        Vector velocity{};
        Vector absVelocity{};
        int flags{};
        int eFlags{};
        float duck{};
        float body{};
        AnimationLayer layers[13];
    };

    enum HitscanMode : int {
        NORMAL = 0,
        LETHAL = 1,
        LETHAL2 = 3, 
        PREFER = 4
    };

    struct HitscanData {
        float damage{};
        Vector position{};
    };

    struct HitscanBox {
        int index{};
        HitscanMode mode{};
    };

    class AimPlayer {
    public:
        std::deque<std::shared_ptr<Backtrack::Record>> records{ };
        stdpp::unique_vector<HitscanBox> hitboxes{ };
    public:
        Entity* entity{ };
        float spawn{ };

        int shots{ };
        int missedShots{ };
        Backtrack::Record walkRecord{ };

        float bodyUpdate{ };
        bool moved{ };

        int standIndex{ };
        int movingIndex{ };
        int unknownMove{ };
        int bodyIndex{ };
        int antiIndex{ };
        int freestandIndex{ };
        int lbyDeltaIndex{ };
        int lastMove{ };

        std::string resolverMode{ };
        float updateTime{ };

        float body{ };
        float oldBody{ };

    public:
        void framestageUpdate(Entity* entity) noexcept;
        void updateAnimations(Backtrack::Record record) noexcept;
        void onRoundStart(Entity* entity);
        void setupHitboxes(Backtrack::Record* record, bool history);
        bool setupHitboxPoints(Backtrack::Record* record, BoneArray* bones, int index, std::vector< Vector >& points);
        bool getBestAimPosition(Vector& aim, float& damage, Backtrack::Record* record);

    public:
        void reset() {
            entity = nullptr;
            spawn = 0.f;
            walkRecord = Backtrack::Record{};
            shots = 0;
            missedShots = 0;

            records.clear();
            hitboxes.clear();
        }
    };

    inline std::array< AimPlayer, 64 > players;
    inline std::vector< AimPlayer* >   targets;


	void run(UserCmd*) noexcept;
	void updateInput() noexcept;

    struct Enemies {
        int id;
        int health;
        float distance;
        float fov;

        bool operator<(const Enemies& enemy) const noexcept {
            if (health != enemy.health)
                return health < enemy.health;
            if (fov != enemy.fov)
                return fov < enemy.fov;
            return distance < enemy.distance;
        }

        Enemies(int id, int health, float distance, float fov) noexcept : id(id), health(health), distance(distance), fov(fov) { }
    };

    struct {
        bool operator()(Enemies a, Enemies b) const
        {
            return a.health < b.health;
        }
    } healthSort;
    struct {
        bool operator()(Enemies a, Enemies b) const
        {
            return a.distance < b.distance;
        }
    } distanceSort;
    struct {
        bool operator()(Enemies a, Enemies b) const
        {
            return a.fov < b.fov;
        }
    } fovSort;
}