#ifndef INFOCENTER_H
#define INFOCENTER_H

#include "actionsfwd.h"
#include "legogamestate.h"
#include "legostate.h"
#include "legoworld.h"
#include "misc.h"
#include "mxgeometry.h"
#include "radio.h"

class MxNotificationParam;
class MxStillPresenter;
class LegoControlManagerNotificationParam;

// VTABLE: LEGO1 0x100d93a8
// VTABLE: BETA10 0x101b9b88
// SIZE 0x94
class InfocenterState : public LegoState {
public:
	enum {
		e_playCutscene = 0,
		e_introCancelled = 1,
		e_notRegistered = 2,
		e_newState = 3,
		e_selectedSave = 4,
		e_selectedCharacterAndDestination = 5,
		// e_6 = 6,
		// e_7 = 7,
		e_exitQueried = 8,
		// e_9 = 9,
		// e_10 = 10,
		e_welcomeAnimation = 11,
		e_exiting = 12,
		e_playCredits = 13,
		e_exitingToIsland = 14,
		e_backToInfoAct1 = 15,
	};

	InfocenterState();
	~InfocenterState() override;

	// FUNCTION: LEGO1 0x10071830
	MxBool IsSerializable() override { return FALSE; } // vtable+0x14

	// FUNCTION: LEGO1 0x10071840
	// FUNCTION: BETA10 0x10031ee0
	const char* ClassName() const override // vtable+0x0c
	{
		// STRING: LEGO1 0x100f04dc
		return "InfocenterState";
	}

	// FUNCTION: LEGO1 0x10071850
	MxBool IsA(const char* p_name) const override // vtable+0x10
	{
		return !strcmp(p_name, InfocenterState::ClassName()) || LegoState::IsA(p_name);
	}

	MxS16 GetMaxNameLength() { return sizeOfArray(m_letters); }
	MxStillPresenter* GetNameLetter(MxS32 p_index) { return m_letters[p_index]; }
	void SetNameLetter(MxS32 p_index, MxStillPresenter* p_letter) { m_letters[p_index] = p_letter; }

	// FUNCTION: BETA10 0x10031bd0
	MxBool HasRegistered() { return m_letters[0] != NULL; }

	// FUNCTION: BETA10 0x10031c10
	InfomainScript::Script GetNextLeaveDialogue()
	{
		return (InfomainScript::Script) m_leaveDialogue[GameState()->GetCurrentAct()].Next();
	}

	// FUNCTION: BETA10 0x10031ac0
	InfomainScript::Script GetNextReturnDialogue()
	{
		return (InfomainScript::Script) m_returnDialogue[GameState()->GetCurrentAct()].Next();
	}

	// TODO: These probably don't exist according to BETA
	Playlist& GetExitDialogueAct1() { return m_exitDialogueAct1; }
	Playlist& GetExitDialogueAct23() { return m_exitDialogueAct23; }
	Playlist& GetBricksterDialogue() { return m_bricksterDialogue; }

	// SYNTHETIC: LEGO1 0x10071900
	// InfocenterState::`scalar deleting destructor'

	// TODO: Most likely getters/setters are not used according to BETA.

	Playlist m_exitDialogueAct1;    // 0x08
	Playlist m_exitDialogueAct23;   // 0x14
	Playlist m_returnDialogue[3];   // 0x20
	Playlist m_leaveDialogue[3];    // 0x44
	Playlist m_bricksterDialogue;   // 0x68
	MxU32 m_state;                  // 0x74
	MxStillPresenter* m_letters[7]; // 0x78
};

// SIZE 0x18
struct InfocenterMapEntry {
	enum {
		e_infocenter = 3,
		e_jetrace = 10,
		e_carrace = 11,
		e_pizzeria = 12,
		e_garage = 13,
		e_hospital = 14,
		e_police = 15,
	};

	InfocenterMapEntry();

	MxStillPresenter* m_destCtl; // 0x00
	MxU32 m_target;              // 0x04
	MxRect<MxS32> m_area;        // 0x08
};

// VTABLE: LEGO1 0x100d9338
// VTABLE: BETA10 0x101b9b10
// SIZE 0x1d8
class Infocenter : public LegoWorld {
public:
	enum Cutscene {
		e_noIntro = -1,
		e_legoMovie,
		e_mindscapeMovie,
		e_introMovie,
		e_outroMovie,
		e_badEndMovie,
		e_goodEndMovie
	};

	enum Character {
		e_noCharacter = 0,
		e_pepper,
		e_mama,
		e_papa,
		e_nick,
		e_laura
	};

	Infocenter();
	~Infocenter() override;

	MxLong Notify(MxParam& p_param) override; // vtable+0x04
	MxResult Tickle() override;               // vtable+0x08

	// FUNCTION: LEGO1 0x1006eb40
	// FUNCTION: BETA10 0x100316e0
	const char* ClassName() const override // vtable+0x0c
	{
		// STRING: LEGO1 0x100f04ec
		return "Infocenter";
	}

	// FUNCTION: LEGO1 0x1006eb50
	MxBool IsA(const char* p_name) const override // vtable+0x10
	{
		return !strcmp(p_name, Infocenter::ClassName()) || LegoWorld::IsA(p_name);
	}

	MxResult Create(MxDSAction& p_dsAction) override; // vtable+0x18
	void ReadyWorld() override;                       // vtable+0x50
	MxBool VTable0x5c() override;                     // vtable+0x5c
	MxBool Escape() override;                         // vtable+0x64
	void Enable(MxBool p_enable) override;            // vtable+0x68

	// SYNTHETIC: LEGO1 0x1006ec60
	// Infocenter::`scalar deleting destructor'

private:
	void InitializeBitmaps();

	MxLong HandleKeyPress(SDL_Keycode p_key);
	MxU8 HandleMouseMove(MxS32 p_x, MxS32 p_y);
	MxU8 HandleButtonUp(MxS32 p_x, MxS32 p_y);
	MxU8 HandleControl(LegoControlManagerNotificationParam& p_param);
	MxLong HandleEndAction(MxEndActionNotificationParam& p_param);
	MxLong HandleNotification0(MxNotificationParam& p_param);

	void UpdateFrameHot(MxBool p_display);
	void Reset();

	void PlayCutscene(Cutscene p_entityId, MxBool p_scale);
	void StopCutscene();

	void UpdateEnabledGlowControl(MxS32 p_x, MxS32 p_y);

	void StartCredits();
	void StopCredits();

	void PlayAction(InfomainScript::Script p_script);
	void StopCurrentAction();

	void PlayBookAnimation();
	void StopBookAnimation();

	InfomainScript::Script m_currentInfomainScript; // 0xf8
	MxS16 m_selectedCharacter;                      // 0xfc
	InfocenterState* m_infocenterState;             // 0x100
	LegoGameState::Area m_destLocation;             // 0x104
	Cutscene m_currentCutscene;                     // 0x108
	Radio m_radio;                                  // 0x10c
	MxStillPresenter* m_dragPresenter;              // 0x11c
	InfocenterMapEntry m_glowInfo[7];               // 0x120
	MxS16 m_enabledGlowControl;                     // 0x1c8
	MxStillPresenter* m_frame;                      // 0x1cc
	MxS16 m_infoManDialogueTimer;                   // 0x1d0
	MxS16 m_bookAnimationTimer;                     // 0x1d2
	MxU16 m_playingMovieCounter;                    // 0x1d4
	MxS16 m_bigInfoBlinkTimer;                      // 0x1d6
};

#endif // INFOCENTER_H
