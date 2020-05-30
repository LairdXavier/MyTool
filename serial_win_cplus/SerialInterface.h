#ifndef __SerialInterface_H_
#define __SerialInterface_H_
#include <Windows.h>
#include <string>
using namespace std;

class SerialInterface
{
private:
    /* data */
    HANDLE hCom;
    string last_error;
public:
    SerialInterface();
    ~SerialInterface();
public:
    //ͬ����ʽ�򿪴���,������Ĭ����Ϣ
    bool openSyn(string serial_name,unsigned char baud_rate,unsigned char parity, unsigned char byte_size, unsigned char stop_bits);
    //ͬ����ʽ�򿪴���(���Լ����ô��ڲ���)
    bool openSyn(string serial_name);

    //�����Ƽ��Ļ����С
    bool setBufferSize(DWORD inputBuff,DWORD outBuffer);
    //���ó�ʱ
    bool setTimeouts(COMMTIMEOUTS &timeouts);
    //���ô�����Ϣ
    bool setDCB(DCB& dcb);

    //ˢ�»�����
    bool purgeBuff(DWORD flags);
    //ˢ�»�����
    bool flushBuff();
    //д����
    DWORD writeData(char *buffer,int length);
    //������
    DWORD readData(char *buffer,int length);
    //д�ַ���
    DWORD writeStr(string str);
    //�رմ���
    void closeComm();
    //�жϴ����Ƿ��
    bool isOpened();
    //�õ����һ��ʧ�ܵĴ�����Ϣ
    string getSerialLastError();
private:
    //�������һ�εĴ�����Ϣ
    void setSerialLastError(string error_msg);
    //��chu���һ�εĴ�����Ϣ
    void clearSerialLastError();
};

#endif /*__SerialInterface_H_*/