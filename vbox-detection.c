#include <windows.h>
#include <winreg.h>
#include <string.h>

/*
 Virtual box detection checking an register key
 c0der => MMxM [http://hc0der.blogspot.com]
*/

int CheckVbox(void){

	HKEY key;
	DWORD tipo = REG_MULTI_SZ, size = 150;
	BYTE value[500];

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System"), 0, KEY_READ, &key) != ERROR_SUCCESS){
		return 0;
	}
 
	if( RegQueryValueEx(key, TEXT("SystemBiosVersion"), 0, &tipo, value, &size) != ERROR_SUCCESS){
		return 0;
	}
 
	RegCloseKey(key);
 
	if(strstr(value,"VBOX")){
		return 1;
	} else {
		return 0;
	}

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	if(CheckVbox()){
		MessageBox(NULL, "Virtual box detected !!!", "Virtual Box", MB_ICONEXCLAMATION);
	} else {
		MessageBox(NULL, "Virtual box not detected !!!", "Virtual Box", MB_OK);
	}

	return 0;
}
