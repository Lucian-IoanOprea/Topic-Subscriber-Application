#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <string>
#include <iostream>
using namespace std;

/*
 * Macro de verificare a erorilor
 * Exemplu:
 *     int fd = open(file_name, O_RDONLY);
 *     DIE(fd == -1, "open failed");
 */

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#define BUFLEN		1551	// dimensiunea maxima a calupului de date
#define MAX_CLIENTS	120	// numarul maxim de clienti in asteptare
typedef struct{
    char nume_topic[50];
    char tip;
    char payload[1501];
} msg_from_udp;

void display(msg_from_udp message){
	int impartire = 1;
	uint32_t num;
	uint8_t sign1;
	uint32_t num1;
	uint16_t num2;
	uint8_t sign;
	memcpy(&num, message.payload + 1, sizeof(uint32_t));
	memcpy(&sign1, message.payload, sizeof(uint8_t));
switch(message.tip){
	case 5:
				cout<<message.payload<<"\n";
				break;
	case 4: 
				struct sockaddr_in udp_addr;
				memcpy(&udp_addr, message.payload, sizeof(udp_addr));
				cout << inet_ntoa(udp_addr.sin_addr);
				cout<<":";
				cout<<ntohs(udp_addr.sin_port);
				break;
	 case 3: 
				cout <<"- "<<message.nume_topic;
				cout <<"- "<<"STRING";
				cout <<"- "<< message.payload<<"\n";
				break;
	case 0: 	
				if(sign1 == 0){
					cout <<"- "<<message.nume_topic;
					cout<<" - "<<"INT";
					cout <<" - "<<ntohl(num)<<"\n";
				} else {
					cout <<"- "<<message.nume_topic;
					cout<<" - "<<"INT";
					cout <<" - "<<ntohl(num)<<"\n";
				}
				break;
	case 1: 
				uint16_t num4;
				memcpy(&num4, message.payload, sizeof(uint16_t));
				printf("- %s - %s - %g\n", message.nume_topic, "SHORT_REAL", (ntohs(num4)/100.00));
				break;
	default: 
				
				memcpy(&num2, message.payload + 1 + sizeof(uint32_t), sizeof(uint16_t));
				while(num2 > 0) {
					impartire = impartire * 10;
					num2 = num2-1;
				}
				memcpy(&sign, message.payload, sizeof(uint8_t));
				memcpy(&num1, message.payload + 1, sizeof(uint32_t));
				sign == 0 ? printf("- %s - %s - %.10g\n", message.nume_topic, "FLOAT", ((ntohl(num1))*1.0)/impartire) 
				: printf("- %s - %s - -%.10g\n", message.nume_topic, "FLOAT", ((ntohl(num1))*1.0)/impartire);
			
			  break;
    }
}




#endif
