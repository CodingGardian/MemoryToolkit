#include "WinAPIComponent.h"
#include <iostream>

WAPIFileIO::WAPIFileIO() {}

WAPIFileIO::~WAPIFileIO() {}

HANDLE WAPIFileIO::OpenFile(const wchar_t* c) {
	HANDLE file = CreateFile(c, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (GetLastError() == ERROR_FILE_NOT_FOUND) { // this is bad TODO: fix
		std::cout << "ERROR: FILE NOT FOUND" << std::endl;
		exit(-1);
	}
	return file;
}

UINT32 WAPIFileIO::GetFileSize2(HANDLE hfile) { // maybe use GetFileSizeEx? It's not likely that files will get that big, but you never know
	return LOWORD(GetFileSize(hfile, NULL));
}