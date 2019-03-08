#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_
#include <iostream>
#include <string>
#include "ql_oe.h"
using namespace std;

class SerialPort{
	public:
		SerialPort(string name,int bt);
		~SerialPort();
		void openSerialPort();
		void writeData();
		void* UartCallback();

	protected:

	private:
		int baudRate;
		string uartName;
		int fd_uart;
		pthread_t thrd_uart_rcv;


};

#endif
