#ifndef LEGOPLANTMANAGER_H
#define LEGOPLANTMANAGER_H

#include "decomp.h"
#include "legomain.h"
#include "mxcore.h"

class LegoEntity;
class LegoPathBoundary;
struct LegoPlantInfo;
class LegoROI;
class LegoStorage;
class LegoWorld;

// VTABLE: LEGO1 0x100d6758
// SIZE 0x2c
class LEGO1_EXPORT LegoPlantManager : public MxCore {
public:
	// SIZE 0x0c
	struct AnimEntry {
		LegoEntity* m_entity; // 0x00
		LegoROI* m_roi;       // 0x04
		MxLong m_time;        // 0x08
	};

	LegoPlantManager();
	~LegoPlantManager() override; // vtable+0x00

	MxResult Tickle() override; // vtable+0x08

	// FUNCTION: LEGO1 0x10026290
	const char* ClassName() const override // vtable+0x0c
	{
		// While this class exists in BETA10, it didn't have a ClassName().
		// The constructor suggests that it did not inherit from MxCore back then and did not have a VTABLE.
		// STRING: LEGO1 0x100f318c
		return "LegoPlantManager";
	}

	void Init();
	void LoadWorldInfo(LegoOmni::World p_worldId);
	void Reset(LegoOmni::World p_worldId);
	MxResult Write(LegoStorage* p_storage);
	MxResult Read(LegoStorage* p_storage);
	MxS32 GetNumPlants();
	MxBool SwitchColor(LegoEntity* p_entity);
	MxBool SwitchVariant(LegoEntity* p_entity);
	MxBool SwitchSound(LegoEntity* p_entity);
	MxBool SwitchMove(LegoEntity* p_entity);
	MxBool SwitchMood(LegoEntity* p_entity);
	MxU32 GetAnimationId(LegoEntity* p_entity);
	MxU32 GetSoundId(LegoEntity* p_entity, MxBool p_basedOnMood);
	LegoPlantInfo* GetInfoArray(MxS32& p_length);
	LegoEntity* CreatePlant(MxS32 p_index, LegoWorld* p_world, LegoOmni::World p_worldId);
	MxBool DecrementCounter(LegoEntity* p_entity);
	void ScheduleAnimation(LegoEntity* p_entity, MxLong p_length);
	MxResult DetermineBoundaries();
	void ClearCounters();
	void SetInitialCounters();

	static void SetCustomizeAnimFile(const char* p_value);

	// FUNCTION: BETA10 0x1007ffa0
	static const char* GetCustomizeAnimFile() { return g_customizeAnimFile; }

	// SYNTHETIC: LEGO1 0x100262a0
	// LegoPlantManager::`scalar deleting destructor'

private:
	void RemovePlant(MxS32 p_index, LegoOmni::World p_worldId);
	void AdjustHeight(MxS32 p_index);
	LegoPlantInfo* GetInfo(LegoEntity* p_entity);
	MxBool DecrementCounter(MxS32 p_index);
	void AdjustCounter(LegoEntity* p_entity, MxS32 p_adjust);

	static char* g_customizeAnimFile;
	static MxS32 g_maxMove[4];
	static MxU32 g_maxSound;

	LegoOmni::World m_worldId;     // 0x08
	MxBool m_boundariesDetermined; // 0x0c
	AnimEntry* m_entries[5];       // 0x10
	MxS8 m_numEntries;             // 0x24
	LegoWorld* m_world;            // 0x28

	friend class DebugViewer;
};

#endif // LEGOPLANTMANAGER_H
