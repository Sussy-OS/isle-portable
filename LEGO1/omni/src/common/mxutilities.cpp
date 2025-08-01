#include "mxutilities.h"

#include "mxcompositepresenter.h"
#include "mxdsaction.h"
#include "mxdsactionlist.h"
#include "mxdsfile.h"
#include "mxdsmultiaction.h"
#include "mxdsobject.h"
#include "mxgeometry.h"
#include "mxpresenterlist.h"

#include <SDL3/SDL_stdinc.h>
#include <assert.h>

LegoSdlEvents g_legoSdlEvents;

// GLOBAL: LEGO1 0x101020e8
void (*g_omniUserMessage)(const char*, MxS32) = NULL;

// FUNCTION: LEGO1 0x100b6e10
// FUNCTION: BETA10 0x10136970
MxBool GetRectIntersection(
	MxS32 p_rect1Width,
	MxS32 p_rect1Height,
	MxS32 p_rect2Width,
	MxS32 p_rect2Height,
	MxS32* p_rect1Left,
	MxS32* p_rect1Top,
	MxS32* p_rect2Left,
	MxS32* p_rect2Top,
	MxS32* p_width,
	MxS32* p_height
)
{
	MxPoint32 rect1Origin(*p_rect1Left, *p_rect1Top);
	MxRect32 rect1(MxPoint32(0, 0), MxSize32(p_rect1Width, p_rect1Height));

	MxPoint32 rect2Origin(*p_rect2Left, *p_rect2Top);
	MxRect32 rect2(MxPoint32(0, 0), MxSize32(p_rect2Width, p_rect2Height));

	MxRect32 rect(0, 0, *p_width, *p_height);
	rect += rect1Origin;

	if (!rect.Intersects(rect1)) {
		return FALSE;
	}

	rect &= rect1;
	rect -= rect1Origin;
	rect += rect2Origin;

	if (!rect.Intersects(rect2)) {
		return FALSE;
	}

	rect &= rect2;
	rect -= rect2Origin;

	*p_rect1Left += rect.GetLeft();
	*p_rect1Top += rect.GetTop();
	*p_rect2Left += rect.GetLeft();
	*p_rect2Top += rect.GetTop();
	*p_width = rect.GetWidth();
	*p_height = rect.GetHeight();
	return TRUE;
}

// FUNCTION: LEGO1 0x100b6ff0
void MakeSourceName(char* p_output, const char* p_input)
{
	const char* cln = strchr(p_input, ':');
	if (cln) {
		p_input = cln + 1;
	}

	strcpy(p_output, p_input);

	SDL_strlwr(p_output);

	char* extLoc = strstr(p_output, ".si");
	if (extLoc) {
		*extLoc = 0;
	}
}

// FUNCTION: LEGO1 0x100b7050
// FUNCTION: BETA10 0x10136c19
MxBool KeyValueStringParse(char* p_output, const char* p_command, const char* p_string)
{
	MxBool didMatch = FALSE;
	assert(p_string);
	assert(p_command);

	MxS16 len = strlen(p_string);
	char* string = new char[len + 1];
	assert(string);
	strcpy(string, p_string);

	const char* delim = ", \t\r\n:";
	for (char* token = strtok(string, delim); token; token = strtok(NULL, delim)) {
		len -= (strlen(token) + 1);

		if (SDL_strcasecmp(token, p_command) == 0) {
			if (p_output && len > 0) {
				char* output = p_output;
				char* cur = &token[strlen(p_command)];
				cur++;
				while (*cur != ',' && *cur != ' ' && *cur != '\0' && *cur != '\t' && *cur != '\n' && *cur != '\r') {
					*output++ = *cur++;
				}
				*output = '\0';
			}

			didMatch = TRUE;
			break;
		}
	}

	delete[] string;
	return didMatch;
}

// FUNCTION: LEGO1 0x100b7170
// FUNCTION: BETA10 0x10136e12
MxBool ContainsPresenter(MxCompositePresenterList& p_presenterList, MxPresenter* p_presenter)
{
	for (MxCompositePresenterList::iterator it = p_presenterList.begin(); it != p_presenterList.end(); it++) {
		if (p_presenter == *it || ((*it)->IsA("MxCompositePresenter") &&
								   ContainsPresenter(*((MxCompositePresenter*) *it)->GetList(), p_presenter))) {
			return TRUE;
		}
	}

	return FALSE;
}

// FUNCTION: LEGO1 0x100b71e0
void OmniError(const char* p_message, MxS32 p_status)
{
	if (g_omniUserMessage) {
		g_omniUserMessage(p_message, p_status);
	}
	else if (p_status) {
		abort();
	}
}

// FUNCTION: LEGO1 0x100b7210
void SetOmniUserMessage(void (*p_omniUserMessage)(const char*, MxS32))
{
	g_omniUserMessage = p_omniUserMessage;
}

// FUNCTION: LEGO1 0x100b7220
// FUNCTION: BETA10 0x10136f37
void ApplyMask(MxDSAction* p_action, MxU32 p_mask, MxBool p_setFlags)
{
	MxU32 oldFlags = p_action->GetFlags();
	MxU32 newFlags;

	if (p_setFlags) {
		newFlags = oldFlags | p_mask;
	}
	else {
		newFlags = oldFlags & ~p_mask;
	}

	p_action->SetFlags(newFlags);

	if (p_action->IsA("MxDSMultiAction")) {
		MxDSActionListCursor cursor(((MxDSMultiAction*) p_action)->GetActionList());
		MxDSAction* action;

		while (cursor.Next(action)) {
			ApplyMask(action, p_mask, p_setFlags);
		}
	}
}
