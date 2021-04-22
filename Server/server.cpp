#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/ip.h>
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<vector>
#include<thread>
#include<map>
#include<cstring>
#include<iterator>
#include<mutex>
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include "server.h"
using namespace std;
struct game_description game;
struct message m;
int connection_checker = 0;
mutex mtx;
map<int,Player> Clients;
map<int,Player>::iterator it;
map<string,Room> Rooms;
map<string,Room>::iterator ir;
vector<Player> Monsters;
vector<Player> Dead_Players;
void connection_check(int sig){
	//cout << "Pipe Error\n";
	connection_checker = 1;
}

void* client_thread(void *args){
	//mtx.unlock();
	int skt = (int)(size_t)args;
	unsigned short type = 0;
	Player character;
	Game_Packet(&game,skt);
	//printf("Game test = %s\n",game.description.c_str());
	cout << "Socket number = " << skt << endl;
	struct sigaction pipe;
        pipe.sa_handler = connection_check;
        sigaction(SIGPIPE, &pipe, 0);
	for(;;){
		int started = 16 & character.flag;
		int alive = 128 & character.flag;
		recv(skt, &type ,1,MSG_WAITALL);
		if(type == 1){
			message(skt,&Clients,&m);
		}
		else if(type == 2){
			if(started && alive){
				mtx.lock();
				Change_Room(&character,&Rooms,skt,&Clients);
				mtx.unlock();
				//Send_Room(&character,&Rooms,skt);
				//Connections(&Rooms,skt,character.room_num);
				mtx.lock();
				Write_Character(&character,skt,&Clients,&Monsters,&Dead_Players,&Rooms);
				mtx.unlock();
			}
			else{
				error(skt,type,5);
			}
		}
		else if(type == 3){
			mtx.lock();
			if(started && alive){
				fight(&character,skt,&Rooms,&Clients,&Monsters,&Dead_Players);
			}
			else{
				error(skt,type,5);
			}
			mtx.unlock();
		}
		else if(type == 4){
			if(started && alive){
				//error(skt,type,8);
				pvp(&character,skt,&Rooms,&Clients,&Dead_Players,&Monsters);
			}
			else{
				error(skt,type,5);
			}
		}
		else if(type == 5){
			if(started && alive){
				loot(&character,skt,&Rooms,&Clients,&Monsters,&Dead_Players);
			}
                        else{
                                error(skt,type,5);
			}
                        //mtx.unlock();
		}
		else if(type == 6){
			if(character.flag == 200){
				character.flag = 152;
				mtx.lock();
				Write_Character(&character,skt,&Clients,&Monsters,&Dead_Players,&Rooms);
				mtx.unlock();
				mtx.lock();
				Send_Room(&character,&Rooms,skt);
				mtx.unlock();
				mtx.lock();
				Connections(&Rooms,skt,character.room_num);
				mtx.unlock();
			}
			else{
				error(skt,type,5);
			}
		}
		else if(type == 10){
			int alive,join,monster,started,ready,total;
        		alive = 128 & character.flag;
        		join = 64 & character.flag;
        		monster = 32 & character.flag;
        		started = 16 & character.flag;
        		ready = 8 & character.flag;
			if(!started && !alive){
				Make_Character(&character,skt);
				int check = 0;
				for(it = Clients.begin(); it != Clients.end(); it++){
					if(it->second.name == character.name){
						check = 1;
					}
				}
				for(int i = 0; i < Dead_Players.size();i++){
					if(Dead_Players[i].name == character.name)
						check = 1;
				}
				/*for(int i = 0; i < Dead_Players.size(); i++){
					if(character.name == Dead_Players[i].name){
						check = 1;
					}
				}*/
				if(check == 0){
					total = character.attack + character.defense + character.regen;
					if(total > game.initial || total < 0){
                        		        error(skt,type,4);
					}
					else{
						mtx.lock();
                                       		cout << it->second.name << endl;
						character.flag = 200;
						Clients.insert(pair<int,Player>(skt,character));
						acceptm(skt,type);
						Write_Character(&character,skt,&Clients,&Monsters,&Dead_Players,&Rooms);
						mtx.unlock();
					}
				}
				else{
					error(skt,7,2);
				}
			}
			else{
				acceptm(skt,type);
				mtx.lock();
				Send_Room(&character,&Rooms,skt);
				mtx.unlock();
				mtx.lock();
                               	Write_Character(&character,skt,&Clients,&Monsters,&Dead_Players,&Rooms);
				mtx.unlock();
			}
		}
		else if(type == 12){
			mtx.lock();
			if(alive){
				character.health = 0;
                        	character.flag = 8;
				Dead(character,&Dead_Players,&Clients,skt);
			}
			else{
				
			}
			cout << "Person at " << skt << " has left" << endl;
			close(skt);
			mtx.unlock();
			pthread_exit(args);
		}

		if(connection_checker == 1){
			mtx.lock();
                	connection_checker = 0;
			cout << "I got to connection_checker" << endl;
			if(!character.name.empty() && alive){
				character.health = 0;
				character.flag = 8;
				Dead(character,&Dead_Players,&Clients,skt);
			}
			else{
				Clients.erase(skt);
			}
			close(skt);
			mtx.unlock();
			pthread_exit(args);
        	}
	}
	return 0;
}

void* handle_client(int client_fd){
	pthread_t new_thread;
        //struct client_args* cargs = (struct client_args*) malloc(sizeof(client_args));
        pthread_create(&new_thread,0,client_thread,(void*)(intptr_t)client_fd);
}

int main(int argc, char ** argv){
	Create_Rooms(&Rooms);
	Create_Monsters(&Monsters);
	/*map<string,Room>::iterator it;
	for(it = Rooms.begin(); it != Rooms.end(); it++){
		cout << it->second.name << endl;
		cout << it->second.description << endl;
		cout << it->second.number << endl;
	}*/
	struct sigaction pipe;
	pipe.sa_handler = connection_check;
        sigaction(SIGPIPE, &pipe, 0);
	struct sockaddr_in sad;
	if(argc > 1)
		sad.sin_port = htons(atoi(argv[1]));
	else {
		sad.sin_port = htons(5099);
		puts("Defaulting to port 5099");
	}
	sad.sin_addr.s_addr = INADDR_ANY;
	sad.sin_family = AF_INET;

	int skt = socket(AF_INET, SOCK_STREAM, 0);
	bind(skt, (struct sockaddr *)(&sad), sizeof(struct sockaddr_in));
	listen(skt, 5);
	int client_fd;
	for(;;){
		client_fd = accept(skt, 0, 0);
		handle_client(client_fd);
	}
	close(skt);
	return 0;
}
