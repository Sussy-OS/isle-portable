#include "legogamestate.h"

#include "3dmanager/lego3dmanager.h"
#include "act2main_actions.h"
#include "act3_actions.h"
#include "ambulance.h"
#include "carrace.h"
#include "carrace_actions.h"
#include "carracer_actions.h"
#include "copter_actions.h"
#include "define.h"
#include "dunebuggy.h"
#include "dunecar_actions.h"
#include "elevbott_actions.h"
#include "garage_actions.h"
#include "helicopter.h"
#include "histbook_actions.h"
#include "hospital_actions.h"
#include "infocenter.h"
#include "infodoor_actions.h"
#include "infomain_actions.h"
#include "infoscor_actions.h"
#include "isle.h"
#include "isle_actions.h"
#include "islepathactor.h"
#include "jetrace_actions.h"
#include "jetracer_actions.h"
#include "jetski.h"
#include "jetski_actions.h"
#include "jetskirace.h"
#include "jukebox_actions.h"
#include "jukeboxw_actions.h"
#include "legoanimationmanager.h"
#include "legobuildingmanager.h"
#include "legocharactermanager.h"
#include "legomain.h"
#include "legonavcontroller.h"
#include "legoplantmanager.h"
#include "legostate.h"
#include "legoutils.h"
#include "legovideomanager.h"
#include "legoworld.h"
#include "misc.h"
#include "mxbackgroundaudiomanager.h"
#include "mxmisc.h"
#include "mxnotificationmanager.h"
#include "mxnotificationparam.h"
#include "mxobjectfactory.h"
#include "mxstring.h"
#include "mxutilities.h"
#include "mxvariabletable.h"
#include "pizza.h"
#include "police_actions.h"
#include "racecar.h"
#include "racecar_actions.h"
#include "regbook_actions.h"
#include "roi/legoroi.h"
#include "scripts.h"
#include "sndanim_actions.h"
#include "towtrack.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <stdio.h>

DECOMP_SIZE_ASSERT(LegoGameState::Username, 0x0e)
DECOMP_SIZE_ASSERT(LegoGameState::ScoreItem, 0x2c)
DECOMP_SIZE_ASSERT(LegoGameState::History, 0x374)
DECOMP_SIZE_ASSERT(LegoGameState, 0x430)
DECOMP_SIZE_ASSERT(ColorStringStruct, 0x08)
DECOMP_SIZE_ASSERT(LegoBackgroundColor, 0x30)
DECOMP_SIZE_ASSERT(LegoFullScreenMovie, 0x24)

// GLOBAL: LEGO1 0x100f3e40
// STRING: LEGO1 0x100f3e3c
const char* g_fileExtensionGS = ".GS";

// GLOBAL: LEGO1 0x100f3e44
// STRING: LEGO1 0x100f3e30
const char* g_playersGSI = "Players.gsi";

// GLOBAL: LEGO1 0x100f3e48
// STRING: LEGO1 0x100f3e24
const char* g_historyGSI = "History.gsi";

// This is a pointer to the end of the global variable name table, which has
// the text "END_OF_VARIABLES" in it.
// TODO: make g_endOfVariables reference the actual end of the variable array.
// GLOBAL: LEGO1 0x100f3e50
// STRING: LEGO1 0x100f3e00
// GLOBAL: BETA10 0x101ed5dc
// STRING: BETA10 0x101ed768
const char* g_endOfVariables = "END_OF_VARIABLES";

// GLOBAL: LEGO1 0x100f3e58
ColorStringStruct g_colorSaveData[43] = {
	{"c_dbbkfny0", "lego red"},    {"c_dbbkxly0", "lego white"}, // dunebuggy back fender, dunebuggy back axle
	{"c_chbasey0", "lego black"},  {"c_chbacky0", "lego black"}, // copter base, copter back
	{"c_chdishy0", "lego white"},  {"c_chhorny0", "lego black"}, // copter dish, copter horn
	{"c_chljety1", "lego black"},  {"c_chrjety1", "lego black"}, // copter left jet, copter right jet
	{"c_chmidly0", "lego black"},  {"c_chmotry0", "lego blue"},  // copter middle, copter motor
	{"c_chsidly0", "lego black"},  {"c_chsidry0", "lego black"}, // copter side left, copter side right
	{"c_chstuty0", "lego black"},  {"c_chtaily0", "lego black"}, // copter skids, copter tail
	{"c_chwindy1", "lego black"},  {"c_dbfbrdy0", "lego red"},   // copter windshield, dunebuggy body
	{"c_dbflagy0", "lego yellow"}, {"c_dbfrfny4", "lego red"},   // dunebuggy flag, dunebuggy front fender
	{"c_dbfrxly0", "lego white"},  {"c_dbhndly0", "lego white"}, // dunebuggy front axle, dunebuggy handlebar
	{"c_dbltbry0", "lego white"},  {"c_jsdashy0", "lego white"}, // dunebuggy rear lights,  jetski dash
	{"c_jsexhy0", "lego black"},   {"c_jsfrnty5", "lego black"}, // jetski exhaust, jetski front
	{"c_jshndly0", "lego red"},    {"c_jslsidy0", "lego black"}, // jetski handlebar, jetski left side
	{"c_jsrsidy0", "lego black"},  {"c_jsskiby0", "lego red"},   // jetski right side, jetski base
	{"c_jswnshy5", "lego white"},  {"c_rcbacky6", "lego green"}, // jetski windshield, racecar back
	{"c_rcedgey0", "lego green"},  {"c_rcfrmey0", "lego red"},   // racecar edge, racecar frame
	{"c_rcfrnty6", "lego green"},  {"c_rcmotry0", "lego white"}, // racecar front, racecar motor
	{"c_rcsidey0", "lego green"},  {"c_rcstery0", "lego white"}, // racecar side, racecar steering wheel
	{"c_rcstrpy0", "lego yellow"}, {"c_rctailya", "lego white"}, // racecar stripe, racecar tail
	{"c_rcwhl1y0", "lego white"},  {"c_rcwhl2y0", "lego white"}, // racecar wheels 1, racecar wheels 2
	{"c_jsbasey0", "lego white"},  {"c_chblady0", "lego black"}, // jetski base, copter blades
	{"c_chseaty0", "lego white"},                                // copter seat
};

// NOTE: This offset = the end of the variables table, the last entry
// in that table is a special entry, the string "END_OF_VARIABLES"
extern const char* g_endOfVariables;

// GLOBAL: LEGO1 0x100f3fb0
// STRING: LEGO1 0x100f3a18
const char* g_delimiter = " \t";

// GLOBAL: LEGO1 0x100f3fb4
// STRING: LEGO1 0x100f3bf0
const char* g_set = "set";

// GLOBAL: LEGO1 0x100f3fb8
// STRING: LEGO1 0x100f0cdc
const char* g_reset = "reset";

// GLOBAL: LEGO1 0x100f3fbc
// STRING: LEGO1 0x100f3be8
const char* g_strEnable = "enable";

// GLOBAL: LEGO1 0x100f3fc0
// STRING: LEGO1 0x100f3bf4
const char* g_strDisable = "disable";

const InternationalCharacter LegoGameState::g_intCharacters[8] = {
	{0xe4, "Aum_Bitmap", 29},
	{0xf6, "Oum_Bitmap", 30},
	{0xdf, "Beta_Bitmap", 31},
	{0xfc, "Uum_Bitmap", 32},
	{0xe5, "Ao_Bitmap", 29},
	{0xe6, "Ae_Bitmap", 30},
	{0xf8, "Oz_Bitmap", 31},
	{0xf1, "Ene_Bitmap", 29}
};

// FUNCTION: LEGO1 0x10039550
LegoGameState::LegoGameState()
{
	SetColors();
	SetROIColorOverride();

	m_stateCount = 0;
	m_actorId = 0;
	m_savePath = NULL;
	m_stateArray = NULL;
	m_jukeboxMusic = JukeboxScript::c_noneJukebox;
	m_currentArea = e_undefined;
	m_previousArea = e_undefined;
	m_unk0x42c = e_undefined;
	m_playerCount = 0;
	m_isDirty = FALSE;
	m_loadedAct = e_actNotFound;
	SetCurrentAct(e_act1);

	m_backgroundColor = new LegoBackgroundColor("backgroundcolor", "set 56 54 68");
	VariableTable()->SetVariable(m_backgroundColor);

	m_tempBackgroundColor = new LegoBackgroundColor("tempBackgroundColor", "set 56 54 68");
	VariableTable()->SetVariable(m_tempBackgroundColor);

	m_fullScreenMovie = new LegoFullScreenMovie("fsmovie", "disable");
	VariableTable()->SetVariable(m_fullScreenMovie);

	VariableTable()->SetVariable("lightposition", "2");
	SerializeScoreHistory(LegoFile::c_read);
}

// FUNCTION: LEGO1 0x10039720
LegoGameState::~LegoGameState()
{
	LegoROI::SetColorOverride(NULL);

	if (m_stateCount) {
		for (MxS16 i = 0; i < m_stateCount; i++) {
			LegoState* state = m_stateArray[i];
			if (state) {
				delete state;
			}
		}

		delete[] m_stateArray;
	}

	delete[] m_savePath;
}

// FUNCTION: LEGO1 0x10039780
// FUNCTION: BETA10 0x10083d43
void LegoGameState::SetActor(MxU8 p_actorId)
{
	if (p_actorId) {
		m_actorId = p_actorId;
	}

	LegoPathActor* oldActor = UserActor();
	SetUserActor(NULL);

	IslePathActor* newActor = new IslePathActor();
	const char* actorName = LegoActor::GetActorName(m_actorId);
	LegoROI* roi = CharacterManager()->GetActorROI(actorName, FALSE);
	MxDSAction action;

	action.SetAtomId(*g_isleScript);
	action.SetObjectId(100000);
	newActor->Create(action);
	newActor->SetActorId(p_actorId);
	newActor->SetROI(roi, FALSE, FALSE);

	if (oldActor) {
		newActor->GetROI()->SetLocal2World(oldActor->GetROI()->GetLocal2World());
		newActor->SetBoundary(oldActor->GetBoundary());
		delete oldActor;
	}

	newActor->ClearFlag(LegoEntity::c_managerOwned);
	SetUserActor(newActor);
}

// FUNCTION: LEGO1 0x10039910
void LegoGameState::RemoveActor()
{
	LegoPathActor* actor = UserActor();
	SetUserActor(NULL);
	delete actor;
	m_actorId = 0;
}

// FUNCTION: LEGO1 0x10039940
// FUNCTION: BETA10 0x10084069
void LegoGameState::ResetROI()
{
	if (m_actorId) {
		LegoPathActor* actor = UserActor();

		if (actor) {
			LegoROI* roi = actor->GetROI();

			if (roi) {
				VideoManager()->Get3DManager()->Remove(*roi);
				VideoManager()->Get3DManager()->Add(*roi);
			}
		}
	}
}

// FUNCTION: LEGO1 0x10039980
// FUNCTION: BETA10 0x100840e4
MxResult LegoGameState::Save(MxULong p_slot)
{
	InfocenterState* infocenterState = (InfocenterState*) GameState()->GetState("InfocenterState");

	if (!infocenterState || !infocenterState->HasRegistered()) {
		return SUCCESS;
	}

	MxResult result = FAILURE;
	LegoFile storage;
	MxVariableTable* variableTable = VariableTable();
	MxS16 count = 0;
	MxU32 i;
	MxS32 j;
	MxU16 area;

	MxString savePath;
	GetFileSavePath(&savePath, p_slot);

	if (storage.Open(savePath.GetData(), LegoFile::c_write) == FAILURE) {
		goto done;
	}

	storage.WriteS32(0x1000c);
	storage.WriteS16(m_currentPlayerId);
	storage.WriteU16(m_currentAct);
	storage.WriteU8(m_actorId);

	for (i = 0; i < sizeOfArray(g_colorSaveData); i++) {
		if (WriteVariable(&storage, variableTable, g_colorSaveData[i].m_targetName) == FAILURE) {
			goto done;
		}
	}

	if (WriteVariable(&storage, variableTable, "backgroundcolor") == FAILURE) {
		goto done;
	}
	if (WriteVariable(&storage, variableTable, "lightposition") == FAILURE) {
		goto done;
	}

	WriteEndOfVariables(&storage);
	CharacterManager()->Write(&storage);
	PlantManager()->Write(&storage);
	result = BuildingManager()->Write(&storage);

	for (j = 0; j < m_stateCount; j++) {
		if (m_stateArray[j]->IsSerializable()) {
			count++;
		}
	}

	storage.WriteS16(count);

	for (j = 0; j < m_stateCount; j++) {
		if (m_stateArray[j]->IsSerializable()) {
			m_stateArray[j]->Serialize(&storage);
		}
	}

	area = m_unk0x42c;
	storage.WriteU16(area);
	SerializeScoreHistory(LegoFile::c_write);
	m_isDirty = FALSE;

done:
	return result;
}

// FUNCTION: LEGO1 0x10039bf0
MxResult LegoGameState::DeleteState()
{
	MxS16 stateCount = m_stateCount;
	LegoState** stateArray = m_stateArray;

	m_stateCount = 0;
	m_stateArray = NULL;

	for (MxS32 count = 0; count < stateCount; count++) {
		if (!stateArray[count]->Reset() && stateArray[count]->IsSerializable()) {
			delete stateArray[count];
		}
		else {
			RegisterState(stateArray[count]);
			stateArray[count] = NULL;
		}
	}

	delete[] stateArray;
	return SUCCESS;
}

// FUNCTION: LEGO1 0x10039c60
// FUNCTION: BETA10 0x10084329
MxResult LegoGameState::Load(MxULong p_slot)
{
	MxResult result = FAILURE;
	LegoFile storage;
	MxVariableTable* variableTable = VariableTable();

	MxString savePath;
	GetFileSavePath(&savePath, p_slot);

	if (storage.Open(savePath.GetData(), LegoFile::c_read) == FAILURE) {
		goto done;
	}

	MxS32 version;
	MxU32 status;
	MxS16 count, actArea;
	const char* lightPosition;

	storage.ReadS32(version);

	assert(version == 0x1000c);
	if (version != 0x1000c) {
		OmniError("Saved game version mismatch", 0);
		goto done;
	}

	storage.ReadS16(m_currentPlayerId);
	storage.ReadS16(actArea);

	SetCurrentAct((Act) actArea);
	storage.ReadU8(m_actorId);
	if (m_actorId) {
		SetActor(m_actorId);
	}

	do {
		status = ReadVariable(&storage, variableTable);
		if (status == 1) {
			goto done;
		}
	} while (status != 2);

	m_backgroundColor->SetLightColor();
	lightPosition = VariableTable()->GetVariable("lightposition");

	if (lightPosition) {
		SetLightPosition(atoi(lightPosition));
	}

	if (CharacterManager()->Read(&storage) == FAILURE) {
		goto done;
	}
	if (PlantManager()->Read(&storage) == FAILURE) {
		goto done;
	}
	if (BuildingManager()->Read(&storage) == FAILURE) {
		goto done;
	}
	if (DeleteState() != SUCCESS) {
		goto done;
	}

	char stateName[80];
	storage.ReadS16(count);

	if (count) {
		for (MxS16 i = 0; i < count; i++) {
			storage.ReadString(stateName);

			LegoState* state = GetState(stateName);
			if (!state) {
				state = CreateState(stateName);

				if (!state) {
					goto done;
				}
			}

			state->Serialize(&storage);
		}
	}

	storage.ReadS16(actArea);

	if (m_currentAct == e_act1) {
		m_unk0x42c = e_undefined;
	}
	else {
		m_unk0x42c = (Area) actArea;
	}

	result = SUCCESS;
	m_isDirty = FALSE;

done:
	if (result != SUCCESS) {
		OmniError("Game state loading was not successful!", 0);
	}

	return result;
}

// FUNCTION: LEGO1 0x10039f00
void LegoGameState::SetSavePath(char* p_savePath)
{
	if (m_savePath != NULL) {
		delete[] m_savePath;
	}

	if (p_savePath) {
		m_savePath = new char[strlen(p_savePath) + 1];
		strcpy(m_savePath, p_savePath);
	}
	else {
		m_savePath = NULL;
	}
}

// FUNCTION: LEGO1 0x10039f70
// FUNCTION: BETA10 0x1008483b
MxResult LegoGameState::WriteVariable(LegoStorage* p_storage, MxVariableTable* p_from, const char* p_variableName)
{
	MxResult result = FAILURE;
	const char* variableValue = p_from->GetVariable(p_variableName);

	if (variableValue) {
		MxU8 length = strlen(p_variableName);
		if (p_storage->Write(&length, sizeof(length)) != SUCCESS) {
			goto done;
		}

		if (p_storage->Write(p_variableName, length) != SUCCESS) {
			goto done;
		}

		length = strlen(variableValue);
		if (p_storage->Write(&length, sizeof(length)) != SUCCESS) {
			goto done;
		}

		result = p_storage->Write(variableValue, length);
	}

done:
	return result;
}

// FUNCTION: LEGO1 0x1003a020
// FUNCTION: BETA10 0x10084928
MxResult LegoGameState::WriteEndOfVariables(LegoStorage* p_storage)
{
	MxU8 len = strlen(g_endOfVariables);

	if (p_storage->Write(&len, sizeof(len)) == SUCCESS) {
		return p_storage->Write(g_endOfVariables, len);
	}

	return FAILURE;
}

// FUNCTION: LEGO1 0x1003a080
// FUNCTION: BETA10 0x1008498b
MxS32 LegoGameState::ReadVariable(LegoStorage* p_storage, MxVariableTable* p_to)
{
	MxS32 result = 1;
	MxU8 len;

	if (p_storage->Read(&len, sizeof(MxU8)) != SUCCESS) {
		goto done;
	}

	char varName[256];
	assert(len < sizeof(varName));

	if (p_storage->Read(varName, len) != SUCCESS) {
		goto done;
	}

	varName[len] = '\0';
	if (strcmp(varName, g_endOfVariables) == 0) {
		// 2 -> "This was the last entry, done reading."
		result = 2;
		goto done;
	}

	if (p_storage->Read(&len, sizeof(MxU8)) != SUCCESS) {
		goto done;
	}

	char value[256];
	assert(len < sizeof(value));

	if (p_storage->Read(value, len) != SUCCESS) {
		goto done;
	}

	value[len] = '\0';
	p_to->SetVariable(varName, value);
	result = SUCCESS;

done:
	return result;
}

// FUNCTION: LEGO1 0x1003a170
// FUNCTION: BETA10 0x10084b45
void LegoGameState::GetFileSavePath(MxString* p_outPath, MxS16 p_slotn)
{
	char baseForSlot[2] = "0";
	char path[1024] = "";

	// Save path base
	if (m_savePath != NULL) {
		strcpy(path, m_savePath);
	}

	// Slot: "G0", "G1", ...
	strcat(path, "\\G");
	baseForSlot[0] += p_slotn;
	strcat(path, baseForSlot);

	// Extension: ".GS"
	strcat(path, g_fileExtensionGS);
	*p_outPath = MxString(path);
	p_outPath->MapPathToFilesystem();
}

// FUNCTION: LEGO1 0x1003a2e0
void LegoGameState::SerializePlayersInfo(MxS16 p_flags)
{
	LegoFile storage;
	MxString playersGSI = MxString(m_savePath);

	playersGSI += "\\";
	playersGSI += g_playersGSI;

	if (storage.Open(playersGSI.GetData(), p_flags) == SUCCESS) {
		if (storage.IsReadMode()) {
			storage.ReadS16(m_playerCount);
		}
		else if (storage.IsWriteMode()) {
			storage.WriteS16(m_playerCount);
		}

		for (MxS16 i = 0; i < m_playerCount; i++) {
			m_players[i].Serialize(&storage);
		}
	}
}

// FUNCTION: LEGO1 0x1003a3f0
MxResult LegoGameState::AddPlayer(Username& p_player)
{
	MxString from, to;

	if (m_playerCount == 9) {
		GetFileSavePath(&from, 8);
		SDL_RemovePath(from.GetData());
		m_playerCount--;
	}

	for (MxS16 i = m_playerCount; i > 0; i--) {
		m_players[i] = m_players[i - 1];
		GetFileSavePath(&from, i - 1);
		GetFileSavePath(&to, i);
		SDL_RenamePath(from.GetData(), to.GetData());
	}

	m_playerCount++;
	m_players[0].Set(p_player);
	m_currentPlayerId = m_history.m_nextPlayerId;
	m_history.m_nextPlayerId = m_currentPlayerId + 1;
	m_history.WriteScoreHistory();
	SetCurrentAct(e_act1);

	return DeleteState();
}

// FUNCTION: LEGO1 0x1003a540
// FUNCTION: BETA10 0x10084fc4
void LegoGameState::SwitchPlayer(MxS16 p_playerId)
{
	if (p_playerId > 0) {
		MxString from, temp, to;

		GetFileSavePath(&from, p_playerId);
		GetFileSavePath(&temp, 36);

		Username selectedName(m_players[p_playerId]);

		SDL_RenamePath(from.GetData(), temp.GetData());

		for (MxS16 i = p_playerId; i > 0; i--) {
			m_players[i] = m_players[i - 1];
			GetFileSavePath(&from, i - 1);
			GetFileSavePath(&to, i);
			SDL_RenamePath(from.GetData(), to.GetData());
		}

		m_players[0] = selectedName;
		GetFileSavePath(&from, 0);
		SDL_RenamePath(temp.GetData(), from.GetData());
	}

	if (Load(0) != SUCCESS) {
		Init();
	}
}

// FUNCTION: LEGO1 0x1003a6e0
MxS16 LegoGameState::FindPlayer(Username& p_player)
{
	for (MxS16 i = 0; i < m_playerCount; i++) {
		if (memcmp(&m_players[i], &p_player, sizeof(p_player)) == 0) {
			return i;
		}
	}

	return -1;
}

// FUNCTION: LEGO1 0x1003a720
// FUNCTION: BETA10 0x10085211
void LegoGameState::StopArea(Area p_area)
{
	if (p_area == e_previousArea) {
		p_area = m_previousArea;
	}

	switch (p_area) {
	case e_isle:
		InvokeAction(Extra::e_stop, *g_isleScript, IsleScript::c__Isle, NULL);
		InvokeAction(Extra::e_close, *g_isleScript, IsleScript::c__Isle, NULL);
		InvokeAction(Extra::e_close, *g_sndAnimScript, SndanimScript::c_SoundAndAnim_Action, NULL);
		break;
	case e_infomain:
		InvokeAction(Extra::e_stop, *g_infomainScript, InfomainScript::c__InfoMain, NULL);
		InvokeAction(Extra::e_close, *g_infomainScript, InfomainScript::c__InfoMain, NULL);
		break;
	case e_infodoor:
		InvokeAction(Extra::e_stop, *g_infodoorScript, InfodoorScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_infodoorScript, InfodoorScript::c__StartUp, NULL);
		break;
	case e_elevbott:
		InvokeAction(Extra::e_stop, *g_elevbottScript, ElevbottScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_elevbottScript, ElevbottScript::c__StartUp, NULL);
		break;
	case e_elevride:
	case e_elevride2:
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevRide_Background_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevRide_Info_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevRide_Two_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevRide_Three_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Meter1_3_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Meter2_3_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Meter3_1_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Meter3_2_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Meter2_1_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Meter1_2_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Meter3_Bitmap, *g_isleScript, IsleScript::c__Isle);
		break;
	case e_elevopen:
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevOpen_Background_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevOpen_LeftArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevOpen_RightArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		break;
	case e_seaview:
		RemoveFromWorld(*g_isleScript, IsleScript::c_SeaView_Background_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_SeaView_LeftArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_SeaView_RightArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		break;
	case e_observe:
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Background_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_LeftArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_RightArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Plane_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Sun_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Moon_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_SkyColor_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_LCab_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_RCab_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_GlobeRArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_GlobeLArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Globe1_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Globe2_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Globe3_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Globe4_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Globe5_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Globe6_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Draw1_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Observe_Draw2_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_Radio_Ctl, *g_isleScript, IsleScript::c__Isle);
		break;
	case e_elevdown:
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevDown_Background_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevDown_LeftArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevDown_RightArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_ElevDown_Elevator_Ctl, *g_isleScript, IsleScript::c__Isle);
		break;
	case e_regbook:
		InvokeAction(Extra::e_stop, *g_regbookScript, RegbookScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_regbookScript, RegbookScript::c__StartUp, NULL);
		break;
	case e_infoscor:
		InvokeAction(Extra::e_stop, *g_infoscorScript, InfoscorScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_infoscorScript, InfoscorScript::c__StartUp, NULL);
		break;
	case e_jetrace:
		InvokeAction(Extra::e_stop, *g_jetraceScript, JetraceScript::c__JetRace_World, NULL);
		InvokeAction(Extra::e_close, *g_jetraceScript, JetraceScript::c__JetRace_World, NULL);
		InvokeAction(Extra::e_close, *g_jetracerScript, 0, NULL);
		break;
	case e_carrace:
		InvokeAction(Extra::e_stop, *g_carraceScript, CarraceScript::c__CarRace_World, NULL);
		InvokeAction(Extra::e_close, *g_carraceScript, CarraceScript::c__CarRace_World, NULL);
		InvokeAction(Extra::e_close, *g_carracerScript, CarracerScript::c_nrt002pz_Anim, NULL);
		break;
	case e_garage:
		Lego()->RemoveWorld(*g_garageScript, 0);
		InvokeAction(Extra::e_stop, *g_garageScript, GarageScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_garageScript, GarageScript::c__StartUp, NULL);
		break;
	case e_garadoor:
		RemoveFromWorld(*g_isleScript, IsleScript::c_GaraDoor_Background_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_GaraDoor_LeftArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_GaraDoor_RightArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_GaraDoor_Door_Ctl, *g_isleScript, IsleScript::c__Isle);
		break;
	case e_hospital:
		InvokeAction(Extra::e_stop, *g_hospitalScript, HospitalScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_hospitalScript, HospitalScript::c__StartUp, NULL);
		break;
	case e_police:
		InvokeAction(Extra::e_stop, *g_policeScript, PoliceScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_policeScript, PoliceScript::c__StartUp, NULL);
		break;
	case e_polidoor:
		RemoveFromWorld(*g_isleScript, IsleScript::c_PoliDoor_Background_Bitmap, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_PoliDoor_LeftArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_PoliDoor_RightArrow_Ctl, *g_isleScript, IsleScript::c__Isle);
		RemoveFromWorld(*g_isleScript, IsleScript::c_PoliDoor_Door_Ctl, *g_isleScript, IsleScript::c__Isle);
		break;
	case e_copterbuild:
		InvokeAction(Extra::e_stop, *g_jukeboxScript, JukeboxScript::c_HelicopterBuild_Movie, NULL);
		InvokeAction(Extra::e_stop, *g_copterScript, CopterScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_copterScript, CopterScript::c__StartUp, NULL);
		break;
	case e_dunecarbuild:
		InvokeAction(Extra::e_stop, *g_jukeboxScript, JukeboxScript::c_DuneCarBuild_Movie, NULL);
		InvokeAction(Extra::e_stop, *g_dunecarScript, DunecarScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_dunecarScript, DunecarScript::c__StartUp, NULL);
		break;
	case e_jetskibuild:
		InvokeAction(Extra::e_stop, *g_jukeboxScript, JukeboxScript::c_JetskiBuild_Movie, NULL);
		InvokeAction(Extra::e_stop, *g_jetskiScript, JetskiScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_jetskiScript, JetskiScript::c__StartUp, NULL);
		break;
	case e_racecarbuild:
		InvokeAction(Extra::e_stop, *g_jukeboxScript, JukeboxScript::c_RaceCarBuild_Movie, NULL);
		InvokeAction(Extra::e_stop, *g_racecarScript, RacecarScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_racecarScript, RacecarScript::c__StartUp, NULL);
		break;
	case e_act2main:
		if (m_currentArea != e_infomain) {
			InvokeAction(Extra::e_stop, *g_act2mainScript, Act2mainScript::c__Act2Main, NULL);
			InvokeAction(Extra::e_close, *g_act2mainScript, Act2mainScript::c__Act2Main, NULL);
		}
		break;
	case e_act3script:
		if (m_currentArea != e_infomain) {
			InvokeAction(Extra::e_stop, *g_act3Script, Act3Script::c__Act3, NULL);
			InvokeAction(Extra::e_close, *g_act3Script, Act3Script::c__Act3, NULL);
		}
		break;
	case e_jukeboxw:
		InvokeAction(Extra::e_stop, *g_jukeboxwScript, JukeboxwScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_jukeboxwScript, JukeboxwScript::c__StartUp, NULL);
		break;
	case e_histbook:
		InvokeAction(Extra::e_disable, *g_histbookScript, HistbookScript::c__StartUp, NULL);
		InvokeAction(Extra::e_stop, *g_histbookScript, HistbookScript::c__StartUp, NULL);
		InvokeAction(Extra::e_close, *g_histbookScript, HistbookScript::c__StartUp, NULL);
		break;
	}
}

inline void LoadIsle()
{
	LegoWorld* world = FindWorld(*g_isleScript, IsleScript::c__Isle);
	if (world != NULL) {
		if (!world->GetUnknown0xd0Empty()) {
			NotificationManager()->Send(world, MxNotificationParam(c_notificationType20, NULL));
		}
	}
	else {
		InvokeAction(Extra::ActionType::e_opendisk, *g_isleScript, IsleScript::c__Isle, NULL);
	}
}

// FUNCTION: LEGO1 0x1003b060
// FUNCTION: BETA10 0x10085adf
void LegoGameState::SwitchArea(Area p_area)
{
	m_previousArea = m_currentArea;
	m_currentArea = p_area;

	Disable(TRUE, LegoOmni::c_disableInput | LegoOmni::c_disable3d);
	BackgroundAudioManager()->Stop();
	AnimationManager()->Suspend();
	VideoManager()->SetUnk0x554(FALSE);

	switch (p_area) {
	case e_isle:
		InvokeAction(Extra::ActionType::e_opendisk, *g_isleScript, IsleScript::c__Isle, NULL);
		break;
	case e_infomain:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_infomainScript, InfomainScript::c__InfoMain, NULL);
		break;
	case e_infodoor:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_infodoorScript, InfodoorScript::c__StartUp, NULL);
		break;
	case e_infocenterExited:
	case e_jetrace2:
	case e_jetraceExterior:
	case e_jetskibuildExited:
	case e_carraceExterior:
	case e_racecarbuildExited:
	case e_unk21:
	case e_pizzeriaExterior:
	case e_garageExterior:
	case e_hospitalExterior:
	case e_hospitalExited:
	case e_policeExterior:
	case e_bike:
	case e_dunecar:
	case e_motocycle:
	case e_copter:
	case e_skateboard:
	case e_jetski:
	case e_vehicleExited:
		LoadIsle();
		break;
	case e_elevbott:
		InvokeAction(Extra::ActionType::e_opendisk, *g_elevbottScript, ElevbottScript::c__StartUp, NULL);
		break;
	case e_elevride:
	case e_elevride2:
		LoadIsle();
		InvokeAction(Extra::ActionType::e_start, *g_isleScript, IsleScript::c_ElevRide, NULL);
		break;
	case e_elevopen:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_start, *g_isleScript, IsleScript::c_ElevOpen, NULL);
		break;
	case e_seaview:
		InvokeAction(Extra::ActionType::e_start, *g_isleScript, IsleScript::c_SeaView, NULL);
		break;
	case e_observe:
		InvokeAction(Extra::ActionType::e_start, *g_isleScript, IsleScript::c_Observe, NULL);
		break;
	case e_elevdown:
		InvokeAction(Extra::ActionType::e_start, *g_isleScript, IsleScript::c_ElevDown, NULL);
		break;
	case e_regbook:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_regbookScript, RegbookScript::c__StartUp, NULL);
		break;
	case e_infoscor:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_infoscorScript, InfoscorScript::c__StartUp, NULL);
		break;
	case e_jetrace:
		if (m_previousArea == e_infomain) {
			m_currentArea = e_jetrace2;
			LoadIsle();
		}
		else {
			InvokeAction(Extra::ActionType::e_opendisk, *g_jetraceScript, JetraceScript::c__JetRace_World, NULL);
		}
		break;
	case e_carrace:
		if (m_previousArea == e_infomain) {
			m_currentArea = e_carraceExterior;
			LoadIsle();
		}
		else {
			InvokeAction(Extra::ActionType::e_opendisk, *g_carraceScript, CarraceScript::c__CarRace_World, NULL);
		}
		break;
	case e_garage:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_garageScript, GarageScript::c__StartUp, NULL);
		break;
	case e_garadoor:
		LoadIsle();
		VariableTable()->SetVariable("VISIBILITY", "Hide Gas");
		UserActor()->ResetWorldTransform(FALSE);
		NavController()->UpdateLocation(59); // LCAMZG1 in g_cameraLocations
		VideoManager()->Get3DManager()->SetFrustrum(90, 0.1f, 250.0f);
		InvokeAction(Extra::ActionType::e_start, *g_isleScript, IsleScript::c_GaraDoor, NULL);
		break;
	case e_garageExited: {
		Act1State* state = (Act1State*) GameState()->GetState("Act1State");
		LoadIsle();

		if (state->GetState() == Act1State::e_transitionToTowtrack) {
			VideoManager()->Get3DManager()->SetFrustrum(90, 0.1f, 250.0f);
		}
		else {
			SetCameraControllerFromIsle();
			UserActor()->ResetWorldTransform(TRUE);
			AnimationManager()->Resume();
		}

		((IslePathActor*) UserActor())
			->SpawnPlayer(
				p_area,
				TRUE,
				IslePathActor::c_spawnBit1 | IslePathActor::c_playMusic | IslePathActor::c_spawnBit3
			);
		break;
	}
	case e_hospital:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_hospitalScript, HospitalScript::c__StartUp, NULL);
		break;
	case e_policeExited:
		LoadIsle();
		SetCameraControllerFromIsle();
		UserActor()->ResetWorldTransform(TRUE);
		AnimationManager()->Resume();
		((IslePathActor*) UserActor())
			->SpawnPlayer(
				p_area,
				TRUE,
				IslePathActor::c_spawnBit1 | IslePathActor::c_playMusic | IslePathActor::c_spawnBit3
			);
		break;
	case e_police:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_policeScript, PoliceScript::c__StartUp, NULL);
		break;
	case e_polidoor:
		LoadIsle();
		InvokeAction(Extra::ActionType::e_start, *g_isleScript, IsleScript::c_PoliDoor, NULL);
		break;
	case e_copterbuild:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_copterScript, CopterScript::c__StartUp, NULL);
		break;
	case e_dunecarbuild:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_dunecarScript, DunecarScript::c__StartUp, NULL);
		break;
	case e_jetskibuild:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_jetskiScript, JetskiScript::c__StartUp, NULL);
		break;
	case e_racecarbuild:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_racecarScript, RacecarScript::c__StartUp, NULL);
		break;
	case e_act2main: {
		LegoWorld* act2main = FindWorld(*g_act2mainScript, Act2mainScript::c__Act2Main);

		if (act2main == NULL) {
			InvokeAction(Extra::ActionType::e_opendisk, *g_act2mainScript, Act2mainScript::c__Act2Main, NULL);
		}
		else {
			act2main->Enable(TRUE);
		}

		break;
	}
	case e_act3script: {
		LegoWorld* act3 = FindWorld(*g_act3Script, Act3Script::c__Act3);

		if (act3 == NULL) {
			InvokeAction(Extra::ActionType::e_opendisk, *g_act3Script, Act3Script::c__Act3, NULL);
		}
		else {
			act3->Enable(TRUE);
		}

		break;
	}
	case e_jukeboxw:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_jukeboxwScript, JukeboxwScript::c__StartUp, NULL);
		break;
	case e_jukeboxExterior:
		LoadIsle();
		break;
	case e_histbook:
		VideoManager()->SetUnk0x554(TRUE);
		InvokeAction(Extra::ActionType::e_opendisk, *g_histbookScript, HistbookScript::c__StartUp, NULL);
		break;
	default:
		break;
	}
}

// FUNCTION: LEGO1 0x1003ba90
void LegoGameState::SetColors()
{
	MxVariableTable* variableTable = VariableTable();

	for (MxS32 i = 0; i < sizeOfArray(g_colorSaveData); i++) {
		variableTable->SetVariable(g_colorSaveData[i].m_targetName, g_colorSaveData[i].m_colorName);
	}
}

// FUNCTION: LEGO1 0x1003bac0
void LegoGameState::SetROIColorOverride()
{
	LegoROI::SetColorOverride(&ROIColorOverride);
}

// FUNCTION: LEGO1 0x1003bad0
MxBool ROIColorOverride(const char* p_input, char* p_output, MxU32 p_copyLen)
{
	if (p_output != NULL && p_copyLen != 0 &&
		(SDL_strncasecmp(p_input, "INDIR-F-", strlen("INDIR-F-")) == 0 ||
		 SDL_strncasecmp(p_input, "INDIR-G-", strlen("INDIR-F-")) == 0)) {

		char buf[256];
		sprintf(buf, "c_%s", &p_input[strlen("INDIR-F-")]);

		const char* value = VariableTable()->GetVariable(buf);
		if (value != NULL) {
			strncpy(p_output, value, p_copyLen);
			p_output[p_copyLen - 1] = '\0';
			return TRUE;
		}
	}

	return FALSE;
}

// FUNCTION: LEGO1 0x1003bbb0
// FUNCTION: BETA10 0x10086280
LegoState* LegoGameState::GetState(const char* p_stateName)
{
	for (MxS32 i = 0; i < m_stateCount; ++i) {
		if (m_stateArray[i]->IsA(p_stateName)) {
			return m_stateArray[i];
		}
	}
	return NULL;
}

// FUNCTION: LEGO1 0x1003bc00
// FUNCTION: BETA10 0x100862fc
LegoState* LegoGameState::CreateState(const char* p_stateName)
{
	// variable name verified by BETA10 0x10086341
	LegoState* state = (LegoState*) ObjectFactory()->Create(p_stateName);
	assert(state);

	RegisterState(state);

	return state;
}

// FUNCTION: LEGO1 0x1003bc30
// FUNCTION: BETA10 0x1008636e
void LegoGameState::RegisterState(LegoState* p_state)
{
	MxS32 targetIndex;
	for (targetIndex = 0; targetIndex < m_stateCount; ++targetIndex) {
		if (m_stateArray[targetIndex]->IsA(p_state->ClassName())) {
			break;
		}
	}

	if (targetIndex == m_stateCount) {
		LegoState** newBuffer = new LegoState*[m_stateCount + 1];

		if (m_stateCount != 0) {
			memcpy(newBuffer, m_stateArray, m_stateCount * sizeof(LegoState*));
			delete[] m_stateArray;
		}

		newBuffer[m_stateCount++] = p_state;
		m_stateArray = newBuffer;
	}
	else {
		delete m_stateArray[targetIndex];
		m_stateArray[targetIndex] = p_state;
	}
}

// FUNCTION: LEGO1 0x1003bd00
void LegoGameState::Init()
{
	m_backgroundColor->SetValue("set 56 54 68");
	m_backgroundColor->SetLightColor();
	m_tempBackgroundColor->SetValue("set 56 54 68");
	VariableTable()->SetVariable("lightposition", "2");
	SetLightPosition(2);
	PlantManager()->Init();
	BuildingManager()->Init();
	CharacterManager()->Init();
	AnimationManager()->Reset(TRUE);
	SetColors();
	RemoveActor();
	DeleteState();
	m_isDirty = FALSE;
	FindLoadedAct();
	SetCurrentAct(e_act1);

	if (m_loadedAct == e_act1) {
		Isle* isle = (Isle*) FindWorld(*g_isleScript, IsleScript::c__Isle);

		Helicopter* copter = (Helicopter*) isle->Find(*g_copterScript, CopterScript::c_Helicopter_Actor);
		if (copter) {
			isle->RemoveActor(copter);
			isle->RemoveVehicle(copter);
			delete copter;
		}

		DuneBuggy* dunebuggy = (DuneBuggy*) isle->Find(*g_dunecarScript, DunecarScript::c_DuneBugy_Actor);
		if (dunebuggy) {
			isle->RemoveActor(dunebuggy);
			isle->RemoveVehicle(dunebuggy);
			delete dunebuggy;
		}

		Jetski* jetski = (Jetski*) isle->Find(*g_jetskiScript, JetskiScript::c_Jetski_Actor);
		if (jetski) {
			isle->RemoveActor(jetski);
			isle->RemoveVehicle(jetski);
			delete jetski;
		}

		RaceCar* racecar = (RaceCar*) isle->Find(*g_racecarScript, RacecarScript::c_RaceCar_Actor);
		if (racecar) {
			isle->RemoveActor(racecar);
			isle->RemoveVehicle(racecar);
			delete racecar;
		}
	}

	m_unk0x42c = e_undefined;
}

// FUNCTION: BETA10 0x10086510
LegoBackgroundColor::LegoBackgroundColor()
{
	m_h = 0.0f;
	m_s = 0.0f;
	m_v = 0.0f;
}

// FUNCTION: LEGO1 0x1003bfb0
// FUNCTION: BETA10 0x1008659d
LegoBackgroundColor::LegoBackgroundColor(const char* p_key, const char* p_value)
{
	m_key = p_key;
	m_key.ToUpperCase();
	SetValue(p_value);
}

// FUNCTION: LEGO1 0x1003c070
// FUNCTION: BETA10 0x10086634
void LegoBackgroundColor::SetValue(const char* p_colorString)
{
	m_value = p_colorString;
	m_value.ToLowerCase();

	LegoVideoManager* videomanager = VideoManager();
	if (!videomanager || !p_colorString) {
		return;
	}

	float convertedR, convertedG, convertedB;
	char* colorStringCopy = strcpy(new char[strlen(p_colorString) + 1], p_colorString);
	char* colorStringSplit = strtok(colorStringCopy, g_delimiter);

	if (!strcmp(colorStringSplit, g_set)) {
		colorStringSplit = strtok(0, g_delimiter);
		if (colorStringSplit) {
			m_h = (float) (atoi(colorStringSplit) * 0.01);
		}
		colorStringSplit = strtok(0, g_delimiter);
		if (colorStringSplit) {
			m_s = (float) (atoi(colorStringSplit) * 0.01);
		}
		colorStringSplit = strtok(0, g_delimiter);
		if (colorStringSplit) {
			m_v = (float) (atoi(colorStringSplit) * 0.01);
		}

		ConvertHSVToRGB(m_h, m_s, m_v, &convertedR, &convertedG, &convertedB);
		videomanager->SetSkyColor(convertedR, convertedG, convertedB);
	}
	else if (!strcmp(colorStringSplit, g_reset)) {
		ConvertHSVToRGB(m_h, m_s, m_v, &convertedR, &convertedG, &convertedB);
		videomanager->SetSkyColor(convertedR, convertedG, convertedB);
	}

	delete[] colorStringCopy;
}

// FUNCTION: LEGO1 0x1003c230
// FUNCTION: BETA10 0x100867f9
void LegoBackgroundColor::ToggleDayNight(MxBool p_sun)
{
	char buffer[30];

	if (p_sun) {
		m_s += 0.1;
		if (m_s > 0.9) {
			m_s = 1.0;
		}
	}
	else {
		m_s -= 0.1;
		if (m_s < 0.1) {
			m_s = 0.1;
		}
	}

	sprintf(buffer, "set %d %d %d", (MxU32) (m_h * 100.0f), (MxU32) (m_s * 100.0f), (MxU32) (m_v * 100.0f));
	m_value = buffer;

	float convertedR, convertedG, convertedB;
	ConvertHSVToRGB(m_h, m_s, m_v, &convertedR, &convertedG, &convertedB);
	VideoManager()->SetSkyColor(convertedR, convertedG, convertedB);
	SetLightColor(convertedR, convertedG, convertedB);
}

// FUNCTION: LEGO1 0x1003c330
// FUNCTION: BETA10 0x100868de
void LegoBackgroundColor::ToggleSkyColor()
{
	char buffer[30];

	m_h += 0.05;
	if (m_h > 1.0) {
		m_h -= 1.0;
	}

	sprintf(buffer, "set %d %d %d", (MxU32) (m_h * 100.0f), (MxU32) (m_s * 100.0f), (MxU32) (m_v * 100.0f));
	m_value = buffer;

	float convertedR, convertedG, convertedB;
	ConvertHSVToRGB(m_h, m_s, m_v, &convertedR, &convertedG, &convertedB);
	VideoManager()->SetSkyColor(convertedR, convertedG, convertedB);
	SetLightColor(convertedR, convertedG, convertedB);
}

// FUNCTION: LEGO1 0x1003c400
// FUNCTION: BETA10 0x10086984
void LegoBackgroundColor::SetLightColor(float p_r, float p_g, float p_b)
{
	if (!VideoManager()->GetVideoParam().Flags().GetLacksLightSupport()) {
		// TODO: Computed constants based on what?
		p_r *= 1. / 0.23;
		p_g *= 1. / 0.63;
		p_b *= 1. / 0.85;

		if (p_r > 1.0) {
			p_r = 1.0;
		}

		if (p_g > 1.0) {
			p_g = 1.0;
		}

		if (p_b > 1.0) {
			p_b = 1.0;
		}

		VideoManager()->Get3DManager()->GetLego3DView()->SetLightColor(FALSE, p_r, p_g, p_b);
		VideoManager()->Get3DManager()->GetLego3DView()->SetLightColor(TRUE, p_r, p_g, p_b);
	}
}

// FUNCTION: LEGO1 0x1003c4b0
void LegoBackgroundColor::SetLightColor()
{
	float convertedR, convertedG, convertedB;
	ConvertHSVToRGB(m_h, m_s, m_v, &convertedR, &convertedG, &convertedB);
	SetLightColor(convertedR, convertedG, convertedB);
}

// FUNCTION: BETA10 0x10086a87
LegoFullScreenMovie::LegoFullScreenMovie()
{
}

// FUNCTION: LEGO1 0x1003c500
// FUNCTION: BETA10 0x10086af6
LegoFullScreenMovie::LegoFullScreenMovie(const char* p_key, const char* p_value)
{
	m_key = p_key;
	m_key.ToUpperCase();
	SetValue(p_value);
}

// FUNCTION: LEGO1 0x1003c5c0
// FUNCTION: BETA10 0x10086b8d
void LegoFullScreenMovie::SetValue(const char* p_option)
{
	m_value = p_option;
	m_value.ToLowerCase();

	LegoVideoManager* videomanager = VideoManager();
	if (videomanager) {
		if (!strcmp(m_value.GetData(), g_strEnable)) {
			videomanager->EnableFullScreenMovie(TRUE);
		}
		else if (!strcmp(m_value.GetData(), g_strDisable)) {
			videomanager->EnableFullScreenMovie(FALSE);
		}
	}
}

// FUNCTION: LEGO1 0x1003c670
LegoGameState::Username::Username()
{
	memset(m_letters, -1, sizeof(m_letters));
}

// FUNCTION: LEGO1 0x1003c690
// FUNCTION: BETA10 0x10086c57
MxResult LegoGameState::Username::Serialize(LegoStorage* p_storage)
{
	if (p_storage->IsReadMode()) {
		for (MxS16 i = 0; i < (MxS16) sizeOfArray(m_letters); i++) {
			p_storage->ReadS16(m_letters[i]);
		}
	}
	else if (p_storage->IsWriteMode()) {
		for (MxS16 i = 0; i < (MxS16) sizeOfArray(m_letters); i++) {
			p_storage->WriteS16(m_letters[i]);
		}
	}

	return SUCCESS;
}

// FUNCTION: LEGO1 0x1003c710
// FUNCTION: BETA10 0x10086d0c
LegoGameState::Username& LegoGameState::Username::operator=(const Username& p_other)
{
	memcpy(m_letters, p_other.m_letters, sizeof(m_letters));
	return *this;
}

// FUNCTION: LEGO1 0x1003c740
// FUNCTION: BETA10 0x10086d39
MxResult LegoGameState::ScoreItem::Serialize(LegoStorage* p_storage)
{
	if (p_storage->IsReadMode()) {
		p_storage->ReadS16(m_totalScore);

		for (MxS32 i = 0; i < 5; i++) {
			for (MxS32 j = 0; j < 5; j++) {
				p_storage->ReadU8(m_scores[i][j]);
			}
		}

		m_name.Serialize(p_storage);
		p_storage->ReadS16(m_playerId);
	}
	else if (p_storage->IsWriteMode()) {
		p_storage->WriteS16(m_totalScore);

		for (MxS32 i = 0; i < 5; i++) {
			for (MxS32 j = 0; j < 5; j++) {
				p_storage->WriteU8(m_scores[i][j]);
			}
		}

		m_name.Serialize(p_storage);
		p_storage->WriteS16(m_playerId);
	}

	return SUCCESS;
}

// FUNCTION: LEGO1 0x1003c830
// FUNCTION: BETA10 0x10086e87
LegoGameState::History::History()
{
	m_count = 0;
	m_nextPlayerId = 0;
}

// FUNCTION: LEGO1 0x1003c870
// FUNCTION: BETA10 0x10086ec9
void LegoGameState::History::WriteScoreHistory()
{
	MxS16 totalScore = 0;
	MxU8 scores[5][5];

	InfocenterState* state = (InfocenterState*) GameState()->GetState("InfocenterState");

	if (!state->m_letters[0]) {
		return;
	}

	JetskiRaceState* jetskiRaceState = (JetskiRaceState*) GameState()->GetState("JetskiRaceState");
	CarRaceState* carRaceState = (CarRaceState*) GameState()->GetState("CarRaceState");
	TowTrackMissionState* towTrackMissionState = (TowTrackMissionState*) GameState()->GetState("TowTrackMissionState");
	PizzaMissionState* pizzaMissionState = (PizzaMissionState*) GameState()->GetState("PizzaMissionState");
	AmbulanceMissionState* ambulanceMissionState =
		(AmbulanceMissionState*) GameState()->GetState("AmbulanceMissionState");

	for (MxS32 actor = 1; actor <= 5; actor++) {
		scores[0][actor - 1] = carRaceState ? carRaceState->GetState(actor)->GetHighScore() : 0;
		totalScore += scores[0][actor - 1];

#ifdef BETA10
		// likely a bug in BETA10
		scores[1][actor - 1] = carRaceState ? carRaceState->GetState(actor)->GetHighScore() : 0;
#else
		scores[1][actor - 1] = jetskiRaceState ? jetskiRaceState->GetState(actor)->GetHighScore() : 0;
#endif
		totalScore += scores[1][actor - 1];

		scores[2][actor - 1] = pizzaMissionState ? pizzaMissionState->GetHighScore(actor) : 0;
		totalScore += scores[2][actor - 1];

		scores[3][actor - 1] = towTrackMissionState ? towTrackMissionState->GetHighScore(actor) : 0;
		totalScore += scores[3][actor - 1];

		scores[4][actor - 1] = ambulanceMissionState ? ambulanceMissionState->GetHighScore(actor) : 0;
		totalScore += scores[4][actor - 1];
	}

	MxS32 playerScoreHistoryIndex;
	ScoreItem* p_scorehist =
		FindPlayerInScoreHistory(GameState()->m_players, GameState()->m_currentPlayerId, playerScoreHistoryIndex);

#ifdef BETA10
	if (!p_scorehist) {
		MxS32 playerScoreRank;
		// LINE: BETA10 0x100870ee
		for (playerScoreRank = 0; playerScoreRank < m_count; playerScoreRank++) {
			if (totalScore > m_scores[m_indices[playerScoreRank]].m_totalScore) {
				break;
			}
		}
		// LINE: BETA10 0x1008713f
		if (playerScoreRank < m_count) {
			if (m_count < 20) {
				playerScoreHistoryIndex = m_count++;
			}
			else {
				playerScoreHistoryIndex = m_indices[19];
			}

			MxS32 max = m_count - 1;
			for (MxS32 j = max; playerScoreRank < j; j--) {
				m_indices[j - 1] = m_indices[j - 2];
			}

			m_indices[playerScoreRank] = playerScoreHistoryIndex;
			p_scorehist = &m_scores[playerScoreHistoryIndex];
		}
		else if (playerScoreRank < 20) {
			m_indices[m_count] = m_count;
			p_scorehist = &m_scores[m_count++];
		}
	}
	else if (p_scorehist->m_totalScore != totalScore) {
		assert(totalScore > p_scorehist->m_totalScore);

		for (MxS32 i = playerScoreHistoryIndex; i > 0 && m_indices[i - 1] < m_indices[i]; i--) {
			MxU8 tmp = m_indices[i - 1];
			m_indices[i - 1] = m_indices[i];
			m_indices[i] = tmp;
		}
	}
	if (p_scorehist) {
		p_scorehist->m_totalScore = totalScore;
		memcpy(p_scorehist->m_scores[0], scores[0], sizeof(scores));
		p_scorehist->m_name = GameState()->m_players[0];
		p_scorehist->m_playerId = GameState()->m_currentPlayerId;
	}
#else
	if (p_scorehist != NULL) {
		p_scorehist->m_totalScore = totalScore;
		memcpy(p_scorehist->m_scores, scores, sizeof(p_scorehist->m_scores));
	}
	else {
		if (m_count < (MxS16) sizeOfArray(m_scores)) {
			m_scores[m_count].m_totalScore = totalScore;
			memcpy(m_scores[m_count].m_scores, scores, sizeof(m_scores[m_count].m_scores));
			m_scores[m_count].m_name = GameState()->m_players[0];
			m_scores[m_count].m_playerId = GameState()->m_currentPlayerId;
			m_count++;
		}
		else if (m_scores[19].m_totalScore <= totalScore) {
			m_scores[19].m_totalScore = totalScore;
			memcpy(m_scores[19].m_scores, scores, sizeof(m_scores[19].m_scores));
			m_scores[19].m_name = GameState()->m_players[0];
			m_scores[19].m_playerId = GameState()->m_currentPlayerId;
		}
	}

	ScoreItem tmpItem;

	for (MxS32 i = m_count - 1; i >= 0; i--) {
		for (MxS32 j = 1; j <= i; j++) {
			if (m_scores[j].m_totalScore > m_scores[j - 1].m_totalScore) {
				tmpItem = m_scores[j - 1];
				m_scores[j - 1] = m_scores[j];
				m_scores[j] = tmpItem;
			}
		}
	}
#endif
}

// FUNCTION: LEGO1 0x1003cc90
// FUNCTION: BETA10 0x1008732a
LegoGameState::ScoreItem* LegoGameState::History::FindPlayerInScoreHistory(
	LegoGameState::Username* p_player,
	MxS16 p_unk0x24,
	MxS32& p_unk0x2c
)
{
	MxS32 i = 0;
	for (; i < m_count; i++) {
		if (!memcmp(p_player, &m_scores[i].m_name, sizeof(*p_player)) && m_scores[i].m_playerId == p_unk0x24) {
			break;
		}
	}

	p_unk0x2c = i;

	if (i >= m_count) {
		return NULL;
	}

	return &m_scores[i];
}

// FUNCTION: LEGO1 0x1003ccf0
// FUNCTION: BETA10 0x100873e7
MxResult LegoGameState::History::Serialize(LegoStorage* p_storage)
{
	if (p_storage->IsReadMode()) {
		p_storage->ReadS16(m_nextPlayerId);
		p_storage->ReadS16(m_count);

		for (MxS16 i = 0; i < m_count; i++) {
			MxS16 j;
			p_storage->ReadS16(j);
			m_scores[i].Serialize(p_storage);
		}
	}
	else if (p_storage->IsWriteMode()) {
		p_storage->WriteS16(m_nextPlayerId);
		p_storage->WriteS16(m_count);

		for (MxS16 i = 0; i < m_count; i++) {
			p_storage->WriteS16(i);
			m_scores[i].Serialize(p_storage);
		}
	}

	return SUCCESS;
}

// FUNCTION: LEGO1 0x1003cdd0
void LegoGameState::SerializeScoreHistory(MxS16 p_flags)
{
	LegoFile storage;
	MxString savePath(m_savePath);
	savePath += "\\";
	savePath += g_historyGSI;

	if (p_flags == LegoFile::c_write) {
		m_history.WriteScoreHistory();
	}

	if (storage.Open(savePath.GetData(), p_flags) == SUCCESS) {
		m_history.Serialize(&storage);
	}
}

// FUNCTION: LEGO1 0x1003cea0
// FUNCTION: BETA10 0x10017840
void LegoGameState::SetCurrentAct(Act p_currentAct)
{
	m_currentAct = p_currentAct;
}

// FUNCTION: LEGO1 0x1003ceb0
void LegoGameState::FindLoadedAct()
{
	if (FindWorld(*g_isleScript, IsleScript::c__Isle)) {
		m_loadedAct = e_act1;
	}
	else if (FindWorld(*g_act2mainScript, Act2mainScript::c__Act2Main)) {
		m_loadedAct = e_act2;
	}
	else if (FindWorld(*g_act3Script, Act3Script::c__Act3)) {
		m_loadedAct = e_act3;
	}
	else {
		m_loadedAct = e_actNotFound;
	}
}
