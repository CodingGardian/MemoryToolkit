#pragma once

// stuff for windows
#include "definitions.h"
#include <windows.h>

// how the fuck do I change char to wchar without messing up cross-compatability

#define WAPI_COMPONENT : public IWindowsAPIComponent

class IWindowsAPIComponent {
public:
	virtual ~IWindowsAPIComponent() = 0;

};

// TODO: Seperate... good enough for now tho
class WAPIFileIO WAPI_COMPONENT {
public:
	WAPIFileIO();
	~WAPIFileIO();

	HANDLE OpenFile(const wchar_t*);
	HANDLE OpenFileA(const wchar_t*, DWORD, DWORD, DWORD, DWORD);

	UINT32 GetFileSize2(HANDLE);


};

class WAPIThread WAPI_COMPONENT {
public:
	WAPIThread();
	~WAPIThread();

	// TODO: finish later
};