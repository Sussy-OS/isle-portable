#ifndef HISTORYBOOK_H
#define HISTORYBOOK_H

#include "decomp.h"
#include "legogamestate.h"
#include "legoworld.h"

class MxStillPresenter;

// VTABLE: LEGO1 0x100da328
// VTABLE: BETA10 0x101b99b0
// SIZE 0x3e4
class HistoryBook : public LegoWorld {
public:
	HistoryBook();
	~HistoryBook() override; // vtable+0x00

	MxLong Notify(MxParam& p_param) override; // vtable+0x04

	// FUNCTION: LEGO1 0x10082390
	// FUNCTION: BETA10 0x1002c1c0
	const char* ClassName() const override // vtable+0x0c
	{
		// STRING: LEGO1 0x100f04bc
		return "HistoryBook";
	}

	// FUNCTION: LEGO1 0x100823a0
	MxBool IsA(const char* p_name) const override // vtable+0x10
	{
		return !strcmp(p_name, HistoryBook::ClassName()) || LegoWorld::IsA(p_name);
	}

	MxResult Create(MxDSAction& p_dsAction) override; // vtable+0x18
	void ReadyWorld() override;                       // vtable+0x50
	MxBool Escape() override;                         // vtable+0x64

	// SYNTHETIC: LEGO1 0x100824b0
	// HistoryBook::`scalar deleting destructor'

private:
	LegoGameState::Area m_destLocation; // 0xf8
	MxStillPresenter* m_alphabet[26];   // 0xfc

	// variable name verified by BETA10 0x1002bd27
	MxStillPresenter* m_name[20][7]; // 0x164

	MxStillPresenter* m_scores[20]; // 0x394

	MxStillPresenter* m_intAlphabet[sizeOfArray(LegoGameState::g_intCharacters)];
	MxU32 m_intAlphabetOffset;
};

#endif // HISTORYBOOK_H
