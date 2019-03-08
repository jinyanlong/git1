#include "CallNet.h"
#include <iostream>

CallNet::CallNet():mDailCbRecv(false){
	mCall.profile_idx=1;
	mCall.reconnect=true;
	mCall.ip_family = QL_DATA_CALL_TYPE_IPV4;	
}
CallNet::~CallNet(){
	
}

void CallNet::data_call_state_callback(ql_data_call_state_s *state)
{
	char command[128];
	if(mCall.profile_idx == state->profile_idx) {
		cout<<"CallNet profile id:"<<mCall.profile_idx;
		if(QL_DATA_CALL_CONNECTED == state->state) {
			cout<<"net is Connected\n"<<"Interface Name:"<<state->name;
			if(QL_DATA_CALL_TYPE_IPV4 == state->ip_family) {
				cout<<"IP address:"<<inet_ntoa(state->v4.ip)<<endl;
				cout<<"Gateway address:", inet_ntoa(state->v4.gateway)<<endl;
				cout<<"Primary DNS address:", inet_ntoa(state->v4.pri_dns)<<endl;
				cout<<"Second DNS address:", inet_ntoa(state->v4.sec_dns)<<endl;

				if(1 != mCall.profile_idx) {
					snprintf(command, sizeof(command), "route add default gw %s",
						inet_ntoa(state->v4.gateway));
					system(command);
					snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf",
						inet_ntoa(state->v4.pri_dns));
					system(command);
					snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf",
						inet_ntoa(state->v4.sec_dns));
					system(command);
				}
			} else {
				char ipv6_buffer[INET6_ADDRSTRLEN];
				inet_ntop(AF_INET6, (void *)&state->v6.ip, ipv6_buffer, INET6_ADDRSTRLEN);
				cout<<"IP address:"<<ipv6_buffer<<endl;
				inet_ntop(AF_INET6, (void *)&state->v6.gateway, ipv6_buffer, INET6_ADDRSTRLEN);
				cout<<"Gateway address:"<<ipv6_buffer<<endl;
				inet_ntop(AF_INET6, (void *)&state->v6.pri_dns, ipv6_buffer, INET6_ADDRSTRLEN);
				cout<<"Primary DNS address:"<<ipv6_buffer<<endl;
				inet_ntop(AF_INET6, (void *)&state->v6.sec_dns, ipv6_buffer, INET6_ADDRSTRLEN);
				cout<<"Second DNS address:"<<ipv6_buffer<<endl;

				if(1 != mCall.profile_idx) {
					inet_ntop(AF_INET6, (void *)&state->v6.gateway, ipv6_buffer, INET6_ADDRSTRLEN);
					snprintf(command, sizeof(command), "ip -6 route del default via %s dev %s",
						ipv6_buffer, state->name);
					system(command);
					
					inet_ntop(AF_INET6, (void *)&state->v6.pri_dns, ipv6_buffer, INET6_ADDRSTRLEN);
					snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf",
						ipv6_buffer);
					system(command);
					
					inet_ntop(AF_INET6, (void *)&state->v6.sec_dns, ipv6_buffer, INET6_ADDRSTRLEN);
					snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf",
						ipv6_buffer);
					system(command);
				}
			}
			printf("\n");
		} else {
			printf("is disconnected, and reason code %x\n", state->err);
		}
		mDailCbRecv = true;
	}
}
int CallNet::connectNet(){
	ql_data_call_error_e err = QL_DATA_CALL_ERROR_NONE;
	ql_data_call_init(data_call_state_callback);
	if(ql_data_call_start(&mCall, &err)) {
		cout<<"start data call failure:"<<err<endl;
		ql_data_call_destroy();
		return -1;
	}
	int count=0;
	while(!mDailCbRecv) {
		if(10 == count) {
			break;
		}
		sleep(1);
		count++;
	}

	printf("start ping www.baidu.com\n");
	system("ping www.baidu.com -c 3");

	printf("get this data call information\n");
	memset(&g_call_info, 0, sizeof(ql_data_call_info_s));
	err = QL_DATA_CALL_ERROR_NONE;
	if(ql_data_call_info_get(g_call.profile_idx, g_call.ip_family, &g_call_info, &err)) {
		cout<<"get data call failure:"<<err<<endl;
		ql_data_call_destroy();
		return -1;
	}
	if(QL_DATA_CALL_TYPE_IPV4 == g_call.ip_family || QL_DATA_CALL_TYPE_IPV4V6 == g_call.ip_family) {
		printf("The profile id %d IPv4 information\n", g_call.profile_idx);
		printf("\tstate:                 %s\n",
			QL_DATA_CALL_CONNECTED == g_call_info.v4.state ? "Connected" : "Disconnected");
		printf("\tIP address:            %s\n", inet_ntoa(g_call_info.v4.addr.ip));
		printf("\tGateway address:       %s\n", inet_ntoa(g_call_info.v4.addr.gateway));
		printf("\tPrimary DNS address:   %s\n", inet_ntoa(g_call_info.v4.addr.pri_dns));
		printf("\tSecond DNS address:    %s\n", inet_ntoa(g_call_info.v4.addr.sec_dns));
		printf("\tPackets Transmit:      %ld\n", g_call_info.v4.stats.pkts_tx);
		printf("\tPackets Receive:       %ld\n", g_call_info.v4.stats.pkts_rx);
		printf("\tBytes Transmit:        %lld\n", g_call_info.v4.stats.bytes_tx);
		printf("\tBytes Receive:        %lld\n", g_call_info.v4.stats.bytes_rx);
		printf("\tPackets drop Transmit: %ld\n", g_call_info.v4.stats.pkts_dropped_tx);
		printf("\tPackets drop Receive:  %ld\n", g_call_info.v4.stats.pkts_dropped_rx);
	}

	if(QL_DATA_CALL_TYPE_IPV6 == g_call.ip_family || QL_DATA_CALL_TYPE_IPV4V6 == g_call.ip_family) {
		char ipv6_buffer[INET6_ADDRSTRLEN];
		printf("The profile id %d IPv6 information\n", g_call.profile_idx);
		printf("\tstate:                 %s\n",
			QL_DATA_CALL_CONNECTED == g_call_info.v6.state ? "Connected" : "Disconnected");
		inet_ntop(AF_INET6, (void *)&g_call_info.v6.addr.ip, ipv6_buffer, INET6_ADDRSTRLEN);
		printf("\tIP address:            %s\n", ipv6_buffer);
		inet_ntop(AF_INET6, (void *)&g_call_info.v6.addr.gateway, ipv6_buffer, INET6_ADDRSTRLEN);
		printf("\tGateway address:       %s\n", ipv6_buffer);
		inet_ntop(AF_INET6, (void *)&g_call_info.v6.addr.pri_dns, ipv6_buffer, INET6_ADDRSTRLEN);
		printf("\tPrimary DNS address:   %s\n", ipv6_buffer);
		inet_ntop(AF_INET6, (void *)&g_call_info.v6.addr.sec_dns, ipv6_buffer, INET6_ADDRSTRLEN);
		printf("\tSecond DNS address:    %s\n", ipv6_buffer);
		printf("\tPackets Transmit:      %ld\n", g_call_info.v6.stats.pkts_tx);
		printf("\tPackets Receive:       %ld\n", g_call_info.v6.stats.pkts_rx);
		printf("\tBytes Transmit:        %lld\n", g_call_info.v6.stats.bytes_tx);
		printf("\tBytes Receive:        %lld\n", g_call_info.v6.stats.bytes_rx);
		printf("\tPackets drop Transmit: %ld\n", g_call_info.v6.stats.pkts_dropped_tx);
		printf("\tPackets drop Receive:  %ld\n", g_call_info.v6.stats.pkts_dropped_rx);
	}
}




// static bool g_dail_cb_recv = false;
// //static int g_err = 0;
// static ql_data_call_s g_call = {
// 	.profile_idx = 1,
// 	.reconnect = true,
// 	.ip_family = QL_DATA_CALL_TYPE_IPV4,
// };
// static ql_data_call_info_s g_call_info;

// struct addr_info {
// 	union {
// 		struct {
// 			struct in_addr ip;
// 			struct in_addr gateway;
// 			struct in_addr pri_dns;
// 			struct in_addr sec_dns;
// 		} v4_addr;

// 		struct {
// 			struct in6_addr ip;
// 			struct in6_addr gateway;
// 			struct in6_addr pri_dns;
// 			struct in6_addr sec_dns;
// 		} v6_addr;
// 	};
// };

// struct addr_info g_info;

// static void data_call_state_callback(ql_data_call_state_s *state)
// {
// 	char command[128];

// 	if(g_call.profile_idx == state->profile_idx) {
// 		printf("profile id %d ", g_call.profile_idx);
// 		printf("IP family %s ", QL_DATA_CALL_TYPE_IPV4 == state->ip_family ? "v4" : "v6");
// 		if(QL_DATA_CALL_CONNECTED == state->state) {
// 			printf("is Connected\n");
// 			printf("Interface Name: %s\n", state->name);
// 			if(QL_DATA_CALL_TYPE_IPV4 == state->ip_family) {
// 				printf("IP address:          %s\n", inet_ntoa(state->v4.ip));
// 				printf("Gateway address:     %s\n", inet_ntoa(state->v4.gateway));
// 				printf("Primary DNS address: %s\n", inet_ntoa(state->v4.pri_dns));
// 				printf("Second DNS address:  %s\n", inet_ntoa(state->v4.sec_dns));

// 				if(1 != g_call.profile_idx) {
// 					snprintf(command, sizeof(command), "route add default gw %s",
// 						inet_ntoa(state->v4.gateway));
// 					system(command);
// 					snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf",
// 						inet_ntoa(state->v4.pri_dns));
// 					system(command);
// 					snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf",
// 						inet_ntoa(state->v4.sec_dns));
// 					system(command);
// 				}
// 			} else {
// 				char ipv6_buffer[INET6_ADDRSTRLEN];
// 				inet_ntop(AF_INET6, (void *)&state->v6.ip, ipv6_buffer, INET6_ADDRSTRLEN);
// 				printf("IP address:          %s\n", ipv6_buffer);
// 				inet_ntop(AF_INET6, (void *)&state->v6.gateway, ipv6_buffer, INET6_ADDRSTRLEN);
// 				printf("Gateway address:     %s\n", ipv6_buffer);
// 				inet_ntop(AF_INET6, (void *)&state->v6.pri_dns, ipv6_buffer, INET6_ADDRSTRLEN);
// 				printf("Primary DNS address: %s\n", ipv6_buffer);
// 				inet_ntop(AF_INET6, (void *)&state->v6.sec_dns, ipv6_buffer, INET6_ADDRSTRLEN);
// 				printf("Second DNS address:  %s\n", ipv6_buffer);

// 				if(1 != g_call.profile_idx) {
// 					inet_ntop(AF_INET6, (void *)&state->v6.gateway, ipv6_buffer, INET6_ADDRSTRLEN);
// 					snprintf(command, sizeof(command), "ip -6 route del default via %s dev %s",
// 						ipv6_buffer, state->name);
// 					system(command);
					
// 					inet_ntop(AF_INET6, (void *)&state->v6.pri_dns, ipv6_buffer, INET6_ADDRSTRLEN);
// 					snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf",
// 						ipv6_buffer);
// 					system(command);
					
// 					inet_ntop(AF_INET6, (void *)&state->v6.sec_dns, ipv6_buffer, INET6_ADDRSTRLEN);
// 					snprintf(command, sizeof(command), "echo 'nameserver %s' >> /etc/resolv.conf",
// 						ipv6_buffer);
// 					system(command);
// 				}
// 			}
// 			printf("\n");
// 		} else {
// 			printf("is disconnected, and reason code %x\n", state->err);
// 		}
// 		g_dail_cb_recv = true;
// 	}
// }

// int main(int argc, char **argv)
// {
// 	int opt = -1;
// 	ql_data_call_error_e err = QL_DATA_CALL_ERROR_NONE;
// 	int count = 0;



// 	ql_data_call_init(data_call_state_callback);
// 	if(ql_data_call_start(&g_call, &err)) {
// 		printf("start data call failure: %x\n", err);
// 		ql_data_call_destroy();
// 		return -1;
// 	}
// 	while(!g_dail_cb_recv) {
// 		if(10 == count) {
// 			break;
// 		}
// 		sleep(1);
// 		count++;
// 	}

// 	printf("start ping www.baidu.com\n");
// 	system("ping www.baidu.com -c 5");

// 	printf("get this data call information\n");
// 	memset(&g_call_info, 0, sizeof(ql_data_call_info_s));
// 	if(ql_data_call_info_get(g_call.profile_idx, g_call.ip_family, &g_call_info, &err)) {
// 		printf("get data call failure: %d\n", err);
// 		ql_data_call_destroy();
// 		return -1;
// 	}
// 	if(QL_DATA_CALL_TYPE_IPV4 == g_call.ip_family || QL_DATA_CALL_TYPE_IPV4V6 == g_call.ip_family) {
// 		printf("The profile id %d IPv4 information\n", g_call.profile_idx);
// 		printf("\tstate:                 %s\n",
// 			QL_DATA_CALL_CONNECTED == g_call_info.v4.state ? "Connected" : "Disconnected");
// 		printf("\tIP address:            %s\n", inet_ntoa(g_call_info.v4.addr.ip));
// 		printf("\tGateway address:       %s\n", inet_ntoa(g_call_info.v4.addr.gateway));
// 		printf("\tPrimary DNS address:   %s\n", inet_ntoa(g_call_info.v4.addr.pri_dns));
// 		printf("\tSecond DNS address:    %s\n", inet_ntoa(g_call_info.v4.addr.sec_dns));
// 		printf("\tPackets Transmit:      %ld\n", g_call_info.v4.stats.pkts_tx);
// 		printf("\tPackets Receive:       %ld\n", g_call_info.v4.stats.pkts_rx);
// 		printf("\tBytes Transmit:        %lld\n", g_call_info.v4.stats.bytes_tx);
// 		printf("\tBytes Receive:        %lld\n", g_call_info.v4.stats.bytes_rx);
// 		printf("\tPackets drop Transmit: %ld\n", g_call_info.v4.stats.pkts_dropped_tx);
// 		printf("\tPackets drop Receive:  %ld\n", g_call_info.v4.stats.pkts_dropped_rx);
// 	}

// 	if(QL_DATA_CALL_TYPE_IPV6 == g_call.ip_family || QL_DATA_CALL_TYPE_IPV4V6 == g_call.ip_family) {
// 		char ipv6_buffer[INET6_ADDRSTRLEN];
// 		printf("The profile id %d IPv6 information\n", g_call.profile_idx);
// 		printf("\tstate:                 %s\n",
// 			QL_DATA_CALL_CONNECTED == g_call_info.v6.state ? "Connected" : "Disconnected");
// 		inet_ntop(AF_INET6, (void *)&g_call_info.v6.addr.ip, ipv6_buffer, INET6_ADDRSTRLEN);
// 		printf("\tIP address:            %s\n", ipv6_buffer);
// 		inet_ntop(AF_INET6, (void *)&g_call_info.v6.addr.gateway, ipv6_buffer, INET6_ADDRSTRLEN);
// 		printf("\tGateway address:       %s\n", ipv6_buffer);
// 		inet_ntop(AF_INET6, (void *)&g_call_info.v6.addr.pri_dns, ipv6_buffer, INET6_ADDRSTRLEN);
// 		printf("\tPrimary DNS address:   %s\n", ipv6_buffer);
// 		inet_ntop(AF_INET6, (void *)&g_call_info.v6.addr.sec_dns, ipv6_buffer, INET6_ADDRSTRLEN);
// 		printf("\tSecond DNS address:    %s\n", ipv6_buffer);
// 		printf("\tPackets Transmit:      %ld\n", g_call_info.v6.stats.pkts_tx);
// 		printf("\tPackets Receive:       %ld\n", g_call_info.v6.stats.pkts_rx);
// 		printf("\tBytes Transmit:        %lld\n", g_call_info.v6.stats.bytes_tx);
// 		printf("\tBytes Receive:        %lld\n", g_call_info.v6.stats.bytes_rx);
// 		printf("\tPackets drop Transmit: %ld\n", g_call_info.v6.stats.pkts_dropped_tx);
// 		printf("\tPackets drop Receive:  %ld\n", g_call_info.v6.stats.pkts_dropped_rx);
// 	}
	
// #if 1
// 	while (1) {
// 		sleep(1);
// 	}
// #else
// 	printf("turn off for about 5 seconds\n");
// 	sleep(5);

// 	g_dail_cb_recv = false;
// 	if(ql_data_call_stop(g_call.profile_idx, g_call.ip_family, &err)) {
// 		printf("stop data call failure: %d\n", err);
// 		ql_data_call_destroy();
// 		return -1;
// 	}
// 	count = 0;
// 	while(!g_dail_cb_recv) {
// 		if(5 == count) {
// 			break;
// 		}
// 		sleep(1);
// 		count++;
// 	}
// 	printf("exit process wait about 1 seconds\n");
// 	sleep(1);
// 	ql_data_call_destroy();
// #endif
// 	return 0;
// }

