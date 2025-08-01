#include "registrationbook.h"

#include "copter_actions.h"
#include "dunebuggy.h"
#include "dunecar_actions.h"
#include "helicopter.h"
#include "infocenter.h"
#include "isle.h"
#include "jetski.h"
#include "jetski_actions.h"
#include "jukebox_actions.h"
#include "legocontrolmanager.h"
#include "legogamestate.h"
#include "legoinputmanager.h"
#include "legopathstruct.h"
#include "legoutils.h"
#include "misc.h"
#include "mxactionnotificationparam.h"
#include "mxbackgroundaudiomanager.h"
#include "mxcontrolpresenter.h"
#include "mxdisplaysurface.h"
#include "mxmisc.h"
#include "mxnotificationmanager.h"
#include "mxstillpresenter.h"
#include "mxtimer.h"
#include "mxtransitionmanager.h"
#include "racecar.h"
#include "racecar_actions.h"
#include "regbook_actions.h"
#include "scripts.h"

#include <SDL3/SDL_log.h>

DECOMP_SIZE_ASSERT(RegistrationBook, 0x2d0)

// GLOBAL: LEGO1 0x100d9924
// GLOBAL: BETA10 0x101bfb3c
const char* g_infoman = "infoman";

// GLOBAL: LEGO1 0x100f7964
MxLong g_checkboxBlinkTimer = 0;

// GLOBAL: LEGO1 0x100f7968
MxBool g_nextCheckbox = FALSE;

// FUNCTION: LEGO1 0x10076d20
RegistrationBook::RegistrationBook() : m_registerDialogueTimer(0x80000000), m_unk0xfc(1)
{
	memset(m_alphabet, 0, sizeof(m_alphabet));
	memset(m_intAlphabet, 0, sizeof(m_intAlphabet));
	memset(m_name, 0, sizeof(m_name));
	m_unk0x280.m_cursorPos = 0;

	memset(m_checkmark, 0, sizeof(m_checkmark));
	memset(&m_unk0x280, -1, sizeof(m_unk0x280) - 2);

	m_unk0x2b8 = 0;
	m_infocenterState = NULL;

	NotificationManager()->Register(this);

	m_unk0x2c1 = FALSE;
	m_checkboxHilite = NULL;
	m_checkboxSurface = NULL;
	m_checkboxNormal = NULL;
}

// FUNCTION: LEGO1 0x10076f50
RegistrationBook::~RegistrationBook()
{
	for (MxS16 i = 0; i < 10; i++) {
		for (MxS16 j = 0; j < 7; j++) {
			if (m_name[i][j] != NULL) {
				delete m_name[i][j]->GetAction();
				delete m_name[i][j];
				m_name[i][j] = NULL;
			}
		}
	}

	InputManager()->UnRegister(this);
	if (InputManager()->GetWorld() == this) {
		InputManager()->ClearWorld();
	}

	ControlManager()->Unregister(this);
	NotificationManager()->Unregister(this);

	if (m_checkboxNormal) {
		m_checkboxNormal->Release();
	}
}

// FUNCTION: LEGO1 0x10077060
MxResult RegistrationBook::Create(MxDSAction& p_dsAction)
{
	MxResult result = LegoWorld::Create(p_dsAction);

	if (result == SUCCESS) {
		InputManager()->SetWorld(this);
		ControlManager()->Register(this);
		SetIsWorldActive(FALSE);
		InputManager()->Register(this);

		GameState()->m_currentArea = LegoGameState::e_regbook;
		GameState()->StopArea(LegoGameState::e_previousArea);

		m_infocenterState = (InfocenterState*) GameState()->GetState("InfocenterState");
	}

	return result;
}

// FUNCTION: LEGO1 0x100770e0
// FUNCTION: BETA10 0x100f2d98
MxLong RegistrationBook::Notify(MxParam& p_param)
{
	MxNotificationParam& param = (MxNotificationParam&) p_param;
	MxLong result = 0;
	LegoWorld::Notify(p_param);

	if (m_worldStarted) {
		switch (param.GetNotification()) {
		case c_notificationEndAction:
			result = HandleEndAction((MxEndActionNotificationParam&) p_param);
			break;
		case c_notificationKeyPress:
			m_registerDialogueTimer = Timer()->GetTime();
			result = HandleKeyPress(((LegoEventNotificationParam&) p_param).GetKey());
			break;
		case c_notificationButtonDown:
			m_registerDialogueTimer = Timer()->GetTime();
			break;
		case c_notificationControl:
			result = HandleControl((LegoControlManagerNotificationParam&) p_param);
			break;
		case c_notificationPathStruct:
			result = HandlePathStruct((LegoPathStructNotificationParam&) p_param);
			break;
		case c_notificationTransitioned:
			GameState()->SwitchArea(LegoGameState::e_infomain);
			break;
		}
	}

	return result;
}

// FUNCTION: LEGO1 0x10077210
MxLong RegistrationBook::HandleEndAction(MxEndActionNotificationParam& p_param)
{
	if (p_param.GetAction()->GetAtomId() != m_atomId) {
		return 0;
	}

	switch ((MxS32) p_param.GetAction()->GetObjectId()) {
	case RegbookScript::c_Textures:
		m_unk0x2c1 = FALSE;

		if (m_unk0x2b8 == 0) {
			TransitionManager()->StartTransition(MxTransitionManager::e_mosaic, 50, FALSE, FALSE);
		}
		break;
	case RegbookScript::c_iic006in_RunAnim:
	case RegbookScript::c_iic007in_PlayWav:
	case RegbookScript::c_iic008in_PlayWav:
		BackgroundAudioManager()->RaiseVolume();
		m_registerDialogueTimer = Timer()->GetTime();
		break;
	}

	return 1;
}

// FUNCTION: LEGO1 0x100772d0
MxLong RegistrationBook::HandleKeyPress(SDL_Keycode p_key)
{
	// keycode is case-insensitive
	SDL_Keycode key = p_key;

	MxStillPresenter** intoAlphabet = [this, key]() -> MxStillPresenter** {
		if (key >= SDLK_A && key <= SDLK_Z) {
			return &m_alphabet[key - SDLK_A];
		}

		for (int i = 0; i < sizeOfArray(m_intAlphabet); i++) {
			if (m_intAlphabet[i] && LegoGameState::g_intCharacters[i].m_character == key) {
				return &m_intAlphabet[i];
			}
		}

		return nullptr;
	}();

	if (!intoAlphabet && key != SDLK_BACKSPACE) {
		if (key == SDLK_SPACE) {
			DeleteObjects(&m_atomId, RegbookScript::c_iic006in_RunAnim, RegbookScript::c_iic008in_PlayWav);
			BackgroundAudioManager()->RaiseVolume();
		}
	}
	else if (key != SDLK_BACKSPACE && m_unk0x280.m_cursorPos < 7) {
		m_name[0][m_unk0x280.m_cursorPos] = (*intoAlphabet)->Clone();

		if (m_name[0][m_unk0x280.m_cursorPos] != NULL) {
			(*intoAlphabet)->GetAction()->SetUnknown24((*intoAlphabet)->GetAction()->GetUnknown24() + 1);
			m_name[0][m_unk0x280.m_cursorPos]->Enable(TRUE);
			m_name[0][m_unk0x280.m_cursorPos]->SetTickleState(MxPresenter::e_repeating);
			m_name[0][m_unk0x280.m_cursorPos]->SetPosition(m_unk0x280.m_cursorPos * 23 + 343, 121);

			if (m_unk0x280.m_cursorPos == 0) {
				m_checkmark[0]->Enable(TRUE);
			}

			m_unk0x280.m_letters[m_unk0x280.m_cursorPos] =
				key >= SDLK_A && key <= SDLK_Z
					? key - SDLK_A
					: (intoAlphabet - m_intAlphabet) + sizeOfArray(m_alphabet) - m_intAlphabetOffset;
			m_unk0x280.m_cursorPos++;
		}
	}
	else {
		if (key == SDLK_BACKSPACE && m_unk0x280.m_cursorPos > 0) {
			m_unk0x280.m_cursorPos--;

			m_name[0][m_unk0x280.m_cursorPos]->Enable(FALSE);

			delete m_name[0][m_unk0x280.m_cursorPos];
			m_name[0][m_unk0x280.m_cursorPos] = NULL;

			if (m_unk0x280.m_cursorPos == 0) {
				m_checkmark[0]->Enable(FALSE);
			}

			m_unk0x280.m_letters[m_unk0x280.m_cursorPos] = -1;
		}
	}

	return 1;
}

// FUNCTION: LEGO1 0x100774a0
MxLong RegistrationBook::HandleControl(LegoControlManagerNotificationParam& p_param)
{
	MxS16 buttonId = p_param.m_enabledChild;
	const InternationalCharacter* intChar = NULL;

	for (int i = 0; i < sizeOfArray(m_intAlphabet); i++) {
		if (m_intAlphabet[i] && LegoGameState::g_intCharacters[i].m_buttonId == buttonId) {
			intChar = &LegoGameState::g_intCharacters[i];
			break;
		}
	}

	if ((buttonId >= 1 && buttonId <= 28) || intChar) {
		if (p_param.m_clickedObjectId == RegbookScript::c_Alphabet_Ctl) {
			// buttonId:
			// - [1, 26]: alphabet
			// - 27: backspace
			// - 28: go back to information center
			// - 29+: international alphabet
			if (buttonId == 28) {
				DeleteObjects(&m_atomId, RegbookScript::c_iic006in_RunAnim, RegbookScript::c_iic008in_PlayWav);

				if (GameState()->GetCurrentAct() == LegoGameState::e_act1) {
					m_infocenterState->m_state = InfocenterState::e_backToInfoAct1;
				}
				else {
					m_infocenterState->m_state = InfocenterState::e_notRegistered;
				}

				TransitionManager()->StartTransition(MxTransitionManager::e_mosaic, 50, FALSE, FALSE);
			}
			else {
				if (buttonId > 28 && !intChar) {
					return 1;
				}

				if (buttonId < 27) {
					HandleKeyPress(SDLK_A + buttonId - 1);
				}
				else if (intChar) {
					HandleKeyPress((SDL_Keycode) intChar->m_character);
				}
				else {
					HandleKeyPress(SDLK_BACKSPACE);
				}
			}
		}
		else {
			// Select another profile (buttonId is always 1)
			InputManager()->DisableInputProcessing();
			DeleteObjects(&m_atomId, RegbookScript::c_iic006in_RunAnim, RegbookScript::c_iic008in_PlayWav);

			MxS16 i;
			for (i = 0; i < 10; i++) {
				if (m_checkmark[i]->GetAction()->GetObjectId() == p_param.m_clickedObjectId) {
					break;
				}
			}

			FUN_100775c0(i);
		}
	}

	return 1;
}

// FUNCTION: LEGO1 0x100775c0
// STUB: BETA10 0x100f32b2
void RegistrationBook::FUN_100775c0(MxS16 p_playerIndex)
{
	if (m_infocenterState->HasRegistered()) {
		GameState()->Save(0);
	}

	// TODO: structure incorrect
	MxS16 player = p_playerIndex == 0 ? GameState()->FindPlayer(*(LegoGameState::Username*) &m_unk0x280.m_letters)
									  : p_playerIndex - 1;

	switch (player) {
	case 0:
		if (!m_infocenterState->HasRegistered()) {
			GameState()->SwitchPlayer(0);
			WriteInfocenterLetters(1);
			FUN_100778c0();
		}
		break;
	case -1:
		GameState()->Init();

		PlayAction(RegbookScript::c_Textures);

		m_unk0x2c1 = TRUE;

		// TOOD: structure incorrect
		GameState()->AddPlayer(*(LegoGameState::Username*) &m_unk0x280.m_letters);
		GameState()->Save(0);

		WriteInfocenterLetters(0);
		GameState()->SerializePlayersInfo(2);
		FUN_100778c0();
		break;
	default:
		GameState()->Init();

		PlayAction(RegbookScript::c_Textures);

		m_unk0x2c1 = TRUE;

		GameState()->SwitchPlayer(player);

		WriteInfocenterLetters(player + 1);
		GameState()->SerializePlayersInfo(2);
		FUN_100778c0();
		break;
	}

	m_infocenterState->m_state = InfocenterState::e_selectedSave;
	if (m_unk0x2b8 == 0 && !m_unk0x2c1) {
		DeleteObjects(&m_atomId, RegbookScript::c_iic006in_RunAnim, RegbookScript::c_iic008in_PlayWav);
		TransitionManager()->StartTransition(MxTransitionManager::e_mosaic, 50, FALSE, FALSE);
	}
}

// FUNCTION: LEGO1 0x10077860
void RegistrationBook::WriteInfocenterLetters(MxS16 p_user)
{
	for (MxS16 i = 0; i < 7; i++) {
		delete m_infocenterState->GetNameLetter(i);
		m_infocenterState->SetNameLetter(i, m_name[p_user][i]);
		m_name[p_user][i] = NULL;
	}
}

// FUNCTION: LEGO1 0x100778c0
void RegistrationBook::FUN_100778c0()
{
	if (GameState()->GetCurrentAct() == LegoGameState::e_act1) {
		Act1State* act1state = (Act1State*) GameState()->GetState("Act1State");

		if (act1state->m_helicopterPlane.IsPresent()) {
			InvokeAction(Extra::e_start, m_atomId, CopterScript::c_Helicopter_Actor, NULL);
			NotificationManager()->Send(
				this,
				LegoPathStructNotificationParam(c_notificationPathStruct, NULL, 0, CopterScript::c_Helicopter_Actor)
			);

			m_unk0x2b8++;
		}

		if (act1state->m_jetskiPlane.IsPresent()) {
			InvokeAction(Extra::e_start, m_atomId, JetskiScript::c_Jetski_Actor, NULL);
			NotificationManager()->Send(
				this,
				LegoPathStructNotificationParam(c_notificationPathStruct, NULL, 0, JetskiScript::c_Jetski_Actor)
			);

			m_unk0x2b8++;
		}

		if (act1state->m_dunebuggyPlane.IsPresent()) {
			InvokeAction(Extra::e_start, m_atomId, DunecarScript::c_DuneBugy_Actor, NULL);
			NotificationManager()->Send(
				this,
				LegoPathStructNotificationParam(c_notificationPathStruct, NULL, 0, DunecarScript::c_DuneBugy_Actor)
			);

			m_unk0x2b8++;
		}

		if (act1state->m_racecarPlane.IsPresent()) {
			InvokeAction(Extra::e_start, m_atomId, RacecarScript::c_RaceCar_Actor, NULL);
			NotificationManager()->Send(
				this,
				LegoPathStructNotificationParam(c_notificationPathStruct, NULL, 0, RacecarScript::c_RaceCar_Actor)
			);

			m_unk0x2b8++;
		}

		if (m_unk0x2b8 != 0) {
			DeleteObjects(&m_atomId, RegbookScript::c_iic006in_RunAnim, RegbookScript::c_iic008in_PlayWav);
			InputManager()->DisableInputProcessing();
			SetAppCursor(e_cursorBusy);
		}
	}
}

// FUNCTION: LEGO1 0x10077cc0
// FUNCTION: BETA10 0x100f3671
void RegistrationBook::ReadyWorld()
{
	// This function is very fragile and appears to oscillate between two versions on small changes.
	// This even happens for commenting out `assert()` calls, which shouldn't affect release builds at all.
	// See https://github.com/isledecomp/isle/pull/1375 for a version that had 100 %.

#ifndef BETA10
	LegoGameState* gameState = GameState();
	gameState->m_history.WriteScoreHistory();
#endif

	PlayMusic(JukeboxScript::c_InformationCenter_Music);

	char letterBuffer[] = "A_Bitmap";
	MxS16 i;

	for (i = 0; i < 26; i++) {
		// TODO: This might be an inline function.
		// See also `HistoryBook::ReadyWorld()`.
		if (i < 26) {
			m_alphabet[i] = (MxStillPresenter*) Find("MxStillPresenter", letterBuffer);
			assert(m_alphabet[i]);

			// We need to loop through the entire alphabet,
			// so increment the first char of the bitmap name
			letterBuffer[0]++;
		}
	}

	for (i = 0; i < sizeOfArray(m_intAlphabet); i++) {
		m_intAlphabet[i] = (MxStillPresenter*) Find("MxStillPresenter", LegoGameState::g_intCharacters[i].m_bitmap);
	}

	m_intAlphabetOffset = 0;
	for (i = 0; i < sizeOfArray(m_intAlphabet); i++) {
		if (!m_intAlphabet[i]) {
			m_intAlphabetOffset++;
		}
		else {
			break;
		}
	}

	// Now we have to do the checkmarks
	char checkmarkBuffer[] = "Check0_Ctl";
	for (i = 0; i < 10; i++) {
		m_checkmark[i] = (MxControlPresenter*) Find("MxControlPresenter", checkmarkBuffer);
		assert(m_checkmark[i]);

		// Just like in the prior letter loop,
		// we need to increment the fifth char
		// to get the next checkmark bitmap
		checkmarkBuffer[5]++;
	}

	LegoGameState::Username* players = GameState()->m_players;

	for (i = 1; i <= GameState()->m_playerCount; i++) {
		for (MxS16 j = 0; j < 7; j++) {
			if (players[i - 1].m_letters[j] != -1) {
				if (j == 0) {
					m_checkmark[i]->Enable(TRUE);
				}

				MxStillPresenter** intoAlphabet =
					[this, index = players[i - 1].m_letters[j]]() mutable -> MxStillPresenter** {
					if (index < sizeOfArray(m_alphabet)) {
						return &m_alphabet[index];
					}

					index -= sizeOfArray(m_alphabet);
					index += m_intAlphabetOffset;

					if (index >= sizeOfArray(m_intAlphabet) || !m_intAlphabet[index]) {
						SDL_Log("Warning: international character not present in current game. Falling back to X");
						return &m_alphabet[SDLK_X - SDLK_A];
					}

					return &m_intAlphabet[index];
				}();

				// Start building the player names using a two-dimensional array
				m_name[i][j] = (*intoAlphabet)->Clone();

				assert(m_name[i][j]);
				// Enable the presenter to actually show the letter in the grid
				m_name[i][j]->Enable(TRUE);

				m_name[i][j]->SetTickleState(MxPresenter::e_repeating);
				m_name[i][j]->SetPosition(23 * j + 343, 27 * i + 121);
			}
		}
	}

#ifdef BETA10
	InfocenterState* infocenterState = (InfocenterState*) GameState()->GetState("InfocenterState");
	assert(infocenterState);

	if (infocenterState->HasRegistered())
#else
	if (m_infocenterState->HasRegistered())
#endif
	{
		PlayAction(RegbookScript::c_iic008in_PlayWav);

		LegoROI* infoman = FindROI(g_infoman);
		if (infoman != NULL) {
			infoman->SetVisibility(FALSE);
		}
	}
	else {
		PlayAction(RegbookScript::c_iic006in_RunAnim);
	}
}

// FUNCTION: BETA10 0x100f3424
inline void RegistrationBook::PlayAction(MxU32 p_objectId)
{
	MxDSAction action;
	action.SetAtomId(*g_regbookScript);
	action.SetObjectId(p_objectId);

	BackgroundAudioManager()->LowerVolume();
	Start(&action);
}

// FUNCTION: LEGO1 0x10077fd0
MxResult RegistrationBook::Tickle()
{
	if (!m_worldStarted) {
		LegoWorld::Tickle();
	}
	else {
		MxLong time = Timer()->GetTime();
		if (m_registerDialogueTimer != 0x80000000 && m_registerDialogueTimer + 30000 <= time) {
			m_registerDialogueTimer = 0x80000000;
			PlayAction(RegbookScript::c_iic007in_PlayWav);
		}

		if (g_checkboxBlinkTimer + 500 <= time) {
			g_checkboxBlinkTimer = time;

			if (m_checkboxHilite) {
				DDBLTFX op;
				op.dwSize = sizeof(op);
				op.dwROP = SRCCOPY;

				if (g_nextCheckbox) {
					m_checkboxSurface->Blt(NULL, m_checkboxHilite, NULL, DDBLT_ROP, &op);
				}
				else {
					m_checkboxSurface->Blt(NULL, m_checkboxNormal, NULL, DDBLT_ROP, &op);
				}
			}
			else {
				CreateSurface();
			}

			g_nextCheckbox = !g_nextCheckbox;
		}
	}

	return SUCCESS;
}

// FUNCTION: LEGO1 0x10078180
void RegistrationBook::Enable(MxBool p_enable)
{
	LegoWorld::Enable(p_enable);

	if (p_enable) {
		InputManager()->SetWorld(this);
		SetIsWorldActive(FALSE);
	}
	else {
		if (InputManager()->GetWorld() == this) {
			InputManager()->ClearWorld();
		}
	}
}

// FUNCTION: LEGO1 0x100781d0
MxLong RegistrationBook::HandlePathStruct(LegoPathStructNotificationParam& p_param)
{
	LegoPathActor* actor = NULL;
	Act1State* act1state = (Act1State*) GameState()->GetState("Act1State");

	switch (p_param.GetData()) {
	case CopterScript::c_Helicopter_Actor:
		actor = (LegoPathActor*) Find(m_atomId, CopterScript::c_Helicopter_Actor);
		act1state->m_helicopter = (Helicopter*) actor;
		if (actor != NULL) {
			actor->SetAtomId(*g_copterScript);
			actor->SetEntityId(CopterScript::c_Helicopter_Actor);
		}
		break;
	case DunecarScript::c_DuneBugy_Actor:
		actor = (LegoPathActor*) Find(m_atomId, DunecarScript::c_DuneBugy_Actor);
		act1state->m_dunebuggy = (DuneBuggy*) actor;
		if (actor != NULL) {
			actor->SetAtomId(*g_dunecarScript);
			actor->SetEntityId(DunecarScript::c_DuneBugy_Actor);
		}
		break;
	case JetskiScript::c_Jetski_Actor:
		actor = (LegoPathActor*) Find(m_atomId, JetskiScript::c_Jetski_Actor);
		act1state->m_jetski = (Jetski*) actor;
		if (actor != NULL) {
			actor->SetAtomId(*g_jetskiScript);
			actor->SetEntityId(JetskiScript::c_Jetski_Actor);
		}
		break;
	case RacecarScript::c_RaceCar_Actor:
		actor = (LegoPathActor*) Find(m_atomId, RacecarScript::c_RaceCar_Actor);
		act1state->m_racecar = (RaceCar*) actor;
		if (actor != NULL) {
			actor->SetAtomId(*g_racecarScript);
			actor->SetEntityId(RacecarScript::c_RaceCar_Actor);
		}
		break;
	}

	if (actor == NULL) {
		NotificationManager()->Send(this, p_param);
	}
	else {
		RemoveActor(actor);
		Remove(actor);
		m_unk0x2b8--;
	}

	if (m_unk0x2b8 == 0 && !m_unk0x2c1) {
		DeleteObjects(&m_atomId, RegbookScript::c_iic006in_RunAnim, RegbookScript::c_iic008in_PlayWav);
		TransitionManager()->StartTransition(MxTransitionManager::e_mosaic, 50, FALSE, FALSE);
	}

	return 1;
}

// FUNCTION: LEGO1 0x10078350
MxBool RegistrationBook::CreateSurface()
{
	MxCompositePresenterList* presenters = m_checkmark[0]->GetList();
	MxStillPresenter* presenter;

	if (presenters) {
		if (presenters->begin() != presenters->end()) {
			presenter = (MxStillPresenter*) presenters->front();
		}
		else {
			presenter = NULL;
		}

		if (presenter) {
			m_checkboxSurface = presenter->GetSurface();
		}

		presenter = (MxStillPresenter*) Find("MxStillPresenter", "CheckHiLite_Bitmap");
		if (presenter) {
			m_checkboxHilite = presenter->GetSurface();
		}

		if (m_checkboxSurface && m_checkboxHilite) {
			m_checkboxNormal = MxDisplaySurface::CopySurface(m_checkboxSurface);
			return TRUE;
		}
	}

	return FALSE;
}

// FUNCTION: LEGO1 0x100783e0
MxBool RegistrationBook::Escape()
{
	DeleteObjects(&m_atomId, RegbookScript::c_iic006in_RunAnim, RegbookScript::c_iic008in_PlayWav);
	return TRUE;
}
