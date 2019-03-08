#include "SerialPort.h"
using namespace std;

SerialPort::SerialPort(string name,int bt):baudRate(bt),uartName(name),fd_uart(NULL)
{
}

SerialPort::~SerialPort(){

}
extern "C"{
void* UartRecv_Proc(void* pThis ){
	((SerialPort*)pThis)->UartCallback();
	return (void *)1;
}	
}

void SerialPort::openSerialPort()
{
	fd_uart = Ql_UART_Open(uartName.c_str(), B_115200, FC_NONE);
	cout<<"open uart"<<uartName<<"baudRate:"<<baudRate<<"fd_uart:"<<fd_uart<<endl;

	ST_UARTDCB dcb = {
		B_115200,
		DB_CS8,
		SB_1,
		PB_NONE,
		FC_NONE
	};

	int iRet = Ql_UART_SetDCB(fd_uart, &dcb);

	if (pthread_create(&thrd_uart_rcv, NULL, UartRecv_Proc, this) != 0)
	{
		cout<<"Fail to create uart thread!\n"
	}

	while (fd_uart >=0)
	{
		string strTmp="Ec20 uart test\n";
		iRet = Ql_UART_Write(fd_uart, strTmp.c_str(), strTmp.length());
		cout<<"uart:"<<fd_uart<< ",result:"<<iRet;
		sleep(1);
	}
}

void SerialPort::writeData()
{

}

void* SerialPort::UartCallback()
{
	int iRet;
	fd_set fdset;
	struct timeval timeout = {3, 0};	// timeout 3s
	char buffer[100] = {0};

	while (fd_uart >=0)
	{
		FD_ZERO(&fdset); 
		FD_SET(fd_uart, &fdset); 
		iRet = select(fd_uart + 1, &fdset, NULL, NULL, &timeout);
		if (-1 == iRet)
		{
			//printf("< failed to select >\n");
			exit(-1);
		}
		else if (0 == iRet)
		{// no data in Rx buffer
			//printf("< no data >\n");
			timeout.tv_sec  = 3;
			timeout.tv_usec = 0;
		}
		else
		{// data is in Rx data
			if (FD_ISSET(fd_uart, &fdset)) 
			{
				do {
					memset(buffer, 0x0, sizeof(buffer));
					iRet = Ql_UART_Read(fd_uart, buffer, 100);
					//printf("uart read(uart)=%d:%s\n", iRet, buffer);
				} while (100 == iRet);
			}
		}
	}
	return (void *)1;
}


