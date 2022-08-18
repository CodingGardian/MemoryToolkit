#pragma once

// stuff for windows
#include "definitions.h"
#include <windows.h>

#include "BaseInterface.h"

// how the fuck do I change char to wchar without messing up cross-compatability

#define WAPI_COMPONENT : public IWindowsAPIComponent

using namespace BASEINTERFACE;

class IWindowsAPIComponent {//: public IBaseInterface {
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

	int load() { return 0; }
	int unload() { return 0; }
};

class WAPIThread WAPI_COMPONENT {
public:
	WAPIThread();
	~WAPIThread();


	int load();

	int unload();
	// TODO: finish later
};