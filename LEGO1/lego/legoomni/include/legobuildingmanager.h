#ifndef LEGOBUILDINGMANAGER_H
#define LEGOBUILDINGMANAGER_H

#include "decomp.h"
#include "lego1_export.h"
#include "misc/legotypes.h"
#include "mxcore.h"

class LegoEntity;
class LegoROI;
class LegoStorage;
class LegoWorld;
class LegoCacheSound;
class LegoPathBoundary;

// SIZE 0x2c
struct LegoBuildingInfo {
	enum {
		c_hasVariants = 0x01,
		c_hasSounds = 0x02,
		c_hasMoves = 0x04,
		c_hasMoods = 0x08
	};

	LegoEntity* m_entity;         // 0x00
	const char* m_variant;        // 0x04
	MxU32 m_sound;                // 0x08
	MxU32 m_move;                 // 0x0c
	MxU8 m_mood;                  // 0x10
	MxS8 m_counter;               // 0x11
	MxS8 m_initialCounter;        // 0x12 - initial value loaded to m_counter
	MxU8 m_flags;                 // 0x13
	float m_adjustedY;            // 0x14
	const char* m_boundaryName;   // 0x18
	float m_x;                    // 0x1c
	float m_y;                    // 0x20
	float m_z;                    // 0x24
	LegoPathBoundary* m_boundary; // 0x28
};

// VTABLE: LEGO1 0x100d6f50
// SIZE 0x30
class LegoBuildingManager : public MxCore {
public:
	// SIZE 0x14
	struct AnimEntry {
		LegoEntity* m_entity; // 0x00
		LegoROI* m_roi;       // 0x04
		MxLong m_time;        // 0x08
		float m_y;            // 0x0c
		MxBool m_muted;       // 0x10
	};

	LegoBuildingManager();
	~LegoBuildingManager() override;

	MxResult Tickle() override; // vtable+0x08

	// FUNCTION: LEGO1 0x1002f930
	const char* ClassName() const override // vtable+0x0c
	{
		// While this class exists in BETA10, it didn't have a ClassName().
		// The constructor suggests that it did not inherit from MxCore back then and did not have a VTABLE.
		// STRING: LEGO1 0x100f37d0
		return "LegoBuildingManager";
	}

	LEGO1_EXPORT static void configureLegoBuildingManager(MxS32);
	static void SetCustomizeAnimFile(const char* p_value);

	void Init();
	void LoadWorldInfo();
	void CreateBuilding(MxS32 p_index, LegoWorld* p_world);
	void Reset();
	MxResult Write(LegoStorage* p_storage);
	MxResult Read(LegoStorage* p_storage);
	LegoBuildingInfo* GetInfo(LegoEntity* p_entity);
	MxBool SwitchVariant(LegoEntity* p_entity);
	MxBool SwitchSound(LegoEntity* p_entity);
	MxBool SwitchMove(LegoEntity* p_entity);
	MxBool SwitchMood(LegoEntity* p_entity);
	MxU32 GetAnimationId(LegoEntity* p_entity);
	MxU32 GetSoundId(LegoEntity* p_entity, MxBool p_basedOnMood);
	MxBool DecrementCounter(LegoEntity* p_entity);
	MxBool DecrementCounter(MxS32 p_index);
	MxBool DecrementCounter(LegoBuildingInfo* p_data);
	void ScheduleAnimation(LegoEntity* p_entity, MxLong p_length, MxBool p_haveSound, MxBool p_hideAfterAnimation);
	void ClearCounters();
	void AdjustHeight(MxS32 p_index);
	MxResult DetermineBoundaries();
	LegoBuildingInfo* GetInfoArray(MxS32& p_length);
	void AdjustCounter(LegoEntity* p_entity, MxS32 p_adjust);
	void SetInitialCounters();

	static const char* GetCustomizeAnimFile() { return g_customizeAnimFile; }

	// SYNTHETIC: LEGO1 0x1002f940
	// LegoBuildingManager::`scalar deleting destructor'

private:
	static char* g_customizeAnimFile;
	static MxS32 g_maxMove[16];
	static MxU32 g_maxSound;

	MxU8 m_nextVariant;            // 0x08
	MxBool m_boundariesDetermined; // 0x09
	AnimEntry* m_entries[5];       // 0x0c
	MxS8 m_numEntries;             // 0x20
	LegoCacheSound* m_sound;       // 0x24
	MxBool m_hideAfterAnimation;   // 0x28
	LegoWorld* m_world;            // 0x2c

	friend class DebugViewer;
};

#endif // LEGOBUILDINGMANAGER_H
