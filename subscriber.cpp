#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "helper.h"
#include <bits/stdc++.h>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
using namespace std;



void split(const string& str, const string& delim, vector<string>& parts) {
  size_t start, end = 0;
  while(end < str.size()) {
    start = end;
    while(start < str.size() && (delim.find(str[start]) != string::npos)) {
      start++;  
    }
    end = start;
    while(end < str.size() && (delim.find(str[end]) == string::npos)) {
      end++; 
    }
    if(end-start != 0) {  
      parts.push_back(string(str, start, end-start));
    }
  }
}
void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_address server_port\n", file);
	exit(0);
}

int main(int argc,char *argv[]){
    if(argc != 4){     
	   cout <<" Wrong parameters\n";  
    }
    fd_set read_fds;
	fd_set tmp_fds;
	FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    char buffer[BUFLEN];
    int sockfd, n, ret, flag = 1;
	int len = strlen(argv[1]) + 1;
    char *id_client;

	id_client = (char*)(malloc(len*sizeof(char)));
	strncpy(id_client, argv[1], strlen(argv[1])+1);

    struct sockaddr_in serv_addr;
    msg_from_udp message;
   
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    DIE(sockfd < 0,"socket error");

    serv_addr.sin_family = AF_INET;
	if(strlen(argv[1]) > 10 ){
		cout <<"Id is maximum 10 characters!";
		return 0;
	}
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	DIE(ret < 0, "TCP_NODELAY");
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

    FD_SET(sockfd, &read_fds);
    FD_SET(0,&read_fds);

    ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");
  
 
    set<string> topics;
    n = send(sockfd,id_client,strlen(id_client) + 1,0);
    DIE(n < 0,"send");


    while(1){
        tmp_fds = read_fds;
		memset(buffer, 0, BUFLEN);
		memset(&message, 0, sizeof(message));
        ret = select(sockfd + 1,&tmp_fds,NULL,NULL,NULL);
        if( FD_ISSET(0,&tmp_fds)){
            char aux[120];
			memset(buffer,0,BUFLEN);
			memset(aux,0,sizeof(aux));
            string delim =" ";
            vector<string> parts;
			parts.resize(0);
            fgets(aux,120,stdin);
            split(aux,delim,parts);
            if(parts.size() == 1 && parts[0] == "exit\n"){
              break;
			}
           if( parts.size() == 3 && parts[0] == "subscribe"){
			   
                string input = parts[0]+ " "+ parts[1] +" "+parts[2];
                memcpy(buffer, input.c_str(), BUFLEN);
				n = send(sockfd, buffer, BUFLEN, 0);
				DIE(n < 0, "send");
            }
           else if( parts.size() == 2 && parts[0] == "unsubscribe"){
			   string aux = parts[1];
                string input = parts[0]+ " "+ parts[1];
                memcpy(buffer, input.c_str(), BUFLEN);
				n = send(sockfd, buffer, BUFLEN, 0);
				if( n == 0){
					cout <<"error in send\n";
				}
		
            }
            else{
                cout<<"Wrong input"<<"\n";
            }
            
        }
        else if (FD_ISSET(sockfd,&tmp_fds)){
			n = recv(sockfd, buffer, sizeof(buffer), 0);
			if(n==0) 
				break;
			memcpy(&message, buffer, sizeof(buffer));
			display(message);			
        }

    }

free(id_client);
close(sockfd);

}

