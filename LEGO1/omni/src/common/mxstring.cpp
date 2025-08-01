#include "mxstring.h"

#include "decomp.h"
#include "mxmain.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_platform_defines.h>
#include <SDL3/SDL_stdinc.h>
#include <stdlib.h>
#include <string.h>

DECOMP_SIZE_ASSERT(MxString, 0x10)

// FUNCTION: LEGO1 0x100ae200
// FUNCTION: BETA10 0x1012c110
MxString::MxString()
{
	// Set string to one char in length and set that char to null terminator
	this->m_data = new char[1];
	this->m_data[0] = 0;
	this->m_length = 0;
}

// FUNCTION: LEGO1 0x100ae2a0
// FUNCTION: BETA10 0x1012c1a1
MxString::MxString(const MxString& p_str)
{
	this->m_length = p_str.m_length;
	this->m_data = new char[this->m_length + 1];
	strcpy(this->m_data, p_str.m_data);
}

// FUNCTION: LEGO1 0x100ae350
// FUNCTION: BETA10 0x1012c24f
MxString::MxString(const char* p_str)
{
	if (p_str) {
		this->m_length = strlen(p_str);
		this->m_data = new char[this->m_length + 1];
		strcpy(this->m_data, p_str);
	}
	else {
		this->m_data = new char[1];
		this->m_data[0] = 0;
		this->m_length = 0;
	}
}

// FUNCTION: BETA10 0x1012c330
MxString::MxString(const char* p_str, MxU16 p_maxlen)
{
	if (p_str) {
		if (strlen(p_str) <= p_maxlen) {
			this->m_length = strlen(p_str);
		}
		else {
			this->m_length = p_maxlen;
		}

		// Basically strncpy
		this->m_data = new char[this->m_length + 1];
		memcpy(this->m_data, p_str, this->m_length);
		this->m_data[this->m_length] = '\0';
	}
	else {
		this->m_data = new char[1];
		this->m_data[0] = 0;
		this->m_length = 0;
	}
}

// FUNCTION: LEGO1 0x100ae420
// FUNCTION: BETA10 0x1012c45b
MxString::~MxString()
{
	delete[] this->m_data;
}

// FUNCTION: BETA10 0x1012c4de
void MxString::Reverse()
{
	char* start = this->m_data;
	char* end = this->m_data + this->m_length - 1;

	while (start < end) {
		CharSwap(start, end);
		start++;
		end--;
	}
}

// FUNCTION: LEGO1 0x100ae490
// FUNCTION: BETA10 0x1012c537
void MxString::ToUpperCase()
{
	SDL_strupr(this->m_data);
}

// FUNCTION: LEGO1 0x100ae4a0
// FUNCTION: BETA10 0x1012c55c
void MxString::ToLowerCase()
{
	SDL_strlwr(this->m_data);
}

// FUNCTION: LEGO1 0x100ae4b0
// FUNCTION: BETA10 0x1012c581
MxString& MxString::operator=(const MxString& p_str)
{
	if (this->m_data != p_str.m_data) {
		delete[] this->m_data;
		this->m_length = p_str.m_length;
		this->m_data = new char[this->m_length + 1];
		strcpy(this->m_data, p_str.m_data);
	}

	return *this;
}

// FUNCTION: LEGO1 0x100ae510
// FUNCTION: BETA10 0x1012c606
const MxString& MxString::operator=(const char* p_str)
{
	if (this->m_data != p_str) {
		delete[] this->m_data;
		this->m_length = strlen(p_str);
		this->m_data = new char[this->m_length + 1];
		strcpy(this->m_data, p_str);
	}

	return *this;
}

// FUNCTION: BETA10 0x1012c68a
MxString MxString::operator+(const MxString& p_str) const
{
	MxString tmp;
	delete[] tmp.m_data;

	tmp.m_length = p_str.m_length + this->m_length;
	tmp.m_data = new char[tmp.m_length + 1];

	strcpy(tmp.m_data, this->m_data);
	strcpy(tmp.m_data + this->m_length, p_str.m_data);

	return MxString(tmp);
}

// Return type is intentionally just MxString, not MxString&.
// This forces MSVC to add $ReturnUdt$ to the stack for 100% match.
// FUNCTION: LEGO1 0x100ae580
// FUNCTION: BETA10 0x1012c78d
MxString MxString::operator+(const char* p_str) const
{
	// MxString constructor allocates 1 byte for m_data, so free that first
	MxString tmp;
	delete[] tmp.m_data;

	tmp.m_length = strlen(p_str) + this->m_length;
	tmp.m_data = new char[tmp.m_length + 1];

	strcpy(tmp.m_data, this->m_data);
	strcpy(tmp.m_data + this->m_length, p_str);

	return MxString(tmp);
}

// FUNCTION: LEGO1 0x100ae690
// FUNCTION: BETA10 0x1012c92f
MxString& MxString::operator+=(const char* p_str)
{
	int newlen = this->m_length + strlen(p_str);

	char* tmp = new char[newlen + 1];
	strcpy(tmp, this->m_data);
	strcpy(tmp + this->m_length, p_str);

	delete[] this->m_data;
	this->m_data = tmp;
	this->m_length = newlen;

	return *this;
}

// FUNCTION: BETA10 0x1012ca10
void MxString::CharSwap(char* p_a, char* p_b)
{
	char t = *p_a;
	*p_a = *p_b;
	*p_b = t;
}

void MxString::MapPathToFilesystem(char* p_path)
{
	// [library:filesystem]
	// This function is used to map an internal Windows-style path (found in SI files or in the code)
	// to an actual file on disk or CD. We have to account for both Windows path separators and case.
#if !defined(SDL_PLATFORM_WINDOWS)
	char* path = p_path;
	while (*path) {
		if (*path == '\\') {
			*path = '/';
		}

		path++;
	}

	size_t pathLen = SDL_strlen(p_path);

	auto mapPath = [p_path, pathLen](const vector<MxString>& p_files) -> bool {
		for (const MxString& file : p_files) {
			// Test whether file is a suffix of the provided path (case insensitive)
			// If yes, copy the original file system path into p_path.
			for (size_t i = pathLen, j = file.GetLength(); i != 0 && j != 0; i--, j--) {
				if (SDL_tolower(p_path[i - 1]) != SDL_tolower(file.GetData()[j - 1])) {
					break;
				}
				else if (j == 1) {
					SDL_strlcpy(&p_path[i - 1], file.GetData(), file.GetLength() + 1);
					return true;
				}
			}
		}

		return false;
	};

	if (!mapPath(MxOmni::GetHDFiles())) {
		mapPath(MxOmni::GetCDFiles());
	}
#else
	char* path = p_path;
	while (*path) {
		if (*path == '/') {
			*path = '\\';
		}

		path++;
	}
#endif
}
