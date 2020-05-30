#include "SerialInterface.h"

/*******************************************************************
* ���ƣ� openSyn
* ���ܣ� ͬ����ʽ�򿪴���,������Ĭ����Ϣ
* ������
    serial_name����������
    baud_rate  �������ʣ�ȡֵ����
        ......
        CBR_9600    9600bps
        CBR_14400   14400bps
        ......
    parity     ��У�鷽ʽ
        EVENPARITY  żУ��
        MARKPARITY  ���У��
        NOPARITY    ��У��
        ODDPARITY   ��У��
        SPACEPARITY �ո�У��
    byte_size  ������λ��С
        4��5��6��7��8
    stop_bits  ��ֹͣλ
        ONESTOPBIT      1��ֹͣλ
        ONE5STOPBITS    1.5��ֹͣλ
        TWOSTOPBITS     2��ֹͣλ
* ���أ� ��ȷ����Ϊture�����󷵻�Ϊfalse
*******************************************************************/
bool SerialInterface::openSyn(string serial_name, unsigned char baud_rate, unsigned char parity, unsigned char byte_size, unsigned char stop_bits)
{
    if (!openSyn(serial_name))
        return false;
    DCB dcb;

    if (false == GetCommState(hCom, &dcb))//��õ�ǰ���ڵ�������Ϣ
    {
        setSerialLastError("SerialInterface::open() : GetCommState Error");
        return false;
    }

    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = baud_rate;
    dcb.Parity = parity;
    dcb.ByteSize = byte_size;
    dcb.StopBits = stop_bits;

    if (false == SetCommState(hCom, &dcb))//��DCB�ṹ�������ô��ж˿���Ϣ
    {
        setSerialLastError("SerialInterface::open() : SetCommState Error");
        return false;
    }

    //��ʱ����
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = MAXDWORD; //�������ʱ
    // �Ѽ����ʱ��Ϊ��󣬰��ܳ�ʱ��Ϊ0������ReadFile�������ز���ɲ��� 

    timeouts.ReadTotalTimeoutMultiplier = 0; //��ʱ��ϵ��
    timeouts.ReadTotalTimeoutConstant = 0; //��ʱ�䳣��
    timeouts.WriteTotalTimeoutMultiplier = 50; // дʱ��ϵ��
    timeouts.WriteTotalTimeoutConstant = 2000; //дʱ�䳣��
    //�ܵĶ�/д��ʱʱ�� = Read(Write)TotalTimeoutMultiplier x Ҫ��/д���ֽ��� + Read(Write)TotalTimeoutConstant. 
    if (false==SetCommTimeouts(hCom, &timeouts))
    {
        setSerialLastError("SerialInterface::open() : SetCommTimeouts Error");
        return false;
    }



    //��ջ�������Ϊ��д������׼��
    if (false == PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT))
    {
        setSerialLastError("SerialInterface::open() : PurgeComm Error");
        return false;
    }
    return true;
}
/*******************************************************************
* ���ƣ� openSyn
* ���ܣ� ͬ����ʽ�򿪴���(���Լ����ô��ڲ���)
* ������
    serial_name����������
* ���أ� ��ȷ����Ϊture�����󷵻�Ϊfalse
*******************************************************************/
bool SerialInterface::openSyn(string serial_name)
{
	hCom = CreateFileA(
		serial_name.data(),//��ͨ�ļ��������豸�ļ����������Ǵ�����
		GENERIC_READ | GENERIC_WRITE,//����ģʽ,����д
		0,//����ģʽ����ռģʽ
		NULL,//ָ��ȫ���Ե�ָ��,��ʹ�ã���NULL
		OPEN_EXISTING,//��δ���,�ڴ����б�������ΪOPEN_EXISTING����ʾ���ܴ����¶˿�ֻ�ܴ����еĶ˿ڡ�
		FILE_ATTRIBUTE_NORMAL,//�ļ�����,ʹ��Ĭ������FILE_ATTRIBUTE_NORMAL��
		NULL//���ڸ����ļ����,ͨ�������������ΪNULL��Ϊ�ձ�ʾ��ʹ��ģ��
	);


	if (INVALID_HANDLE_VALUE == hCom)//�����ж�
	{
		hCom = NULL;
        setSerialLastError("open():CreateFileA Error!");
		return false;
	}
	return true;
}
/*******************************************************************
* ���ƣ� closeComm
* ���ܣ� �رմ���
* ������ ��
* ���أ� ��ȷ����Ϊture�����󷵻�Ϊfalse
*******************************************************************/
void SerialInterface::closeComm()
{
    if(NULL==hCom)
        return;
    CloseHandle(hCom);
    hCom=NULL;
}
/*******************************************************************
* ���ƣ� closeComm
* ���ܣ� �жϴ����Ƿ��
* ������ ��
* ���أ� ��ȷ����Ϊture�����󷵻�Ϊfalse
*******************************************************************/
bool SerialInterface::isOpened()
{
    return NULL == hCom ? false : true;
}

DWORD SerialInterface::readData(char *buffer,int length)
{
    DWORD writeSize=0;

    bool ret=false;

    ret=ReadFile(hCom,buffer,length,&writeSize,NULL);

    if(false==ret)
        return 0;

    return writeSize;
}


DWORD SerialInterface::writeData(char *buffer,int length)
{
    DWORD writeSize=0;

    bool ret=false;

    ret=WriteFile(hCom,buffer,length,&writeSize,NULL);

    if(false==ret)
        return 0;

    return writeSize;

}


DWORD SerialInterface::writeStr(string str)
{
	bool ret = false;

	DWORD writeSize = 0;


	ret = WriteFile(hCom, str.data(), str.length(), &writeSize, NULL);

    if (0 == ret)
    {
        last_error = "Error By writeStr(string str)";
        return 0;
    }
		

    last_error = "";
	return writeSize;

}
/*******************************************************************
* ���ƣ� setTimeouts
* ���ܣ� ���ó�ʱ
* ������
    timeouts����ʱ���õ�COMMTIMEOUTS�ṹ��
* ���أ� ��ȷ����Ϊture�����󷵻�Ϊfalse
*******************************************************************/
bool SerialInterface::setTimeouts(COMMTIMEOUTS &timeouts)
{

    if (false == SetCommTimeouts(hCom, &timeouts))
    {
        setSerialLastError("SerialInterface::setTimeouts() : SetCommTimeouts Error");
        return false;
    }
    return true;
}
/*******************************************************************
* ���ƣ� setDCB
* ���ܣ� ���ô�����Ϣ
* ������
    dcb��������Ϣ���õ�DCB�ṹ��
* ���أ� ��ȷ����Ϊture�����󷵻�Ϊfalse
*******************************************************************/
bool SerialInterface::setDCB(DCB& dcb)
{
    if (false == SetCommState(hCom, &dcb))
    {
        setSerialLastError("SerialInterface::setDCB() : SetCommState Error");
        return false;
    }
    return true;
}
/*******************************************************************
* ���ƣ� purgeBuff
* ���ܣ� ˢ�»�����
* ������ 
    flags����Ҫ��ɵĲ�����ȡֵ����
        PURGE_RXABORT ��ֹ����δ��ɵ��ص���ȡ�������������أ���ʹ��ȡ������δ��ɡ�
        PURGE_RXCLEAR ������뻺����������豸����������һ������
        PURGE_TXABORT ��ֹ����δ��ɵ��ص�д�������������أ���ʹд������δ��ɡ�
        PURGE_TXCLEAR ������������������豸����������һ������
* ���أ� ��ȷ����Ϊture�����󷵻�Ϊfalse
* ��ʾ��PurgeComm()���������ڶ�д������ͬʱ����ջ���������Ӧ�ó����ڶ�д����ʱ
����PurgeComm()���������ܱ�֤�������ڵ������ַ��������͡�
*******************************************************************/
bool SerialInterface::purgeBuff(DWORD flags)
{

    if (false == PurgeComm(hCom, flags))
    {
        setSerialLastError("SerialInterface::purgeBuff() : PurgeComm Error");
        return false;
    }
    return true;
}
/*******************************************************************
* ���ƣ�flushBuff
* ���ܣ�ˢ�»�����
* ��������
* ���أ���ȷ����Ϊture�����󷵻�Ϊfalse
* ��ʾ���ú���ֻ���������Ƶ�֧�䣬���ܳ�ʱ���Ƶ�֧�䣬�������е�д������ɺ�ŷ��ء�
*******************************************************************/
bool SerialInterface::flushBuff()
{
    if(FlushFileBuffers(hCom))
    {
        setSerialLastError("SerialInterface::flushBuff() : FlushFileBuffers Error");
        return false;
    }
    return true;
}
/*******************************************************************
* ���ƣ� setBufferSize
* ���ܣ� �����Ƽ��Ļ����С
* ������
    inputBuff�����뻺���С
    outBuffer����������С
* ���أ� ��ȷ����Ϊture�����󷵻�Ϊfalse
*******************************************************************/
bool SerialInterface::setBufferSize(DWORD inputBuff,DWORD outBuffer)
{
    if(inputBuff<=0||outBuffer<=0)
        return false;
    
    return SetupComm(hCom,inputBuff,outBuffer);
}
/*******************************************************************
* ���ƣ� getSerialLastError
* ���ܣ� �õ����һ��ʧ�ܵĴ�����Ϣ
* ������ ��
* ���أ� ��������:string,������Ϣ
*******************************************************************/
string SerialInterface::getSerialLastError()
{
    return last_error;
}

void SerialInterface::setSerialLastError(string error_msg)
{
    last_error = error_msg;
}
void SerialInterface::clearSerialLastError()
{
    last_error = "";
}
SerialInterface::SerialInterface()
{
    hCom=NULL;
}
SerialInterface::~SerialInterface()
{

}