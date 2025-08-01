#include "act3.h"

#include "3dmanager/lego3dmanager.h"
#include "act3_actions.h"
#include "act3actors.h"
#include "helicopter.h"
#include "jukebox_actions.h"
#include "legoanimationmanager.h"
#include "legobuildingmanager.h"
#include "legocontrolmanager.h"
#include "legomain.h"
#include "legonavcontroller.h"
#include "legoplantmanager.h"
#include "legoutils.h"
#include "legovideomanager.h"
#include "misc.h"
#include "mxactionnotificationparam.h"
#include "mxbackgroundaudiomanager.h"
#include "mxmisc.h"
#include "mxnotificationmanager.h"
#include "mxticklemanager.h"
#include "mxtimer.h"
#include "mxtransitionmanager.h"
#include "scripts.h"

#include <stdio.h>
#include <vec.h>

DECOMP_SIZE_ASSERT(Act3, 0x4274)
DECOMP_SIZE_ASSERT(Act3State, 0x0c)
DECOMP_SIZE_ASSERT(Act3ListElement, 0x0c)
DECOMP_SIZE_ASSERT(Act3List, 0x10)

// GLOBAL: LEGO1 0x100d94f8
Act3Script::Script g_pizzaHitSounds[] = {
	Act3Script::c_sns02xni_PlayWav,
	Act3Script::c_sns03xni_PlayWav,
	Act3Script::c_sns04xni_PlayWav,
	Act3Script::c_sns05xni_PlayWav,
	Act3Script::c_sns06xni_PlayWav,
	Act3Script::c_sns07xni_PlayWav,
	Act3Script::c_sns08xni_PlayWav,
	Act3Script::c_sns09xni_PlayWav,
	Act3Script::c_sns10xni_PlayWav,
	Act3Script::c_sns11xni_PlayWav,
	Act3Script::c_sns12xla_PlayWav,
	Act3Script::c_sns13xla_PlayWav,
	Act3Script::c_sns14xla_PlayWav,
	Act3Script::c_sns15xla_PlayWav,
	Act3Script::c_sns16xla_PlayWav,
	Act3Script::c_sns17xla_PlayWav
};

// GLOBAL: LEGO1 0x100d9538
Act3Script::Script g_pizzaMissSounds[] = {
	Act3Script::c_sns19xni_PlayWav,
	Act3Script::c_sns20xni_PlayWav,
	Act3Script::c_sns22xni_PlayWav,
	Act3Script::c_sns23xni_PlayWav,
	Act3Script::c_sns35xla_PlayWav,
	(Act3Script::Script) 0
};

// GLOBAL: LEGO1 0x100d9550
Act3Script::Script g_copDonutSounds[] = {
	Act3Script::c_sns25xni_PlayWav,
	Act3Script::c_sns26xni_PlayWav,
	Act3Script::c_sns27xni_PlayWav,
	Act3Script::c_sns28xni_PlayWav,
	Act3Script::c_sns29xni_PlayWav,
	Act3Script::c_sns37xla_PlayWav,
	Act3Script::c_sns38xla_PlayWav,
	Act3Script::c_sns39xla_PlayWav
};

// GLOBAL: LEGO1 0x100d9570
Act3Script::Script g_donutMissSounds[] = {
	Act3Script::c_sns30xni_PlayWav,
	Act3Script::c_sns31xni_PlayWav,
	Act3Script::c_sns32xni_PlayWav,
	Act3Script::c_sns40xla_PlayWav,
	Act3Script::c_sns41xla_PlayWav,
	Act3Script::c_sns42xla_PlayWav
};

// GLOBAL: LEGO1 0x100d9588
Act3Script::Script g_islanderSounds[] = {
	Act3Script::c_sns43xma_PlayWav, Act3Script::c_sns46xin_PlayWav, Act3Script::c_sns60xna_PlayWav,
	Act3Script::c_sns52xro_PlayWav, Act3Script::c_sns58xna_PlayWav, Act3Script::c_sns68xbu_PlayWav,
	Act3Script::c_sns59xna_PlayWav, Act3Script::c_sns51xin_PlayWav, Act3Script::c_sns61xva_PlayWav,
	Act3Script::c_sns44xma_PlayWav, Act3Script::c_sns47xin_PlayWav, Act3Script::c_sns53xro_PlayWav,
	Act3Script::c_sns45xma_PlayWav, Act3Script::c_sns69xsn_PlayWav, Act3Script::c_sns48xin_PlayWav,
	Act3Script::c_sns66xsl_PlayWav, Act3Script::c_sns49xin_PlayWav, Act3Script::c_sns62xmg_PlayWav,
	Act3Script::c_sns54xro_PlayWav, Act3Script::c_sns50xin_PlayWav
};

// GLOBAL: LEGO1 0x100d95d8
Act3Script::Script g_bricksterDonutSounds[] = {
	Act3Script::c_tns080br_PlayWav,
	Act3Script::c_tnsx07br_PlayWav,
	Act3Script::c_snsxx2br_PlayWav,
	Act3Script::c_snsy23br_PlayWav
};

// GLOBAL: LEGO1 0x100f7814
MxU8 g_unk0x100f7814 = 0;

// GLOBAL: LEGO1 0x100d95e8
Act3Script::Script g_unk0x100d95e8[] =
	{Act3Script::c_tlp053in_RunAnim, Act3Script::c_tlp064la_RunAnim, Act3Script::c_tlp068in_RunAnim};

// FUNCTION: LEGO1 0x10071d40
void Act3List::Insert(MxS32 p_objectId, InsertMode p_option)
{
	if (m_unk0x0c) {
		return;
	}

	switch (p_option) {
	case InsertMode::e_replaceAction:
		if (!empty()) {
			DeleteActionWrapper();
			push_back(Act3ListElement(p_objectId, p_option, FALSE));
		}
		else {
			InvokeAction(Extra::e_start, *g_act3Script, p_objectId, NULL);
			push_back(Act3ListElement(p_objectId, p_option, TRUE));
		}
		break;
	case InsertMode::e_queueAction:
		if (empty()) {
			push_back(Act3ListElement(p_objectId, p_option, TRUE));
			InvokeAction(Extra::e_start, *g_act3Script, p_objectId, NULL);
		}
		else {
			push_back(Act3ListElement(p_objectId, p_option, FALSE));
		}
		break;
	case InsertMode::e_onlyIfEmpty:
		if (empty()) {
			push_back(Act3ListElement(p_objectId, p_option, TRUE));
			InvokeAction(Extra::e_start, *g_act3Script, p_objectId, NULL);
		}
		break;
	}
}

// FUNCTION: LEGO1 0x10071fa0
void Act3List::DeleteActionWrapper()
{
	DeleteAction();
}

// FUNCTION: LEGO1 0x10071fb0
void Act3List::Clear()
{
	m_unk0x0c = 1;
	BackgroundAudioManager()->Stop();

	if (empty()) {
		return;
	}

	for (Act3List::iterator it = begin(); it != end();) {
		if ((*it).m_hasStarted) {
			MxDSAction ds;
			ds.SetAtomId(*g_act3Script);
			ds.SetObjectId((*it).m_objectId);
			DeleteObject(ds);
		}

		erase(it++);
	}
}

// Removes the element with the given objectId from the list, or the first if `p_objectId` is zero.
// FUNCTION: LEGO1 0x100720d0
void Act3List::RemoveByObjectIdOrFirst(MxU32 p_objectId)
{
	if (m_unk0x0c) {
		return;
	}

	MxU32 removed = FALSE;
	Act3List::iterator it;
	// This iterator appears to be unnecessary - maybe left in by accident, or it was used for assertions.
	// Removing it decreases the match percentage.
	Act3List::iterator unusedIterator;

	if (empty()) {
		return;
	}

	if (!p_objectId) {
		pop_front();
		removed = TRUE;
	}
	else {
		for (it = begin(); it != end(); it++) {
			// Removing this variable decreases the match, but replacing `*it` by `unused` below also does.
			Act3ListElement& unused = *it;

			if ((*it).m_hasStarted && (*it).m_objectId == p_objectId) {
				erase(it);
				removed = TRUE;
				break;
			}
		}
	}

	if (removed && size() > 0) {
		it = begin();
		unusedIterator = it;
		Act3ListElement& firstItem = front();
		it++;

		while (it != end()) {
			if ((*it).m_unk0x04 == 1) {
				for (Act3List::iterator it2 = begin(); it2 != it; erase(it2++)) {
					if ((*it2).m_hasStarted) {
						DeleteActionWrapper();
						return;
					}
				}
			}

			it++;
			unusedIterator++;
		}

		if (!firstItem.m_hasStarted) {
			firstItem.m_hasStarted = TRUE;
			InvokeAction(Extra::e_start, *g_act3Script, firstItem.m_objectId, NULL);
		}
	}
}

// FUNCTION: LEGO1 0x10072270
// FUNCTION: BETA10 0x10015470
Act3::Act3()
{
	m_state = NULL;
	m_unk0x41fc = 0;
	m_cop1 = NULL;
	m_cop2 = NULL;
	m_brickster = NULL;
	m_copter = NULL;
	m_shark = NULL;
	m_time = -1;
	m_unk0x421e = 0;

	memset(m_helicopterDots, 0, sizeof(m_helicopterDots));

	NavController()->ResetMaxLinearAccel(NavController()->GetMaxLinearAccel() * 30.0f);
	NavController()->ResetMaxLinearDeccel(NavController()->GetMaxLinearDeccel() * 30.0f);
	NotificationManager()->Register(this);
}

// FUNCTION: LEGO1 0x100726a0
// FUNCTION: BETA10 0x100155da
Act3::~Act3()
{
	Destroy(TRUE);
	NotificationManager()->Unregister(this);
	TickleManager()->UnregisterClient(this);
}

// FUNCTION: LEGO1 0x10072780
// FUNCTION: BETA10 0x100156ac
void Act3::EatPizza(MxS32 p_index)
{
	assert(p_index < MAX_PIZZAS);
	RemovePizza(m_pizzas[p_index]);
}

// FUNCTION: LEGO1 0x100727a0
// FUNCTION: BETA10 0x1001570d
void Act3::EatDonut(MxS32 p_index)
{
	assert(p_index < MAX_DONUTS);
	RemoveDonut(m_donuts[p_index]);
}

// FUNCTION: LEGO1 0x100727c0
// FUNCTION: BETA10 0x1001576e
void Act3::RemovePizza(Act3Ammo& p_p)
{
#ifdef _DEBUG
	MxS32 i;
	for (i = 0; i < MAX_PIZZAS; i++) {
		if (&m_pizzas[i] == &p_p) {
			break;
		}
	}

	assert(i != MAX_PIZZAS);
#endif

	assert(p_p.IsValid());
	p_p.Remove();
}

// FUNCTION: LEGO1 0x100727d0
// FUNCTION: BETA10 0x10015828
void Act3::RemoveDonut(Act3Ammo& p_p)
{
#ifdef _DEBUG
	MxS32 i;
	for (i = 0; i < MAX_DONUTS; i++) {
		if (&m_donuts[i] == &p_p) {
			break;
		}
	}

	assert(i != MAX_DONUTS);
#endif

	assert(p_p.IsValid());
	p_p.Remove();
}

// FUNCTION: LEGO1 0x100727e0
// FUNCTION: BETA10 0x100158e2
MxResult Act3::ShootPizza(LegoPathController* p_controller, Vector3& p_location, Vector3& p_direction, Vector3& p_up)
{
	MxS32 nextPizza;
	for (nextPizza = 0; nextPizza < (MxS32) sizeOfArray(m_pizzas); nextPizza++) {
		if (!m_pizzas[nextPizza].IsValid()) {
			LegoPathBoundary* boundary = NULL;
			MxU32 local18 = TRUE;

			m_pizzas[nextPizza].Create(this, TRUE, nextPizza);

			if (m_pizzas[nextPizza].FUN_10053b40(p_location, p_direction, p_up) != SUCCESS) {
				return FAILURE;
			}

			MxFloat unk0x19c = *m_pizzas[nextPizza].GetUnknown0x19c();
			if (p_controller->FUN_1004a380(
					p_location,
					p_direction,
					m_pizzas[nextPizza].GetUnknown0x160(),
					boundary,
					unk0x19c
				) == SUCCESS) {
				Mx3DPointFloat direction;

				direction = p_direction;
				direction *= unk0x19c;
				direction += p_location;

				assert(m_brickster && m_brickster->GetROI());

				direction -= m_brickster->GetROI()->GetLocal2World()[3];

				local18 = FALSE;
				if (m_pizzas[nextPizza].FUN_10053cb0(p_controller, boundary, unk0x19c) == SUCCESS) {
					p_controller->PlaceActor(&m_pizzas[nextPizza]);
					boundary->AddActor(&m_pizzas[nextPizza]);
					m_pizzas[nextPizza].SetWorldSpeed(10.0f);
					return SUCCESS;
				}
			}

			if (local18 && m_pizzas[nextPizza].FUN_10053d30(p_controller, unk0x19c) == SUCCESS) {
				p_controller->PlaceActor(&m_pizzas[nextPizza]);
				m_pizzas[nextPizza].SetWorldSpeed(10.0f);
				return SUCCESS;
			}

			break;
		}
	}

	return FAILURE;
}

// FUNCTION: LEGO1 0x10072980
// FUNCTION: BETA10 0x10015c69
MxResult Act3::ShootDonut(LegoPathController* p_controller, Vector3& p_location, Vector3& p_direction, Vector3& p_up)
{
	MxS32 nextDonut;
	for (nextDonut = 0; nextDonut < (MxS32) sizeOfArray(m_donuts); nextDonut++) {
		if (!m_donuts[nextDonut].IsValid()) {
			LegoPathBoundary* boundary = NULL;

			m_donuts[nextDonut].Create(this, FALSE, nextDonut);

			if (m_donuts[nextDonut].FUN_10053b40(p_location, p_direction, p_up) != SUCCESS) {
				return FAILURE;
			}

			MxFloat unk0x19c = *m_donuts[nextDonut].GetUnknown0x19c();
			if (p_controller->FUN_1004a380(
					p_location,
					p_direction,
					m_donuts[nextDonut].GetUnknown0x160(),
					boundary,
					unk0x19c
				) == SUCCESS) {
				if (m_donuts[nextDonut].FUN_10053cb0(p_controller, boundary, unk0x19c) == SUCCESS) {
					p_controller->PlaceActor(&m_donuts[nextDonut]);
					boundary->AddActor(&m_donuts[nextDonut]);
					m_donuts[nextDonut].SetWorldSpeed(10.0f);
					return SUCCESS;
				}
			}
			else if (m_donuts[nextDonut].FUN_10053d30(p_controller, unk0x19c) == SUCCESS) {
				p_controller->PlaceActor(&m_donuts[nextDonut]);
				m_donuts[nextDonut].SetWorldSpeed(10.0f);
				return SUCCESS;
			}
		}
	}

	return FAILURE;
}

// FUNCTION: LEGO1 0x10072ad0
// FUNCTION: BETA10 0x10015eec
void Act3::TriggerHitSound(undefined4 p_param1)
{
	float time = Timer()->GetTime();
	Act3Script::Script objectId;

	switch (p_param1) {
	case 1: {
		if (m_pizzaHitSound >= sizeOfArray(g_pizzaHitSounds)) {
			m_pizzaHitSound = 0;
		}

		objectId = g_pizzaHitSounds[m_pizzaHitSound++];
		break;
	}
	case 2: {
		if (m_pizzaMissSound >= sizeOfArray(g_pizzaMissSounds) - 1) {
			m_pizzaMissSound = 0;
		}

		objectId = g_pizzaMissSounds[m_pizzaMissSound++];
		break;
	}
	case 3: {
		if (m_copDonutSound >= sizeOfArray(g_copDonutSounds)) {
			m_copDonutSound = 0;
		}

		objectId = g_copDonutSounds[m_copDonutSound++];
		break;
	}
	case 4: {
		if (m_donutMissSound >= sizeOfArray(g_donutMissSounds)) {
			m_donutMissSound = 0;
		}

		objectId = g_donutMissSounds[m_donutMissSound++];
		break;
	}
	case 5: {
		if (m_islanderSound >= sizeOfArray(g_islanderSounds)) {
			m_islanderSound = 0;
		}

		objectId = g_islanderSounds[m_islanderSound++];
		break;
	}
	case 6: {
		if (m_bricksterDonutSound >= sizeOfArray(g_bricksterDonutSounds)) {
			m_bricksterDonutSound = 0;
		}

		m_unk0x4220.Insert(g_bricksterDonutSounds[m_bricksterDonutSound++], Act3List::e_replaceAction);
		return;
	}
	default:
		return;
	}

	m_unk0x4220.Insert(objectId, Act3List::e_onlyIfEmpty);
}

// FUNCTION: LEGO1 0x10072c30
// FUNCTION: BETA10 0x100160fb
MxResult Act3::Create(MxDSAction& p_dsAction)
{
	GameState()->FindLoadedAct();

	MxResult result = LegoWorld::Create(p_dsAction);
	if (result == SUCCESS) {
		ControlManager()->Register(this);
		InputManager()->SetWorld(this);
		InputManager()->Register(this);

		switch (GameState()->GetLoadedAct()) {
		case LegoGameState::e_act2:
			GameState()->StopArea(LegoGameState::e_infomain);
			GameState()->StopArea(LegoGameState::e_act2main);
			break;
		case LegoGameState::e_act3:
			GameState()->StopArea(LegoGameState::e_infomain);
			GameState()->StopArea(LegoGameState::e_act3script);
			break;
		case LegoGameState::e_act1:
		case LegoGameState::e_actNotFound:
			GameState()->StopArea(LegoGameState::e_undefined);
			if (GameState()->m_previousArea == LegoGameState::e_infomain) {
				GameState()->StopArea(LegoGameState::e_isle);
			}
		}

		LegoGameState* gameState = GameState();
		Act3State* state = (Act3State*) gameState->GetState("Act3State");

		if (state == NULL) {
			state = (Act3State*) gameState->CreateState("Act3State");
		}

		m_state = state;
		assert(m_state);

		GameState()->m_currentArea = LegoGameState::e_act3script;
		GameState()->SetCurrentAct(LegoGameState::e_act3);
		GameState()->m_isDirty = TRUE;
	}

	return result;
}

// FUNCTION: LEGO1 0x10072d50
// FUNCTION: BETA10 0x1001627f
void Act3::Destroy(MxBool p_fromDestructor)
{
	NavController()->Reset();
	ControlManager()->Unregister(this);

	if (InputManager()->GetWorld() == this) {
		InputManager()->ClearWorld();
	}

	InputManager()->UnRegister(this);

	if (UserActor() != NULL) {
		if ((IslePathActor*) UserActor() == m_copter) {
			((IslePathActor*) UserActor())->Exit();
		}

		Remove(UserActor());
	}

	if (!p_fromDestructor) {
		LegoWorld::Destroy(FALSE);
	}
}

// FUNCTION: LEGO1 0x10072de0
// FUNCTION: BETA10 0x10016322
MxLong Act3::Notify(MxParam& p_param)
{
	MxNotificationParam& param = (MxNotificationParam&) p_param;
	LegoWorld::Notify(p_param);

	if (m_worldStarted) {
		switch (param.GetNotification()) {
		case c_notificationEndAction: {
			MxEndActionNotificationParam& param = (MxEndActionNotificationParam&) p_param;

			if (param.GetAction() != NULL && param.GetAction()->GetAtomId() == *g_act3Script) {
				if (param.GetAction()->GetObjectId() == Act3Script::c_HelicopterDashboard) {
					MxDSAction action;
					Disable(FALSE, LegoOmni::c_disableInput | LegoOmni::c_disable3d | LegoOmni::c_clearScreen);
					SetAppCursor(e_cursorArrow);
					VideoManager()->Get3DManager()->SetFrustrum(45.0f, 0.1f, 125.0f);

					m_brickster->SetWorldSpeed(5.0f);
					m_brickster->SetActorState(LegoPathActor::c_initial);
					assert(BackgroundAudioManager());

					action.SetAtomId(*g_jukeboxScript);
					action.SetObjectId(Act3Script::c_pzhitdn_PlayWav);

					BackgroundAudioManager()->PlayMusic(action, 5, MxPresenter::e_repeating);
					m_brickster->FUN_100417c0();

					m_cop1->SetActorState(LegoPathActor::c_initial);
					m_cop1->SetWorldSpeed(2.0f);
					m_cop1->VTable0xa8();

					m_cop2->SetActorState(LegoPathActor::c_initial);
					m_cop2->SetWorldSpeed(2.0f);
					m_cop2->VTable0xa8();

					m_brickster->VTable0xa8();
					m_pizzaHitSound = 0;
					m_pizzaMissSound = 0;
					m_copDonutSound = 0;
					m_donutMissSound = 0;
					m_islanderSound = 0;
					m_bricksterDonutSound = 0;

					MxS32 length;
					LegoBuildingInfo* info = BuildingManager()->GetInfoArray(length);
					m_unk0x421e = 0;

					while (--length >= 0) {
						if (info[length].m_counter < 0 && info[length].m_boundary != NULL &&
							info[length].m_entity != NULL) {
							m_unk0x421e++;
						}
					}

					length = 0;
					m_unk0x421e--;
					char buf[80];

					do {
						sprintf(buf, "HelicopterDotOn%d_Bitmap", length + 1);
						m_helicopterDots[length] = (MxPresenter*) Find("MxPresenter", buf);

						if (m_unk0x421e > length) {
							m_helicopterDots[length]->Enable(TRUE);
						}
						else {
							m_helicopterDots[length]->Enable(FALSE);
						}

						length++;
					} while (length < (MxS32) sizeOfArray(m_helicopterDots));
				}
				else {
					m_unk0x4220.RemoveByObjectIdOrFirst(param.GetAction()->GetObjectId());
				}
			}
			break;
		}
		case c_notificationKeyPress:
			if (m_state->m_unk0x08 == 1 && ((LegoEventNotificationParam&) p_param).GetKey() == ' ') {
				AnimationManager()->FUN_10061010(FALSE);
				return 1;
			}
			break;
		case c_notificationButtonUp:
		case c_notificationButtonDown:
			if (m_state->m_unk0x08 == 1) {
				return 1;
			}
			break;
		case c_notificationEndAnim:
			if (m_state->m_unk0x08 == 1) {
				assert(m_copter && m_brickster && m_cop1 && m_cop2);
				m_unk0x4220.RemoveByObjectIdOrFirst(0);
				m_state->m_unk0x08 = 0;
				Disable(TRUE, LegoOmni::c_disableInput | LegoOmni::c_disable3d | LegoOmni::c_clearScreen);
				m_copter->HandleClick();
				m_copter->m_state->m_unk0x08 = 1;
				m_copter->HandleEndAnim((LegoEndAnimNotificationParam&) param);
			}
			break;
		case c_notificationTransitioned:
			HandleTransitionEnd();
			return 1;
		}
	}

	return 0;
}

// FUNCTION: LEGO1 0x10073240
MxLong Act3::HandleTransitionEnd()
{
	if (m_destLocation != LegoGameState::e_undefined) {
		GameState()->SwitchArea(m_destLocation);
		m_destLocation = LegoGameState::e_undefined;
	}

	return 1;
}

// FUNCTION: LEGO1 0x10073270
void Act3::ReadyWorld()
{
	PlantManager()->SetInitialCounters();
	BuildingManager()->SetInitialCounters();
	AnimationManager()->FUN_1005f6d0(FALSE);
	VideoManager()->Get3DManager()->SetFrustrum(90.0f, 0.1f, 125.0f);

	m_unk0x426c = g_unk0x100d95e8[SDL_rand(3)];
	AnimationManager()
		->FUN_10060dc0(m_unk0x426c, NULL, TRUE, LegoAnimationManager::e_unk0, NULL, TRUE, FALSE, FALSE, FALSE);

	m_state->m_unk0x08 = 1;
}

// FUNCTION: LEGO1 0x10073300
MxResult Act3::Tickle()
{
	if (!m_worldStarted) {
		LegoWorld::Tickle();
		return SUCCESS;
	}

	if (m_unk0x426c != (Act3Script::Script) 0) {
		if (AnimationManager()->FUN_10064ee0(m_unk0x426c)) {
			Disable(FALSE, LegoOmni::c_disableInput | LegoOmni::c_disable3d | LegoOmni::c_clearScreen);
			TickleManager()->UnregisterClient(this);
			m_unk0x426c = (Act3Script::Script) 0;
		}
	}

	return SUCCESS;
}

// FUNCTION: LEGO1 0x10073360
// FUNCTION: BETA10 0x100169d5
MxResult Act3::FUN_10073360(Act3Ammo& p_ammo, const Vector3& p_param2)
{
	assert(m_brickster);
	m_brickster->FUN_100417a0(p_ammo, p_param2);
	TriggerHitSound(1);
	return SUCCESS;
}

// FUNCTION: LEGO1 0x10073390
// FUNCTION: BETA10 0x10016a40
MxResult Act3::FUN_10073390(Act3Ammo& p_ammo, const Vector3& p_param2)
{
	assert(m_cop1 && m_cop2);

	if (!(g_unk0x100f7814 & 1)) {
		m_cop1->FUN_10040350(p_ammo, p_param2);
	}
	else {
		m_cop2->FUN_10040350(p_ammo, p_param2);
	}

	TriggerHitSound(3);
	g_unk0x100f7814++;
	return SUCCESS;
}

// FUNCTION: LEGO1 0x100733d0
// FUNCTION: BETA10 0x10016b5d
void Act3::AddCop(Act3Cop* p_cop)
{
	if (m_cop1) {
		m_cop2 = p_cop;
	}
	else {
		m_cop1 = p_cop;
	}
}

// FUNCTION: LEGO1 0x100733f0
// FUNCTION: BETA10 0x10016ba2
void Act3::SetBrickster(Act3Brickster* p_brickster)
{
	m_brickster = p_brickster;
}

// FUNCTION: LEGO1 0x10073400
void Act3::FUN_10073400()
{
	m_state->m_unk0x08 = 2;
	m_destLocation = LegoGameState::e_infomain;
	TransitionManager()->StartTransition(MxTransitionManager::e_mosaic, 50, FALSE, FALSE);
}

// FUNCTION: LEGO1 0x10073430
void Act3::FUN_10073430()
{
	m_state->m_unk0x08 = 3;
	m_destLocation = LegoGameState::e_infomain;
	TransitionManager()->StartTransition(MxTransitionManager::e_mosaic, 50, FALSE, FALSE);
}

// FUNCTION: LEGO1 0x10073460
// FUNCTION: BETA10 0x10016bc6
void Act3::GoodEnding(const Matrix4& p_destination)
{
	assert(m_cop1 && m_cop2 && m_brickster && m_state);

	m_cop1->SetActorState(LegoPathActor::c_disabled);
	m_cop2->SetActorState(LegoPathActor::c_disabled);
	m_brickster->SetActorState(LegoPathActor::c_disabled);

#ifndef BETA10
	m_unk0x4220.Clear();
	m_copter->FUN_10004640(p_destination);

	DebugPrintf("In Good Ending...");
	DebugCopter(
		m_copter->GetROI()->GetLocal2World(),
		p_destination,
		m_copter->m_unk0x160,
		m_copter->m_unk0x1a8,
		m_copter->m_unk0x1f4
	);

	EmitGameEvent(e_goodEnding);
#else
	m_state->m_unk0x08 = 2;
	GameState()->SwitchArea(LegoGameState::Area::e_infomain);
#endif
}

// FUNCTION: LEGO1 0x10073500
void Act3::DebugPrintf(const char* p_format, ...)
{
	// empty
}

// FUNCTION: LEGO1 0x10073510
void Act3::DebugCopter(
	const Matrix4& p_copter,
	const Matrix4& p_destination,
	const Matrix4& p_startPosition,
	const Matrix4& p_endPosition,
	const MxQuaternionTransformer& p_quatTransform
)
{
	DebugPrintf("Copter matrix...\n\n");

	// STRING: LEGO1 0x100f78e0
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_copter[0]));
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_copter[1]));
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_copter[2]));
	// STRING: LEGO1 0x100f78cc
	DebugPrintf("\t%g, %g, %g, %g\n\n", EXPAND4(p_copter[3]));

	DebugPrintf("Destination matrix...");
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_destination[0]));
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_destination[1]));
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_destination[2]));
	DebugPrintf("\t%g, %g, %g, %g\n\n", EXPAND4(p_destination[3]));

	DebugPrintf("Start position...");
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_startPosition[0]));
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_startPosition[1]));
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_startPosition[2]));
	DebugPrintf("\t%g, %g, %g, %g\n\n", EXPAND4(p_startPosition[3]));

	DebugPrintf("End position...");
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_endPosition[0]));
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_endPosition[1]));
	DebugPrintf("\t%g, %g, %g, %g", EXPAND4(p_endPosition[2]));
	DebugPrintf("\t%g, %g, %g, %g\n\n", EXPAND4(p_endPosition[3]));

	Mx4DPointFloat startQuat, endQuat;

	if (p_quatTransform.GetFlags() != 0) {
		p_quatTransform.GetQuat(startQuat, endQuat);

		DebugPrintf("Source quaternion...");
		// STRING: LEGO1 0x100f7864
		DebugPrintf("\t%g, %g, %g, %g\n", EXPAND4(startQuat));

		DebugPrintf("Destination quaternion...");
		DebugPrintf("\t%g, %g, %g, %g\n", EXPAND4(endQuat));
	}
}

// FUNCTION: LEGO1 0x100739c0
// FUNCTION: BETA10 0x10016cc4
void Act3::BadEnding(const Matrix4& p_destination)
{
	assert(m_cop1 && m_cop2 && m_brickster && m_state);

	m_cop1->SetActorState(LegoPathActor::c_disabled);
	m_cop2->SetActorState(LegoPathActor::c_disabled);
	m_brickster->SetActorState(LegoPathActor::c_disabled);

	m_unk0x4220.Clear();
	m_copter->FUN_10004670(p_destination);

	DebugPrintf("In Bad Ending...");
	DebugCopter(
		m_copter->GetROI()->GetLocal2World(),
		p_destination,
		m_copter->m_unk0x160,
		m_copter->m_unk0x1a8,
		m_copter->m_unk0x1f4
	);

	EmitGameEvent(e_badEnding);
}

// FUNCTION: LEGO1 0x10073a60
void Act3::FUN_10073a60()
{
	m_unk0x421e--;
	m_helicopterDots[m_unk0x421e]->Enable(FALSE);
}

// FUNCTION: LEGO1 0x10073a90
void Act3::Enable(MxBool p_enable)
{
	if ((MxBool) m_set0xd0.empty() == p_enable) {
		return;
	}

	LegoWorld::Enable(p_enable);

	if (p_enable) {
		if (GameState()->m_previousArea == LegoGameState::e_infomain) {
			GameState()->StopArea(LegoGameState::e_infomain);
		}

		Disable(FALSE, LegoOmni::c_disableInput | LegoOmni::c_disable3d | LegoOmni::c_clearScreen);
		PlayMusic(JukeboxScript::c_Act3Music);
		GameState()->m_isDirty = TRUE;

		if (m_time > 0) {
			MxFloat delta = Timer()->GetTime() - m_time - 100.0f;
			m_time = -1.0f;

			m_cop1->SetLastTime(m_cop1->GetLastTime() + delta);
			m_cop1->SetActorTime(m_cop1->GetActorTime() + delta);
			m_cop1->SetUnknown0x20(m_cop1->GetUnknown0x20() + delta);
			m_cop1->SetUnknown0x1c(m_cop1->GetUnknown0x1c() + delta);

			m_cop2->SetLastTime(m_cop2->GetLastTime() + delta);
			m_cop2->SetActorTime(m_cop2->GetActorTime() + delta);
			m_cop2->SetUnknown0x20(m_cop2->GetUnknown0x20() + delta);
			m_cop2->SetUnknown0x1c(m_cop2->GetUnknown0x1c() + delta);

			m_brickster->SetLastTime(m_brickster->GetLastTime() + delta);
			m_brickster->SetActorTime(m_brickster->GetActorTime() + delta);
			m_brickster->SetUnknown0x20(m_brickster->GetUnknown0x20() + delta);
			m_brickster->SetUnknown0x24(m_brickster->GetUnknown0x24() + delta);
			m_brickster->SetUnknown0x50(m_brickster->GetUnknown0x50() + delta);
			m_brickster->SetUnknown0x1c(m_brickster->GetUnknown0x1c() + delta);

			m_copter->SetLastTime(m_copter->GetLastTime() + delta);
			m_copter->SetActorTime(m_copter->GetActorTime() + delta);

			m_shark->SetLastTime(m_shark->GetLastTime() + delta);
			m_shark->SetActorTime(m_shark->GetActorTime() + delta);
			m_shark->SetUnknown0x2c(m_shark->GetUnknown0x2c() + delta);

			MxS32 i;
			for (i = 0; i < (MxS32) sizeOfArray(m_pizzas); i++) {
				if (m_pizzas[i].IsValid()) {
					m_pizzas[i].SetLastTime(m_pizzas[i].GetLastTime() + delta);
					m_pizzas[i].SetActorTime(m_pizzas[i].GetActorTime() + delta);
					m_pizzas[i].SetUnknown0x158(m_pizzas[i].GetUnknown0x158() + delta);
				}
			}

			for (i = 0; i < (MxS32) sizeOfArray(m_donuts); i++) {
				if (m_donuts[i].IsValid()) {
					m_donuts[i].SetLastTime(m_donuts[i].GetLastTime() + delta);
					m_donuts[i].SetActorTime(m_donuts[i].GetActorTime() + delta);
					m_donuts[i].SetUnknown0x158(m_donuts[i].GetUnknown0x158() + delta);
				}
			}

			PlaceActor(m_copter);
			m_copter->GetBoundary()->AddActor(m_copter);

			InputManager()->SetWorld(this);
			InputManager()->Register(this);
			SetUserActor(m_copter);
			m_copter->VTable0xa8();
			SetAppCursor(e_cursorArrow);
		}
	}
	else {
		SetUserActor(NULL);
		BackgroundAudioManager()->Stop();
		m_time = Timer()->GetTime();
		TickleManager()->UnregisterClient(this);
	}
}

// FUNCTION: LEGO1 0x10073e40
void Act3::VTable0x60()
{
	// empty
}

// FUNCTION: LEGO1 0x10073e50
MxBool Act3::Escape()
{
	BackgroundAudioManager()->Stop();
	AnimationManager()->FUN_10061010(FALSE);
	DeleteObjects(&m_atomId, Act3Script::c_tlp053in_RunAnim, 999);
	m_destLocation = LegoGameState::e_infomain;
	return TRUE;
}
