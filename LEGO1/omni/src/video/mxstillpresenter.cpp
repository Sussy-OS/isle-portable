#include "mxstillpresenter.h"

#include "decomp.h"
#include "define.h"
#include "mxcompositepresenter.h"
#include "mxdisplaysurface.h"
#include "mxdsmediaaction.h"
#include "mxdssubscriber.h"
#include "mxmain.h"
#include "mxmisc.h"
#include "mxpalette.h"
#include "mxutilities.h"
#include "mxvideomanager.h"

#include <SDL3/SDL_stdinc.h>

DECOMP_SIZE_ASSERT(MxStillPresenter, 0x6c);

// FUNCTION: LEGO1 0x100b9c70
void MxStillPresenter::Destroy(MxBool p_fromDestructor)
{
	ENTER(m_criticalSection);

	if (m_bitmapInfo) {
		delete[] ((MxU8*) m_bitmapInfo);
	}
	m_bitmapInfo = NULL;

	m_criticalSection.Leave();

	if (!p_fromDestructor) {
		MxVideoPresenter::Destroy(FALSE);
	}
}

// FUNCTION: LEGO1 0x100b9cc0
void MxStillPresenter::LoadHeader(MxStreamChunk* p_chunk)
{
	if (m_bitmapInfo) {
		delete[] ((MxU8*) m_bitmapInfo);
	}

	MxU8* data = new MxU8[p_chunk->GetLength()];
	m_bitmapInfo = (MxBITMAPINFO*) data;
	memcpy(m_bitmapInfo, p_chunk->GetData(), p_chunk->GetLength());
}

// FUNCTION: LEGO1 0x100b9d10
void MxStillPresenter::CreateBitmap()
{
	if (m_frameBitmap) {
		delete m_frameBitmap;
	}

	m_frameBitmap = new MxBitmap;
	m_frameBitmap->ImportBitmapInfo(m_bitmapInfo);

	delete[] ((MxU8*) m_bitmapInfo);
	m_bitmapInfo = NULL;
}

// FUNCTION: LEGO1 0x100b9db0
void MxStillPresenter::NextFrame()
{
	MxStreamChunk* chunk = NextChunk();
	LoadFrame(chunk);
	m_subscriber->FreeDataChunk(chunk);
}

// FUNCTION: LEGO1 0x100b9dd0
void MxStillPresenter::LoadFrame(MxStreamChunk* p_chunk)
{
	memcpy(m_frameBitmap->GetImage(), p_chunk->GetData(), p_chunk->GetLength());

	// MxRect32 rect(m_location, MxSize32(GetWidth(), GetHeight()));
	MxS32 height = GetHeight() - 1;
	MxS32 width = GetWidth() - 1;
	MxS32 x = m_location.GetX();
	MxS32 y = m_location.GetY();

	MxRect32 rect(x, y, width + x, height + y);
	MVideoManager()->InvalidateRect(rect);

	if (UseSurface()) {
		undefined4 useVideoMemory = 0;
		m_surface = MxOmni::GetInstance()->GetVideoManager()->GetDisplaySurface()->VTable0x44(
			m_frameBitmap,
			&useVideoMemory,
			DoNotWriteToSurface(),
			m_action->GetFlags() & MxDSAction::c_bit4
		);

		delete m_alpha;
		m_alpha = new AlphaMask(*m_frameBitmap);

		delete m_frameBitmap;
		m_frameBitmap = NULL;

		if (m_surface && useVideoMemory) {
			SetUseVideoMemory(TRUE);
		}
		else {
			SetUseVideoMemory(FALSE);
		}
	}
}

// FUNCTION: LEGO1 0x100b9f30
void MxStillPresenter::RealizePalette()
{
	MxPalette* palette = m_frameBitmap->CreatePalette();
	MVideoManager()->RealizePalette(palette);
	delete palette;
}

// FUNCTION: LEGO1 0x100b9f60
void MxStillPresenter::StartingTickle()
{
	MxVideoPresenter::StartingTickle();

	if (m_currentTickleState == e_streaming && ((MxDSMediaAction*) m_action)->GetPaletteManagement()) {
		RealizePalette();
	}
}

// FUNCTION: LEGO1 0x100b9f90
void MxStillPresenter::StreamingTickle()
{
	MxStreamChunk* chunk = CurrentChunk();

	if (chunk && m_action->GetElapsedTime() >= chunk->GetTime()) {
		m_chunkTime = chunk->GetTime();
		NextFrame();
		ProgressTickleState(e_repeating);

		if (m_action->GetDuration() == -1 && m_compositePresenter) {
			m_compositePresenter->VTable0x60(this);
		}
	}
}

// FUNCTION: LEGO1 0x100b9ff0
void MxStillPresenter::RepeatingTickle()
{
	if (m_action->GetDuration() != -1) {
		if (m_action->GetElapsedTime() >= m_action->GetStartTime() + m_action->GetDuration()) {
			ProgressTickleState(e_freezing);
		}
	}
}

// FUNCTION: LEGO1 0x100ba040
// FUNCTION: BETA10 0x10142724
void MxStillPresenter::SetPosition(MxS32 p_x, MxS32 p_y)
{
	MxPoint32 oldLocation(m_location);
	m_location.SetX(p_x);
	m_location.SetY(p_y);

	if (IsEnabled()) {
		MxRect32 area(0, 0, GetWidth() - 1, GetHeight() - 1);

		MxRect32 rectA(area);
		rectA += oldLocation;

		MxRect32 rectB(area);
		rectB += m_location;

		MVideoManager()->InvalidateRect(rectA);
		MVideoManager()->UpdateView(rectA.GetLeft(), rectA.GetTop(), rectA.GetWidth(), rectA.GetHeight());

		MVideoManager()->InvalidateRect(rectB);
		MVideoManager()->UpdateView(rectB.GetLeft(), rectB.GetTop(), rectB.GetWidth(), rectB.GetHeight());
	}
}

// FUNCTION: LEGO1 0x100ba140
void MxStillPresenter::Enable(MxBool p_enable)
{
	MxPresenter::Enable(p_enable);

	if (MVideoManager() && (m_alpha || m_frameBitmap)) {
		// MxRect32 rect(m_location, MxSize32(GetWidth(), GetHeight()));
		MxS32 height = GetHeight();
		MxS32 width = GetWidth();
		MxS32 x = m_location.GetX();
		MxS32 y = m_location.GetY();

		MxRect32 rect(x, y, width + x, height + y);
		MVideoManager()->InvalidateRect(rect);
		MVideoManager()->UpdateView(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
	}
}

// FUNCTION: LEGO1 0x100ba1e0
void MxStillPresenter::ParseExtra()
{
	MxPresenter::ParseExtra();

	if (m_action->GetFlags() & MxDSAction::c_bit5) {
		SetDoNotWriteToSurface(TRUE);
	}

	MxU16 extraLength;
	char* extraData;
	m_action->GetExtra(extraLength, extraData);

	if (extraLength) {
		char extraCopy[512];
		memcpy(extraCopy, extraData, extraLength);
		extraCopy[extraLength] = '\0';

		char output[512];
		if (KeyValueStringParse(output, g_strVISIBILITY, extraCopy)) {
			if (SDL_strcasecmp(output, "FALSE") == 0) {
				Enable(FALSE);
			}
		}

		if (KeyValueStringParse(output, g_strBMP_ISMAP, extraCopy)) {
			SetBitmapIsMap(TRUE);
			SetUseSurface(FALSE);
			SetUseVideoMemory(FALSE);
		}
	}
}

// FUNCTION: LEGO1 0x100ba2c0
MxStillPresenter* MxStillPresenter::Clone()
{
	MxResult result = FAILURE;
	MxStillPresenter* presenter = new MxStillPresenter;

	if (presenter) {
		if (presenter->AddToManager() == SUCCESS) {
			MxDSAction* action = GetAction()->Clone();

			if (action && presenter->StartAction(NULL, action) == SUCCESS) {
				presenter->SetLoadedFirstFrame(LoadedFirstFrame());
				presenter->SetUseSurface(UseSurface());
				presenter->SetUseVideoMemory(UseVideoMemory());
				presenter->SetDoNotWriteToSurface(DoNotWriteToSurface());
				presenter->SetBitmapIsMap(BitmapIsMap());

				if (m_frameBitmap) {
					presenter->m_frameBitmap = new MxBitmap;

					if (!presenter->m_frameBitmap || presenter->m_frameBitmap->ImportBitmap(m_frameBitmap) != SUCCESS) {
						goto done;
					}
				}

				if (m_surface) {
					presenter->m_surface = MxDisplaySurface::CopySurface(m_surface);
				}

				if (m_alpha) {
					presenter->m_alpha = new MxVideoPresenter::AlphaMask(*m_alpha);
				}

				result = SUCCESS;
			}
		}
	}

done:
	if (result != SUCCESS) {
		delete presenter;
		presenter = NULL;
	}

	return presenter;
}
