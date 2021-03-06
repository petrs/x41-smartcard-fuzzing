// MinidriverTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cardmod.h"

typedef DWORD(WINAPI *CARDACQUIRECONTEXT)(PCARD_DATA, DWORD);
	
#define ATR "\x3B\xFE\x18\x00\x00\x80\x31\xFE\x45\x45\x73\x74\x45\x49\x44\x20\x76\x65\x72\x20\x31\x2E\x30\xA8"
#define CARDNAME L"CardName"
char *atr = (char *) ATR;
size_t atrlen = sizeof(ATR) - 1;

int main(int argc, char **argv)
{
	HMODULE winscard;
	HMODULE minidriver;
	CARDACQUIRECONTEXT cactx;
	CARD_DATA pcarddata;

	_CARD_FREE_SPACE_INFO space;
	LPSTR files;
	DWORD cbfile;

	memset((void *)&pcarddata, 0, sizeof(pcarddata));

	/* Load Winscard first to make sure the minidriver
	   loads our version */
#if 1
	winscard = LoadLibrary(L"C:\\fuzz\\winscard.dll");
	if (!winscard) {
		printf("Error: Could not load winscard.dll\n");
		return -1;
	}
#endif 
	minidriver = LoadLibrary(L"C:\\fuzz\\minidriver.dll");
	if (!minidriver) {
		printf("Error: Could not load minidriver.dll\n");
		return -1;
	}

	cactx = (CARDACQUIRECONTEXT) GetProcAddress(minidriver, "CardAcquireContext");
	if (!cactx) {
		printf("Error: could not get CardAcquireContext()\n");
		return -1;
	}

	pcarddata.dwVersion = CARD_DATA_VERSION_SEVEN;
	pcarddata.cbAtr = atrlen;
	pcarddata.pbAtr = (PBYTE) atr;
	pcarddata.pwszCardName = (LPWSTR) CARDNAME;
	pcarddata.pfnCspAlloc = (PFN_CSP_ALLOC) malloc;
	pcarddata.pfnCspReAlloc = (PFN_CSP_REALLOC) realloc;
	pcarddata.pfnCspFree = (PFN_CSP_FREE) free;
	pcarddata.hScard = 0x654321;

	if (cactx(&pcarddata, 0) != 0) {
		printf("Error: CardAcquireContext() failed\n");
		return -1;
	}

	if (pcarddata.pfnCardQueryFreeSpace)
		pcarddata.pfnCardQueryFreeSpace(&pcarddata, 0, &space);

	if (pcarddata.pfnCardEnumFiles)
		pcarddata.pfnCardEnumFiles(&pcarddata, (LPSTR) "/", &files, &cbfile, 0);

	printf("Woohoo\n");
    return 0;
}

