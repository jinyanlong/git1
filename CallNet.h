#ifndef _CALLNET_H_
#define _CALLNET_H_
#include "ql_oe.h"
#include "ql_wwan_v2.h"
using namespace std;

class CallNet{
	public:
		CallNet();
		~CallNet();
		int connectNet();
	protected:
		void data_call_state_callback(ql_data_call_state_s *state);

	private:
		ql_data_call_s mCall;
		bool mDailCbRecv;
}
