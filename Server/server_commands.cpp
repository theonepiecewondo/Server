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
void Game_Packet(struct game_description *game, int skt){
	int type = 11;
	write(skt,&type,1);
       	write(skt,&game->initial,2);
       	write(skt,&game->limit,2);
      	write(skt,&game->length,2);
       	char buffer[game->length];
	memcpy(buffer,game->description.c_str(),sizeof(buffer));
  	write(skt,&buffer,game->length);
	

}
void Make_Character(class Player *c, int skt){
	char buffer[32];
	recv(skt, &buffer,32,MSG_WAITALL);
        recv(skt, &c->flag,1,MSG_WAITALL);
        recv(skt, &c->attack,2,MSG_WAITALL);
        recv(skt, &c->defense,2,MSG_WAITALL);
        recv(skt, &c->regen,2,MSG_WAITALL);
	recv(skt, &c->health,2,MSG_WAITALL);
	c->health = 100;
        recv(skt, &c->gold,2,MSG_WAITALL);
	c->gold = 100;
	recv(skt, &c->room_num,2,MSG_WAITALL);
	c->room_num = 10;
        recv(skt, &c->length,2,MSG_WAITALL);
	char buffer2[c->length];
        recv(skt, &buffer2,c->length,MSG_WAITALL);
	c->name = buffer;
	c->player_d = buffer2;
	memset(buffer,0,32);
	memset(buffer2,0,c->length+2);
	//memcp cy(&c->name,buffer,sizeof(c->name));
}

void Write_Character(class Player *c, int skt,map<int,Player> *Clients,vector<Player> *Monsters,vector<Player> *Dead_Players,map<string,Room> *Rooms){
	unsigned short type = 10;
	mutex mtx;
	map<int,Player>::iterator it,ir;
	map<string,Room>::iterator r;
	for(it = Clients->begin(); it != Clients->end(); it++){
		if(it->second.room_num == c->room_num && it->second.health > 0){
			if(it->second.name == c->name){
				it->second.flag = c->flag;
				it->second.health = c->health;
				it->second.gold = c->gold;
			}
			//cout << "Character: " << it->second.name << " is in room " << it->second.room_num << " at socket " << it->first << endl;
			char name[32];
			memcpy(name,it->second.name.c_str(),sizeof(name));
			char description[it->second.length];
			memcpy(description,it->second.player_d.c_str(),sizeof(description));
			for(ir = Clients->begin(); ir != Clients->end(); ir++){
				if(ir->second.room_num == c->room_num){
					mtx.lock();
					write(ir->first,&type,1);
                        		write(ir->first,&name,32);
                        		write(ir->first,&it->second.flag,1);
                        		write(ir->first,&it->second.attack,2);
                        		write(ir->first,&it->second.defense,2);
                        		write(ir->first,&it->second.regen,2);
                        		write(ir->first,&it->second.health,2);
                        		write(ir->first,&it->second.gold,2);
                        		write(ir->first,&it->second.room_num,2);
                        		write(ir->first,&it->second.length,2);
                        		write(ir->first,&description,it->second.length);
					mtx.unlock();
				}
			}
			memset(name,0,32);
			memset(description,0,it->second.length);
			/*write(skt,&type,1);
                        write(skt,&name,32);
                        write(skt,&it->second.flag,1);
                        write(skt,&it->second.attack,2);
                        write(skt,&it->second.defense,2);
                        write(skt,&it->second.regen,2);
                        write(skt,&it->second.health,2);
                        write(skt,&it->second.gold,2);
                        write(skt,&it->second.room_num,2);
                        write(skt,&it->second.length,2);
                        write(skt,&description,it->second.length);
			memset(name,0,32);
			memset(description,0,it->second.length);*/
		}
	}
	vector<Player>::iterator m;
	//map<int,Player>::iterator mc;

	for(it = Clients->begin(); it != Clients->end(); it++){
		for(m = Monsters->begin(); m != Monsters->end(); m++){
			if(m->room_num == c->room_num && it->second.room_num == m->room_num){
                        	char name[32];
                       		memcpy(name,m->name.c_str(),sizeof(name));
                        	write(it->first,&type,1);
                        	write(it->first,&name,32);
                        	write(it->first,&m->flag,1);
                        	write(it->first,&m->attack,2);
                        	write(it->first,&m->defense,2);
                        	write(it->first,&m->regen,2);
                        	write(it->first,&m->health,2);
                        	write(it->first,&m->gold,2);
                        	write(it->first,&m->room_num,2);
                        	write(it->first,&m->length,2);
                        	char description[m->length];
                        	memcpy(description,m->player_d.c_str(),sizeof(description));
                        	write(it->first,&description,m->length);
				memset(name,0,32);
                		memset(description,0,m->length);
                	}
		}
	}
	/*for(m = Monsters->begin(); m != Monsters->end(); m++){
                        if(m->room_num == c->room_num){
                                char name[32];
                                memcpy(name,m->name.c_str(),sizeof(name));
                                write(skt,&type,1);
                                write(skt,&name,32);
                                write(skt,&m->flag,1);
                                write(skt,&m->attack,2);
                                write(skt,&m->defense,2);
                                write(skt,&m->regen,2);
                                write(skt,&m->health,2);
                                write(skt,&m->gold,2);
                                write(skt,&m->room_num,2);
                                write(skt,&m->length,2);
                                char description[m->length];
                                memcpy(description,m->player_d.c_str(),sizeof(description));
                                write(skt,&description,m->length);
                                memset(name,0,32);
                                memset(description,0,m->length);
                        }
                }
	*/

	vector<Player>::iterator d;
	for(it = Clients->begin(); it != Clients->end(); it++){
        	for(d = Dead_Players->begin(); d != Dead_Players->end(); d++){
                	if(d->room_num == c->room_num && d->room_num == it->second.room_num){
                        	char name[32];
                        	memcpy(name,d->name.c_str(),sizeof(name));
                        	write(it->first,&type,1);
                        	write(it->first,&name,32);
                        	write(it->first,&d->flag,1);
                        	write(it->first,&d->attack,2);
                        	write(it->first,&d->defense,2);
                        	write(it->first,&d->regen,2);
                        	write(it->first,&d->health,2);
                        	write(it->first,&d->gold,2);
                        	write(it->first,&d->room_num,2);
                        	write(it->first,&d->length,2);
                        	char description[d->length];
                        	memcpy(description,d->player_d.c_str(),sizeof(description));
                        	write(it->first,&description,d->length);
                       		memset(name,0,32);
                       		memset(description,0,d->length);
                	}
		}

        }

}

void acceptm(int skt,int accepted){
	int type = 8;
        write(skt,&type,1);
        write(skt,&accepted,1);
	printf("Accept message sent\n");
}

void message(int skt, map<int,Player> *Clients,struct message *m){
	map<int,Player>::iterator it;
	mutex mtx;
	char recipient[32];
	char sender[32];
	memset(recipient,0,sizeof(recipient));
        memset(sender,0,sizeof(sender));
	recv(skt,&m->length,2,MSG_WAITALL);
	char message[m->length];
	recv(skt,recipient,32,MSG_WAITALL);
	recv(skt,sender,32,MSG_WAITALL);
	recv(skt,message,m->length,MSG_WAITALL);
	mtx.lock();
	m->recipient = recipient;
	for(it = Clients->begin(); it != Clients->end();it++){
		if(it->second.name == m->recipient){
			int type = 1;
			write(it->first,&type,1);
			write(it->first,&m->length,2);
			write(it->first,&recipient,32);
			write(it->first,&sender,32);
			write(it->first,&message,m->length);
			memset(recipient,0,sizeof(recipient));
			memset(sender,0,sizeof(sender));
			memset(message,0,sizeof(message));
	
		}
		else if (it == Clients->end()){
			error(skt,7,9);
			break;
			
		}
		else{
		}
	}
	mtx.unlock();
}

void error(int skt, int type, int code){
	type = 7;
	string message;
	switch(code){
		case 0:
			message = "Other";
			break;
		case 1: 
			message = "Bad Room";
			break;
		case 2:
			message = "Player Exists";
			break;
		case 3:
			message = "Bad Monster: Not existent";
			break;
		case 4:
			message = "Stat Error";
			break;
		case 5:
			message = "Not Ready, Start Game or Make a Character";
			break;
		case 6:
			message = "No Target";
			break;
		case 7:
			message = "Can't Fight";
			break;
		case 8:
			message = "No PVP Combat allowed";
			break;
		case 9:
			message = "Message Recipient not found";
	}
	int length = message.length();
	char buffer[length];
	memcpy(buffer,message.c_str(),sizeof(buffer));
	write(skt,&type,1);
	write(skt,&code,1);
	write(skt,&length,2);
	write(skt,&buffer,length);
	memset(buffer,0,length+2);
}
void Create_Rooms(map<string,Room> *Rooms){
	ifstream file ("rooms.csv");
	map<string,Room>::iterator it; 
	vector<string> words;
	string temp, temp2;
	while(file.good()){
		getline(file,temp);
		stringstream s(temp);
		while(getline(s,temp2,'#')){
			words.push_back(temp2);
			if(words.size() == 4){
				Room R;
				R.name = words[0];
				R.description = words[1];
				R.number = stoi(words[2]);
				R.length = words[1].length();
				/*cout << words[0] << endl;
				cout << words[1] << endl;
				cout << words[2] << endl;
				cout << words[3] << endl;*/
				Rooms->insert({words[3],R});
				words.clear();
			}
		}
	}

}

void Send_Room(class Player *c, map<string,Room> *Rooms,int skt){
	int type = 9;
	mutex mtx;
	mtx.lock();
	map<string,Room>::iterator it;
	for(it = Rooms->begin(); it != Rooms->end(); it++){
		if(c->room_num == it->second.number){
			char name[32];
			char description[it->second.length];
			memcpy(name,it->second.name.c_str(),sizeof(name));
			memcpy(description,it->second.description.c_str(),sizeof(description));
			write(skt,&type,1);
			write(skt,&c->room_num,2);
			write(skt,&name,32);
			write(skt,&it->second.length,2);
			write(skt,&description,it->second.length);
			memset(name,0,sizeof(name));
			memset(description,0,sizeof(description));
		}
		else if(it == Rooms->end()){
			error(skt,7,1);
		}
		else{
		}
	}
	mtx.unlock();
}
void Change_Room(class Player *c, map<string,Room> *Rooms,int skt,map<int,Player> *Clients){
 	int type = 2;
	mutex mtx;
	unsigned short room_num = 0;
	recv(skt,&room_num,2,MSG_WAITALL);
	map<string,Room>::iterator it;
	vector<int> cn;
	for(it = Rooms->begin(); it != Rooms->end(); it++){
                if(it->second.number == c->room_num){
                        stringstream temp(it->first);
                        while(1){
                                int cnt;
                                temp >> cnt;
                                if(!temp)
                                        break;
				cn.push_back(cnt);
			}
		}
	}
	map<int,Player>::iterator ir;
	for(int i = 0; i < cn.size(); i++){
		if(room_num == cn[i]){
			c->room_num = cn[i];
			for(ir = Clients->begin(); ir != Clients->end(); ir++){
				if(c->name == ir->second.name){
					if(c->health < 100){
						c->health = c->health + c->regen;
						if(c->health > 100)
							c->health = 100;
					}
					ir->second.room_num = c->room_num;
					Send_Room(c,Rooms,skt);
					Connections(Rooms,skt,c->room_num);
					cn.clear();
					break;
				}
			}
		}
		else if(i == cn.size()-1){
			error(skt,7,1);
			Send_Room(c,Rooms,skt);
                        Connections(Rooms,skt,c->room_num);
			cn.clear();
		}
		else{
		}
	}
}
void Connections(map<string,Room> *Rooms, int skt,int curr_room){
	int type = 13;
	map<string,Room>::iterator it;
	for(it = Rooms->begin(); it != Rooms->end(); it++){
		if(it->second.number == curr_room){
			stringstream temp(it->first);
			while(1){
				map<string,Room>::iterator ir;
				int cnt;
				temp >> cnt;
				if(!temp)
					break;

				for(ir = Rooms->begin(); ir != Rooms->end(); ir++){
					if(ir->second.number == cnt){
						char name[32];
		                        	char description[ir->second.length];
                		        	memcpy(name,ir->second.name.c_str(),sizeof(name));
                 		       		memcpy(description,ir->second.description.c_str(),sizeof(description));
                     				write(skt,&type,1);
                        			write(skt,&ir->second.number,2);
                        			write(skt,&name,32);
                        			write(skt,&ir->second.length,2);
                        			write(skt,&description,ir->second.length);
                        			memset(name,0,sizeof(name));
                        			memset(description,0,sizeof(description));

					}
					else{

					}
				}
			}
		}
	}
}
void Create_Monsters(vector<Player> *Monsters){
	ifstream file ("monsters.csv");
        vector<string> words;
        string temp, temp2;
        while(file.good()){
                getline(file,temp);
                stringstream s(temp);
                while(getline(s,temp2,'#')){
                        words.push_back(temp2);
                        if(words.size() == 9){
                                Player m;
				m.name = words[0];
				m.flag = stoi(words[1]);
				m.attack = stoi(words[2]);
				m.defense = stoi(words[3]);
				m.regen = stoi(words[4]);
				m.health = stoi(words[5]);
				m.gold = stoi(words[6]);
				m.room_num = stoi(words[7]);
				m.player_d = words[8];
				m.length = m.player_d.length();
				Monsters->push_back(m);
                                words.clear();
                        }
                }
        }

}
void fight(class Player *c, int skt, map<string,Room> *Rooms, map<int,Player> *Clients, vector<Player> *Monsters,vector<Player> *Dead_Players){
	mutex mtx;
	int type = 3;
	map<string,Room>::iterator r;
        vector<Player>::iterator m;
        map<int,Player>::iterator p;
        for(r = Rooms->begin(); r != Rooms->end(); r++){
		for(m = Monsters->begin(); m != Monsters->end(); m++){
                	if(r->second.number == c->room_num && c->room_num == m->room_num && m->flag == 184){
				int player_damage = c->defense - m->attack;
				int monster_damage = m->defense - c->attack;
				c->health = c->health + player_damage + c->regen;
				m->health = m->health + monster_damage + m->regen;
				if(c->health <= 0){
					c->health = 0;
					c->flag = 8;
					Dead(*c,Dead_Players,Clients,skt);
				}
		
				if(m->health <= 0){
					m->health = 0;
					m->flag = 56;
				}
				if(m->health > 100)
					m->health = 100;
                                if(c->health > 100)
                                        c->health = 100;
				mtx.lock();
				Write_Character(c,skt,Clients,Monsters,Dead_Players,Rooms);
				mtx.unlock();
			}
			else if(r->second.number == c->room_num && c->room_num == m->room_num && m->flag == 56){
				error(skt,7,7);
			}
			else{
			}
		}
        }

}
void loot(class Player *c, int skt, map<string,Room> *Rooms, map<int,Player> *Clients, vector<Player> *Monsters,vector<Player> *Dead_Players){
	int type = 5;
	mutex mtx;
	char buffer[32];
	string looter_name;
        map<string,Room>::iterator r;
        vector<Player>::iterator m;
        map<int,Player>::iterator p;
	vector<Player>::iterator d;
	recv(skt,&buffer,32,MSG_WAITALL);
	looter_name = buffer;
	int i = 0;
	for(r = Rooms->begin(); r != Rooms->end(); r++){
		for(m = Monsters->begin(); m != Monsters->end(); m++){
			if(r->second.number == c->room_num && c->room_num == m->room_num && looter_name == m->name && m->flag == 56){
				c->gold = c->gold + m->gold;
				m->gold = 0;
				mtx.lock();
				Write_Character(c,skt,Clients,Monsters,Dead_Players,Rooms);
				mtx.unlock();
				i = 0;
			}
			else if(i == Monsters->size()){
				cout << "Monster size " << Monsters->size() << endl;
				error(skt,7,3);
			}
			else{
	
			}
			i++;
		}
		i = 0;
		for(d = Dead_Players->begin(); d != Dead_Players->end(); d++){
			if(r->second.number == c->room_num && d->name == looter_name){
				c->gold = c->gold + d->gold;
				d->gold = 0;
				mtx.lock();
				Write_Character(c,skt,Clients,Monsters,Dead_Players,Rooms);
				mtx.unlock();
				i = 0;
			}
			else if(i == Dead_Players->size()){
				error(skt,7,6);
			}
			else{
			}
			i++;
		}
		i = 0;
	}
}

void Dead(class Player c, vector<Player> *Dead_Players, map<int,Player> *Clients,int skt){
	mutex mtx;
	mtx.lock();
	c.health = 0;
	c.flag = 8;
	cout << "character name " << c.name << " died" << endl;
	Dead_Players->push_back(c);
	char name[32];
        memcpy(name,c.name.c_str(),sizeof(name));
        char description[c.length];
        memcpy(description,c.player_d.c_str(),sizeof(description));
        int temp = 10;
        write(skt,&temp,1);
        write(skt,&name,32);
        write(skt,&c.flag,1);
        write(skt,&c.attack,2);
        write(skt,&c.defense,2);
        write(skt,&c.regen,2);
        write(skt,&c.health,2);
        write(skt,&c.gold,2);
        write(skt,&c.room_num,2);
        write(skt,&c.length,2);
        write(skt,&description,c.length);
        memset(name,0,32);
        memset(description,0,c.length);
	Clients->erase(skt);
	mtx.unlock();
}
void pvp(class Player *c, int skt, map<string,Room> *Rooms, map<int,Player> *Clients,vector<Player> *Dead_Players,vector<Player> *Monsters){
	mutex mtx;
	int type = 4;
	char buffer[32];
	string pvp_name;
	map<string,Room>::iterator r;
        map<int,Player>::iterator p;
	recv(skt,&buffer,32,MSG_WAITALL);
	pvp_name = buffer;
        for(r = Rooms->begin(); r != Rooms->end(); r++){
		for(p = Clients->begin(); p != Clients->end(); p++){
                	if(r->second.number == c->room_num && c->room_num == p->second.room_num && p->second.name == pvp_name){
				int player_damage = c->defense - p->second.attack;
				int pvp_damage = p->second.defense - c->attack;
				c->health = c->health + player_damage + c->regen;
				p->second.health = p->second.health + pvp_damage + p->second.regen;
				if(c->health <= 0){
					c->health = 0;
					c->flag = 8;
					Dead(*c,Dead_Players,Clients,skt);
				}
				if(p->second.health <= 0){
					p->second.health = 0;
					p->second.flag = 8;
					mtx.lock();
					char name[32];
                       			memcpy(name,p->second.name.c_str(),sizeof(name));
                		        char description[p->second.length];
		                        memcpy(description,p->second.player_d.c_str(),sizeof(description));
					int temp = 10;
					Dead_Players->push_back(p->second);
					write(p->first,&temp,1);
		                        write(p->first,&name,32);
               			        write(p->first,&p->second.flag,1);
                 		       	write(p->first,&p->second.attack,2);
                  		      	write(p->first,&p->second.defense,2);
                     		  	write(p->first,&p->second.regen,2);
                      	  		write(p->first,&p->second.health,2);
                        		write(p->first,&p->second.gold,2);
                        		write(p->first,&p->second.room_num,2);
                        		write(p->first,&p->second.length,2);
                        		write(p->first,&description,p->second.length);
                        		memset(name,0,32);
                        		memset(description,0,p->second.length);
					Clients->erase(p->first);
					mtx.unlock();
					close(p->first);
					//Dead(*c,Dead_Players,Clients,skt);
				}
				if(p->second.health > 100)
					p->second.health = 100;
                                if(c->health > 100)
                                        c->health = 100;
				mtx.lock();
				Write_Character(c,skt,Clients,Monsters,Dead_Players,Rooms);
				mtx.unlock();
			}
			else if(r->second.number == c->room_num && c->room_num == p->second.room_num && p->second.flag == 8){
				error(skt,7,7);
			}
			else{
			}
		}
        }
}
