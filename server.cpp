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

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

bool check_topic(unordered_map<string,set<string>> &topics,string a){
	for( auto it : topics){
		if( it.first == a)
		return true;
	}
	return false;
}

int check_online(unordered_map<int,string> &online,string a){
	for( auto it : online){
		if( it.second == a)
		return it.first;
	}
	return -1;
}
bool check_offline(unordered_map<string,set<string>> &offline,string nume,string a){
	for( auto it : offline[nume]){
		if( it == a)
		return true;
	}
	return false;
}
bool check_message(unordered_map<string,vector<msg_from_udp>> &stock,string a){
	for( auto it : stock){
		if( it.first == a)
		return true;
	}
	return false;
}
void send_message(int sock,int type,string tosend){
	msg_from_udp msg;
	char buffer[BUFLEN];
	memset(&msg,0,sizeof(msg_from_udp));
	memset(buffer,0,BUFLEN);
	msg.tip = (uint8_t) 5;
    if( type == 0){
	memcpy(msg.payload, "This id is used!", strlen("This id is used!")+1);
	memcpy(buffer, &msg, sizeof(msg));
	int n = send(sock,buffer,sizeof(buffer),0);
	if( n < 0)
	cout <<"send error";
	}
	else if(type == 1){
	memcpy(msg.payload, "Not subscribed!", strlen("Not subscribed!")+1);
	memcpy(buffer, &msg, sizeof(msg));
	int n = send(sock,buffer,sizeof(buffer),0);
	if( n < 0)
	cout <<"send error";
	}
	else{
	memcpy(msg.payload, tosend.c_str(), strlen(tosend.c_str())+1);
	memcpy(buffer, &msg, sizeof(msg));
	int n = send(sock,buffer,sizeof(buffer),0);
	if( n < 0)
	cout <<"send error";

	}
	
}
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

int main(int argc, char *argv[])
{	
	if (argc < 2) {
		usage(argv[0]);
	}
	
	char buffer[BUFLEN], buffer1[BUFLEN];
	struct sockaddr_in serv_addr, servu_addr, cli_addr;
	fd_set read_fds;	
	fd_set tmp_fds;	
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	string response;
	int n, ret, ok = 0,sockfdt, sockfdu, newsockfd, portno, fdmax, flag = 1;
    unordered_map<int,string> client_ids;
    unordered_map<string,set<string>> topics;
    unordered_map<string,set<string>> store_and_forward;
	socklen_t clilen;
    socklen_t socklen;


    sockfdt = socket(AF_INET,SOCK_STREAM,0);
    DIE(sockfdt < 0,"socket");

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	ret = setsockopt(sockfdt, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	DIE(ret < 0, "TCP_NODELAY");
	
	serv_addr.sin_family = AF_INET;
	portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
   	msg_from_udp mymessage, ip_port;
	ret = bind(sockfdt, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
  	DIE(ret < 0, "bind");
   

	sockfdu = socket(AF_INET, SOCK_DGRAM, 0); 
	DIE(sockfdu < 0, "socket");
    socklen = (socklen_t)sizeof(struct sockaddr_in);



	memset((char *) &servu_addr, 0, sizeof(servu_addr));
	servu_addr.sin_addr.s_addr = INADDR_ANY;
	servu_addr.sin_family = AF_INET;
	servu_addr.sin_port = htons(portno);

	FD_SET(sockfdt,&read_fds);
	FD_SET(sockfdu, &read_fds);

	ret = bind(sockfdu, (struct sockaddr *) &servu_addr, socklen);
	DIE(ret < 0, "bind");

	ret = listen(sockfdt, MAX_CLIENTS);
	DIE(ret < 0, "listen");
    

	fdmax = max(sockfdu,sockfdt);
    FD_SET(0,&read_fds);

    unordered_map<string, vector<msg_from_udp>> stock;
    unordered_map<string,vector<sockaddr_in>> udp_info;

	while (1) {
        tmp_fds = read_fds;
		if(ok == 1)
		 break;
        ret = select(fdmax + 1,&tmp_fds,NULL,NULL,NULL);
        DIE(ret<0,"select error");
            for(int i = 0; i <= fdmax;i++){
				memset(&ip_port,0,sizeof(ip_port));
				memset(&mymessage,0,sizeof(mymessage));
				memset(buffer1, 0, BUFLEN);
				memset(buffer,0,BUFLEN);
                if( FD_ISSET(i,&tmp_fds)){
                        if(i == 0){
                         
                            fgets(buffer,BUFLEN,stdin);
                            if(strncmp(buffer,"exit",4) == 0){
                             {
								 ok = 1;
								 break;
							 }
                            }
                            else{
                                cout<<"The only command is exit\n";
                            }
                        }
                        else if(i == sockfdu){
                            n = recvfrom(i,buffer,sizeof(buffer),0,(struct sockaddr *)&servu_addr,&socklen);
							if( n < 0){
							cout << "receive error\n";
							}
							memcpy(&mymessage,buffer,sizeof(buffer));
							if(check_topic(topics,mymessage.nume_topic) == true ){
								for( auto it : topics[mymessage.nume_topic]){
									if(check_online(client_ids,it) != -1 ){
										
										ip_port.tip = (uint8_t) 4;
										memcpy(ip_port.payload, &servu_addr, sizeof(servu_addr));
										
										memcpy(buffer1, &ip_port, sizeof(ip_port));
										
										n = send(check_online(client_ids,it),buffer1,sizeof(buffer),0);
										DIE(n < 0,"send error");

										n = send(check_online(client_ids,it),buffer,sizeof(buffer),0);
										DIE(n < 0,"send error");

									}
									else if( check_offline(store_and_forward,mymessage.nume_topic,it)){
										stock[it].push_back(mymessage);
										udp_info[it].push_back(servu_addr);								
									}
								}

							}

                        }
					else if(i == sockfdt){
					clilen = sizeof(cli_addr);
					newsockfd = accept(sockfdt, (struct sockaddr *) &cli_addr, &clilen);
					FD_SET(newsockfd,&read_fds);
					DIE(newsockfd < 0, "accept error");
					
					n = recv(newsockfd,buffer,sizeof(buffer),0);
					string id(buffer);
				
						if(check_online(client_ids,id)!= -1){
							string a =" ";
						  send_message(newsockfd,0,a);
							close(newsockfd);
							FD_CLR(newsockfd, &read_fds);
							
						}
						else{
							fdmax = max(fdmax,newsockfd);
							client_ids.insert(make_pair(newsockfd,id));
							printf("New client %s connected from %s:%d.\n", buffer,inet_ntoa(cli_addr.sin_addr), 
															ntohs(cli_addr.sin_port));
							if(check_message(stock,id) == true ){
								while( stock[id].size() != 0 ){
									memcpy(buffer,&(stock[id].back()),sizeof(msg_from_udp));
									ip_port.tip = (uint8_t) 4;
									memcpy(ip_port.payload, &(udp_info[id].back()), sizeof(udp_info[id].back()));
									memcpy(buffer1, &ip_port, sizeof(ip_port));	
									n = send(newsockfd, buffer1, sizeof(buffer), 0);
									int n1 = send(newsockfd, buffer, sizeof(buffer), 0);
									if( n == 0 || n1 == 0)
									cout <<"Eroare la trimitere\n";
									stock[id].pop_back();
									udp_info[id].pop_back();
								}
							}
						}
					}
					else{
						n = recv(i,buffer,sizeof(buffer),0);
						DIE(n < 0,"recv");

						if( n == 0){
						close(i);
						FD_CLR(i,&read_fds);
						cout <<"Client with id "<< client_ids[i] <<" "<<"disconnected\n";
						client_ids.erase(i);
						}
						else{
							vector<string> parts;		
							parts.resize(0);
							string delim = " ";
						    split(buffer,delim,parts);
							int ok = 0;
							if(parts.size() == 2 ){
								parts[1].pop_back();
							}
							if( check_topic(topics,parts[1]) == true ){
								if( parts[0] == "subscribe"){
									topics[parts[1]].insert(client_ids[i]);
									ok = 1;
								}
								else if( parts[0] == "unsubscribe"){
									bool is = false;
									for(auto it : topics[parts[1]])
									   if( it == client_ids[i])
									      is = true;
									if( is == true){
									topics[parts[1]].erase(client_ids[i]);
									store_and_forward[parts[1]].erase(client_ids[i]);
									ok = 1;
									}
									else{
										string a = " ";
									send_message(i,1,a);
									}
								}
							}
						else{
						
							if(parts[0] == "subscribe" ) {
								topics[parts[1]].insert(client_ids[i]);
							}		

						}
						if( parts.size() == 3){
						if( parts[2] == "1\n"){
							store_and_forward[parts[1]].insert(client_ids[i]);
						}
				      
				   }
				   if( ok == 1){
							string response = parts[0] + "d " + parts[1]; 
							int type = 2;
							send_message(i,type,response);
				   }
				}

			}

                }
            }
		
			
	}
	close(sockfdt);
	close(sockfdu);

	return 0;
}