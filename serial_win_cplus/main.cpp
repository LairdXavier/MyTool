#include <iostream>
#include <cstdlib>
#include "SerialInterface.h"
#include <windows.h>
#include <tchar.h>

using namespace std;

int main()
{

    SerialInterface com;

    if(!com.openSyn("COM1", CBR_9600, NOPARITY,8, ONESTOPBIT))
    {
        cout << com.getSerialLastError() << endl;
        getchar();
        return 0;
    }
    //测试发送
    if (com.writeStr("Hello World!"))
    {
        cout << "send success" << endl;
    }
    else
    {
        cout << "send fail" << endl;
    }

    com.closeComm();

    return 0;    
}