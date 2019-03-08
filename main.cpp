#include "iostream"
#include "SerialPort.h"
using namespace std;
int main(int argc,char*argv[]){
	cout<<"Ec20 App start"<<endl;
	SerialPort uart("/dev/ttyHS0",115200);
	uart.openSerialPort();
	while(1){
		sleep(3);
	}

	return 0;
}