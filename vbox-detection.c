#include <windows.h>
#include <winreg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 
// c0der => MMxM [http://hc0der.blogspot.com]
 
int check_vbox(){
	int result;
	HKEY key;
	long check,ccheck;
	char *value = malloc(151);
	DWORD tipo = REG_MULTI_SZ,size = 150;
 
	check = RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DESCRIPTION\\System"),0,KEY_READ,&key);
	if(check != ERROR_SUCCESS)
		exit(1);
 
	ccheck = RegQueryValueEx(key,TEXT("SystemBiosVersion"),0,&tipo,value,&size);
	if(ccheck != ERROR_SUCCESS)
		exit(1);
 
	RegCloseKey(key);
 
	if(strstr(value,"VBOX"))
		result = 1;
	else
		result = 0;
 
	free(value);
	return result;
}
 
int main(){
	FreeConsole();
	int c = check_vbox();
	if(c == 1)
		MessageBox(NULL, "Virtual box detected !!!", "Virtual Box", MB_ICONEXCLAMATION);
	if(c == 0)
		MessageBox(NULL, "Virtual box not detected !!!", "Virtual Box", MB_OK);
}
