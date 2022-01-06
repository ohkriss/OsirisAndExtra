#pragma once

#include <array>
#include <deque>

#include "../ConfigStructs.h"

#include "../SDK/NetworkChannel.h"
#include "../SDK/matrix3x4.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/Vector.h"
#include "../SDK/Client.h"
#include "../SDK/GameEvent.h"
#include "../SDK/ClientState.h"

enum class FrameStage;
struct UserCmd;

namespace Backtrack
{
    void update(FrameStage) noexcept;
    void run(UserCmd*) noexcept;

    void addLatencyToNetwork(NetworkChannel*, float) noexcept;
    void updateIncomingSequences() noexcept;

    float getLerp() noexcept;

    struct Record {
		Entity* entity;
		float   immune;
		int     tick;
		int     lag;
		bool    dormant;

        Vector origin;
        Vector oldOrigin;
        Vector velocity;

        Vector head;
        Vector eyeAngle;
        Vector absAngle;
        Vector mins;
        Vector maxs;

        float body;
        float duck;

        float simulationTime;
        float oldSimulationTime;

        int flags;

        matrix3x4 matrix[MAXSTUDIOBONES];

        AnimationLayer layers[13];
        float poses[24];
        Vector animVelocity;

        bool setupBones;
		UtlVector<matrix3x4>* bones;

		// lagfix stuff.
		bool   brokeLc;
		Vector predOrigin;
		Vector predVelocity;
		float  predTime;
		int    predFlags;

		// resolver stuff.
		size_t mode;
		bool   fakeWalk;
		bool   shot;
		float  away;
		float  animTime;
		std::string resolverMode;

		// other stuff.
		float  m_interp_time;
	public:

		// default ctor.
		__forceinline Record() :
			setupBones{ false },
			brokeLc{ false },
			fakeWalk{ false },
			shot{ false },
			lag{},
			bones{} {}

		// ctor.
		__forceinline Record(Entity* entity) :
			setupBones{ false },
			brokeLc{ false },
			fakeWalk{ false },
			shot{ false },
			lag{},
			bones{} {

			store(entity);
		}

		// dtor.
		__forceinline ~Record() {
			// free heap allocated game mem.
			memory->memalloc->Free(bones);
		}

		__forceinline void invalidate() {
			// free heap allocated game mem.
			memory->memalloc->Free(bones);

			// mark as not setup.
			setupBones = false;

			// allocate new memory.
			bones = (UtlVector<matrix3x4>*)memory->memalloc->Alloc(sizeof(UtlVector<matrix3x4>) * 128);
		}

		// function: allocates memory for SetupBones and stores relevant data.
		void store(Entity* entity) {
			// allocate game heap.
			bones = (UtlVector<matrix3x4>*)memory->memalloc->Alloc(sizeof(UtlVector<matrix3x4>) * 128);

			// player data.
			entity = entity;
			immune = entity->gunGameImmunity();
			tick = memory->clientState->m_clock_drift_mgr.m_server_tick;
			// netvars.
			predTime = simulationTime = entity->simulationTime();
			oldSimulationTime = entity->simulationTime();
			predFlags = flags = entity->flags();
			predOrigin = origin = entity->origin();
			oldOrigin = entity->origin();
			eyeAngle = entity->eyeAngles();
			absAngle = entity->getAbsAngle();
			body = entity->lby();
			mins = entity->getCollideable()->vecMins;
			maxs = entity->getCollideable()->vecMaxs;
			duck = entity->duckAmount();
			predVelocity = velocity = entity->velocity();

			// save networked animlayers.
			memcpy(layers, entity->animOverlays(), sizeof(AnimationLayer) * 13);

			// normalize eye angles.
			eyeAngle.normalize();
			std::clamp(eyeAngle.x, -90.f, 90.f);

			// get lag.
			lag = (int)(0.5f + (simulationTime - oldSimulationTime) / memory->globalVars->intervalPerTick);

			// compute animtime.
			animTime = oldSimulationTime + memory->globalVars->intervalPerTick;
		}

		// function: restores 'predicted' variables to their original.
		__forceinline void predict() {
			brokeLc = false;
			predOrigin = origin;
			predVelocity = velocity;
			predTime = simulationTime;
			predFlags = flags;
		}

		// function: writes current record to bone cache.
		__forceinline void cache() {
			// get bone cache ptr.
			auto cache = &entity->getBoneCache();

			cache = bones;
			cache->allocationCount = 128;

			entity->origin() = predOrigin;
			entity->getCollideable()->vecMins = mins;
			entity->getCollideable()->vecMaxs = maxs;

			//entity->setAbsAngles(absAngle);
			//entity->setAbsOrigin(predOrigin);
		}

		__forceinline bool dormant() {
			return dormant;
		}

		__forceinline bool immune() {
			return immune > 0.f;
		}

		// function: checks if LagRecord obj is hittable if we were to fire at it now.
		bool valid() {
			// use prediction curtime for this.
			float curtime = (int)(0.5f + (localPlayer->tickBase()) / memory->globalVars->intervalPerTick);
			// correct is the amount of time we have to correct game time,
			float correct{ };
			//correct = g_cl.m_lerp + g_cl.m_latency;
			
			// stupid fake latency goes into the incoming latency.
			//float in = g_csgo.m_net->GetLatency(INetChannel::FLOW_INCOMING);
			//correct += in;

			// check bounds [ 0, sv_maxunlag ]
			//std::clamp(correct, 0.f, g_csgo.sv_maxunlag->GetFloat());

			// calculate difference between tick sent by player and our latency based tick.
			// ensure this record isn't too old.
			return std::abs(correct - (curtime - simulationTime)) < 0.19f;
		}

    };

    struct incomingSequence {
        int inreliablestate;
        int sequencenr;
        float servertime;
    };

    const std::deque<Record>* getRecords(std::size_t index) noexcept;
    bool valid(float simtime) noexcept;
    void init() noexcept;
    float getMaxUnlag() noexcept;
}
