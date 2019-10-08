#ifndef SERVER_H
#define SERVER_H
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/ip.h>
#include<stdio.h>
#include<unistd.h>
#include<vector>
#include<thread>
#include<cstring>
#include<string>
#include<map>
using namespace std;

struct client_args {
        int client_fd;
        struct sockaddr_in cad;
};

class Player{
	public:
		string name;
		unsigned char flag = 8;
		unsigned short attack;
		unsigned short defense;
		unsigned short regen;
		signed short health = 100;
		unsigned short gold = 100;
		unsigned room_num = 10;
		unsigned short length;
		string player_d;
};

class Room{
	public:
		unsigned short number = 0;
		string name;
		unsigned short length;
		string description;
};

struct message{
	unsigned short length;
	string recipient;
	string sender;
	string message;
};
struct game_description{
        unsigned short initial = 100;
        unsigned short limit = 65535;
        string description = "After spending days in a barrel floating in the sea, you feel a tug of the barrel. Seems some sap has fished you out of the sea and saved you from your plight. Beforehand you were cast into the sea by a storm and forced to take refuge in a barrel when your boat capsized. Maybe a dingy wasn't such a good idea to use eh. Anyways, your embarked on this journey in search of the best treasure a pirate could ever dream of...the One Piece. The One Piece is said to be the leftover treasure of the king of the pirates, Gol D. Rogers. Who knows what awaits you on your journey...";
	unsigned short length = description.size();
};


void Game_Packet(struct game_description *game, int skt);
void Make_Character(class Player *c, int skt);
void Write_Character(class Player *c, int skt,map<int,Player> *Clients,vector<Player> *Monsters,vector<Player> *Dead_Players,map<string,Room> *Rooms);
void acceptm(int skt,int accepted);
void error(int skt, int type, int code);
void message(int skt, map<int,Player> *Clients,struct message *m);
void Create_Rooms(map<string,Room> *Rooms);
void Send_Room(class Player *c, map<string,Room> *Rooms,int skt);
void Change_Room(class Player *c, map<string,Room> *Rooms,int skt,map<int,Player> *Clients);
void Connections(map<string,Room> *Room,int skt,int curr_room);
void Create_Monsters(vector<Player> *Monsters);
void fight(class Player *c, int skt, map<string,Room> *Rooms, map<int,Player> *Clients, vector<Player> *Monsters,vector<Player> *Dead_Players);
void loot(class Player *c, int skt, map<string,Room> *Rooms, map<int,Player> *Clients, vector<Player> *Monsters,vector<Player> *Dead_Players);
void Dead(class Player c, vector<Player> *Dead_Players,map<int,Player> *Clients,int skt);
void pvp(class Player *c, int skt, map<string,Room> *Rooms, map<int,Player> *Clients,vector<Player> *Dead_Players,vector<Player> *Monsters);
#endif
