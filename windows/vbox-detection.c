#include <windows.h>
#include <winreg.h>
#include <string.h>

// Virtual box detection through the windows register

int checkVbox(void){
    int ret = 0;

    DWORD ktype = REG_MULTI_SZ, size = 128;
    BYTE value[256];
    HKEY key;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System"), 0,
        KEY_READ, &key) != ERROR_SUCCESS){
        goto end;
    }

    if(RegQueryValueEx(key, TEXT("VideoBiosVersion"), 0,
        &ktype, value, &size) != ERROR_SUCCESS){
        goto end;
    }

    RegCloseKey(key);

    if(strstr(value,"VirtualBox")){
        ret = 1;
    }

    end:
    return ret;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    LPSTR msg;
    UINT opt;

    if(checkVbox()){
        msg = "Virtual box detected !!!";
        opt = MB_ICONEXCLAMATION;
    } else {
        msg = "Virtual box not detected !!!";
        opt = MB_OK;
    }

    MessageBox(NULL, msg, "Virtual Box", opt);

    return 0;
}
