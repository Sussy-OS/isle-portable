#include "mxvideomanager.h"

#include "mxautolock.h"
#include "mxdisplaysurface.h"
#include "mxmain.h"
#include "mxmisc.h"
#include "mxpalette.h"
#include "mxpresenter.h"
#include "mxregion.h"
#include "mxticklemanager.h"
#include "mxticklethread.h"

#include <SDL3/SDL_log.h>

DECOMP_SIZE_ASSERT(MxVideoManager, 0x64)

// FUNCTION: LEGO1 0x100be1f0
// STUB: BETA10 0x1012ca40
MxVideoManager::MxVideoManager()
{
	Init();
}

// FUNCTION: LEGO1 0x100be270
// FUNCTION: BETA10 0x1012dde0
void MxVideoManager::UpdateView(MxU32 p_x, MxU32 p_y, MxU32 p_width, MxU32 p_height)
{
}

// FUNCTION: LEGO1 0x100be2a0
// FUNCTION: BETA10 0x1012cad8
MxVideoManager::~MxVideoManager()
{
	Destroy(TRUE);
}

// FUNCTION: LEGO1 0x100be320
// FUNCTION: BETA10 0x1012cb66
MxResult MxVideoManager::Init()
{
	m_pDirectDraw = NULL;
	m_pDirect3D = NULL;
	m_displaySurface = NULL;
	m_region = NULL;
	m_videoParam.SetPalette(NULL);
	m_unk0x60 = FALSE;
	return SUCCESS;
}

// FUNCTION: LEGO1 0x100be340
// FUNCTION: BETA10 0x1012cbca
void MxVideoManager::Destroy(MxBool p_fromDestructor)
{
	if (m_thread) {
		m_thread->Terminate();
		delete m_thread;
	}
	else {
		TickleManager()->UnregisterClient(this);
	}

	ENTER(m_criticalSection);

	if (m_displaySurface) {
		delete m_displaySurface;
	}

	if (m_region) {
		delete m_region;
	}

	if (m_videoParam.GetPalette()) {
		delete m_videoParam.GetPalette();
	}

	if (m_unk0x60) {
		if (m_pDirectDraw) {
			m_pDirectDraw->Release();
		}
		if (m_pDirect3D) {
			m_pDirect3D->Release();
		}
	}

	Init();
	m_criticalSection.Leave();

	if (!p_fromDestructor) {
		MxPresentationManager::Destroy();
	}
}

// FUNCTION: LEGO1 0x100be3e0
// FUNCTION: BETA10 0x1012cdaa
void MxVideoManager::UpdateRegion()
{
	if (m_region->IsEmpty() == FALSE) {
		MxRect32 rect(m_region->GetBoundingRect());
		rect &= m_videoParam.GetRect();

		m_displaySurface
			->Display(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
	}
}

// FUNCTION: LEGO1 0x100be440
// FUNCTION: BETA10 0x1012ce5e
void MxVideoManager::SortPresenterList()
{
	if (m_presenters->GetNumElements() <= 1) {
		return;
	}

	MxPresenterListCursor a(m_presenters);
	MxPresenterListCursor b(m_presenters);
	MxU32 count = m_presenters->GetNumElements() - 1;
	MxBool finished;

	if (count != 0) {
		do {
			a.Reset();
			b.Head();

			finished = TRUE;
			for (MxU32 i = count; i != 0; i--) {
				MxPresenter *presenterA, *presenterB;

				a.Next(presenterA);
				b.Next(presenterB);

				if (presenterA->GetDisplayZ() < presenterB->GetDisplayZ()) {
					a.SetValue(presenterB);
					b.SetValue(presenterA);
					finished = FALSE;
				}
			}
		} while (!finished && --count != 0);
	}
}

// FUNCTION: LEGO1 0x100be600
// STUB: BETA10 0x1012cfbc
MxResult MxVideoManager::VTable0x28(
	MxVideoParam& p_videoParam,
	LPDIRECTDRAW p_pDirectDraw,
	LPDIRECT3D2 p_pDirect3D,
	LPDIRECTDRAWSURFACE p_ddSurface1,
	LPDIRECTDRAWSURFACE p_ddSurface2,
	LPDIRECTDRAWCLIPPER p_ddClipper,
	MxU32 p_frequencyMS,
	MxBool p_createThread
)
{
	MxBool locked = FALSE;
	MxResult status = FAILURE;

	m_unk0x60 = FALSE;

	if (MxPresentationManager::Create() != SUCCESS) {
		goto done;
	}

	ENTER(m_criticalSection);
	locked = TRUE;

	m_videoParam = p_videoParam;
	m_region = new MxRegion();

	if (!m_region) {
		goto done;
	}

	m_pDirectDraw = p_pDirectDraw;
	m_pDirect3D = p_pDirect3D;

	MxPalette* palette;
	if (p_videoParam.GetPalette() == NULL) {
		palette = new MxPalette();
		m_videoParam.SetPalette(palette);

		if (!palette) {
			goto done;
		}
	}
	else {
		palette = p_videoParam.GetPalette()->Clone();
		m_videoParam.SetPalette(palette);

		if (!palette) {
			goto done;
		}
	}

	m_displaySurface = new MxDisplaySurface();
	if (m_displaySurface && m_displaySurface->Init(m_videoParam, p_ddSurface1, p_ddSurface2, p_ddClipper) == SUCCESS) {
		m_displaySurface->SetPalette(m_videoParam.GetPalette());

		if (p_createThread) {
			m_thread = new MxTickleThread(this, p_frequencyMS);

			if (!m_thread || m_thread->Start(0, 0) != SUCCESS) {
				goto done;
			}
		}
		else {
			TickleManager()->RegisterClient(this, p_frequencyMS);
		}

		status = SUCCESS;
	}

done:
	if (status != SUCCESS) {
		Destroy();
	}

	if (locked) {
		m_criticalSection.Leave();
	}

	return status;
}

// FUNCTION: LEGO1 0x100be820
// STUB: BETA10 0x1012d3f1
MxResult MxVideoManager::Create(MxVideoParam& p_videoParam, MxU32 p_frequencyMS, MxBool p_createThread)
{
	MxBool locked = FALSE;
	MxResult status = FAILURE;
	HWND hWnd = NULL;

	m_unk0x60 = TRUE;

	if (MxPresentationManager::Create() != SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MxPresentationManager::Create failed");
		goto done;
	}

	ENTER(m_criticalSection);
	locked = TRUE;

	m_videoParam = p_videoParam;
	m_region = new MxRegion();

	if (!m_region) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MxRegion::MxRegion failed");
		goto done;
	}

	if (DirectDrawCreate(NULL, &m_pDirectDraw, NULL) != DD_OK) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "::DirectDrawCreate failed");
		goto done;
	}

	if (m_pDirectDraw->SetCooperativeLevel(MxOmni::GetInstance()->GetWindowHandle(), DDSCL_NORMAL) != DD_OK) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IDirectDraw::SetCooperativeLevel failed");
		goto done;
	}

	MxPalette* palette;
	if (p_videoParam.GetPalette() == NULL) {
		palette = new MxPalette();
		m_videoParam.SetPalette(palette);

		if (!palette) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MxPalette::MxPalette failed");
			goto done;
		}
	}
	else {
		palette = p_videoParam.GetPalette()->Clone();
		m_videoParam.SetPalette(palette);

		if (!palette) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MxPalette::Clone failed");
			goto done;
		}
	}

	m_displaySurface = new MxDisplaySurface();
	if (m_displaySurface && m_displaySurface->Create(m_videoParam) == SUCCESS) {
		m_displaySurface->SetPalette(m_videoParam.GetPalette());

		if (p_createThread) {
			m_thread = new MxTickleThread(this, p_frequencyMS);

			if (!m_thread || m_thread->Start(0, 0) != SUCCESS) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MxTickleThread::MxTickleThread failed");
				goto done;
			}
		}
		else {
			TickleManager()->RegisterClient(this, p_frequencyMS);
		}

		status = SUCCESS;
	}
	else {
		SDL_LogError(
			SDL_LOG_CATEGORY_APPLICATION,
			"MxDisplaySurface::MxDisplaySurface/MxDisplaySurface::Create failed"
		);
	}

done:
	if (status != SUCCESS) {
		Destroy();
	}

	if (locked) {
		m_criticalSection.Leave();
	}

	return status;
}

// FUNCTION: LEGO1 0x100bea50
// FUNCTION: BETA10 0x1012d85f
void MxVideoManager::Destroy()
{
	Destroy(FALSE);
}

// FUNCTION: LEGO1 0x100bea60
void MxVideoManager::InvalidateRect(MxRect32& p_rect)
{
	ENTER(m_criticalSection);

	if (m_region) {
		m_region->AddRect(p_rect);
	}

	m_criticalSection.Leave();
}

// FUNCTION: LEGO1 0x100bea90
// FUNCTION: BETA10 0x1012d8e3
MxResult MxVideoManager::Tickle()
{
	AUTOLOCK(m_criticalSection);

	SortPresenterList();

	MxPresenter* presenter;
	MxPresenterListCursor cursor(m_presenters);

	while (cursor.Next(presenter)) {
		presenter->Tickle();
	}

	cursor.Reset();

	while (cursor.Next(presenter)) {
		presenter->PutData();
	}

	UpdateRegion();
	m_region->Reset();

	return SUCCESS;
}

// FUNCTION: LEGO1 0x100bebe0
MxResult MxVideoManager::RealizePalette(MxPalette* p_palette)
{
	PALETTEENTRY paletteEntries[256];

	ENTER(m_criticalSection);

	if (p_palette && m_videoParam.GetPalette()) {
		p_palette->GetEntries(paletteEntries);
		m_videoParam.GetPalette()->SetEntries(paletteEntries);
		m_displaySurface->SetPalette(m_videoParam.GetPalette());
	}

	m_criticalSection.Leave();
	return SUCCESS;
}
