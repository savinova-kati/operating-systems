#include "zmq.hpp"
#include "zmq.h"
#include <iostream>
#include <unistd.h>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

void random(vector<vector<char> > & p)
{
    int j=-1, k, v, l, x[2], y;
    srand(time(0));
    for(l=4; l>0; l--)
        for(k=5;k-l;k--)
        {
            v = 1&rand();
            //v = rand() % 2;
            do for (x[v] = 1 + rand() % 10, x[1 - v] = 1 + rand() % 7, y = j = 0; j - l; y |= p[x[0]][x[1]] != '.', x[1 - v]++, j++); while(y);
            x[1 - v] -= l + 1, p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]]='/', x [v]+=2, p[x [0]][x[1]]='/', x[v]--, x[1 - v]++;
            for (j = -1; ++j - l; p[x[0]][x[1]] = 'X', x[v]--, p[x[0]][x[1]] = '/', x[v]+=2, p[x[0]][x[1]]='/', x[v]--, x[1 - v]++);
            p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]] = '/', x[v]+=2, p[x[0]][x[1]] = '/';
        }
        for (int i = 0; i < 12; ++i)
        {
                replace(p[i].begin(), p[i].end(), '/', '.');
        }
}

void send_message(string message_string, zmq::socket_t& socket)
{
    zmq::message_t message_back(message_string.size());
    memcpy(message_back.data(), message_string.c_str(), message_string.size());
    if(!socket.send(message_back))
    {
        cout << "Не удается отправить сообщение. " << getpid() << "\n";
    }
}

void print(vector<vector<char> >& p)
{
        for (int i = 1; i < 11; ++i)
        {
                for (int j = 1; j < 11; ++j)
                {
                        cout << p[i][j];
                }
                cout << "\n";
        }
}

int main()
{
	zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP); // устанавливает флаги на сокет
	string port, reply;
	cout << "Введите номер игры\n";
	cin >> port;
	socket.bind("tcp://*:" + port); //Связывает объект Socket с локальной конечной точкой.
	//TCP/IP — сетевая модель передачи данных, представленных в цифровом виде. Модель описывает способ передачи данных от источника информации к получателю.
	unsigned milliseconds;
	cout << "Введите время, в течение которого сокет должен ждать ответа от клиента и отправлять сообщение клиенту \n";
	cin >> milliseconds;
	socket.setsockopt(ZMQ_SNDTIMEO, (int)milliseconds);
	//Это ассоциативный контейнер, который работает по принципу — [ключ — значение]
	map<int, pair<unsigned, unsigned> >statistics; 
	map<int, pair<unsigned, unsigned> > amount;
	map<int, pair<vector<vector<char> >, vector<vector<char> > > > fields;
	map<int, vector<pair<unsigned, unsigned> > > possible_turns;
	map<int, pair<unsigned, unsigned> > last_commands;
	map<int, bool> finishing;
	map<int, vector<pair<unsigned, unsigned> > > variants;
	while (true) 
	{
		zmq::message_t request;
		socket.recv(&request); // Функция служит для чтения данных из сокета. 
		string message(static_cast<char*>(request.data()), request.size()), reply;
		string command = message.substr(0, message.find(" "));
		int ID = stoi(message.substr(message.find(" ") + 1));
		cout << message << "\n";
		if (command == "ID")
		{
			statistics[ID] = make_pair(0, 0);
			amount[ID] = make_pair(0, 0);
			send_message("OK", socket);
		}
		else if (command == "Statistics")
		{
			if (statistics.find(ID) != statistics.end())
			{
				pair<unsigned, unsigned> numbers = statistics[ID];
				reply = to_string(numbers.first) + " " + to_string(numbers.second);
			}
			else
			{
				reply = "Игрок с таким ID уже сужествует";
			}
			send_message(reply, socket);
		}
		else if (command == "Get")
		{
			print(fields[ID].first);
			send_message("OK", socket);	
		}
		else if (command == "Exit")
		{
			if (statistics.find(ID) != statistics.end()) // ?????
			{
				statistics.erase(ID);
				amount.erase(ID);
				fields.erase(ID);
				possible_turns.erase(ID);
				last_commands.erase(ID);
				finishing.erase(ID);
				variants.erase(ID);
			}
			send_message("Спасибо за игру!", socket);
		}	
		else if (command == "Begin")
		{

			amount[ID] = make_pair(0, 0);
			vector<vector<char> > server_field (12, vector<char>(12, '.'));
			vector<vector<char> > player_field (12, vector<char>(12, '.'));
			random(server_field);
			fields[ID] = make_pair(server_field, player_field);

			server_field.clear();
			player_field.clear();
	
			vector<pair<unsigned, unsigned> > turns (100);
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; ++j)
				{
					turns[i * 10 + j] = make_pair(i, j);
				}
			}
			//cout << "Created turns\n";
			possible_turns[ID] = turns;
			finishing[ID] = false;
			last_commands[ID] = make_pair(-1, -1);
			variants[ID].clear();
			variants[ID].push_back(make_pair(1, 0));
			variants[ID].push_back(make_pair(-1, 0));
			variants[ID].push_back(make_pair(0, 1));
			variants[ID].push_back(make_pair(0, -1));
			turns.clear();
			//reply = "Start";
			send_message("Start", socket);
		}
		else if (command.substr(0, 3) == "Try")
		{
			string coordinates = message.substr(0, message.find(" "));
			cout << coordinates << "\n";
			unsigned horizontal = unsigned(coordinates[3]) - unsigned('0') + 1;
		        unsigned vertical = unsigned(coordinates[4]) - unsigned('0') + 1;
			cout << horizontal << " " << vertical << "\n";
			pair<vector<vector<char> >, vector<vector<char> > > squares = fields[ID];
			if (squares.first[vertical][horizontal] == 'X')
			{
				squares.first[vertical][horizontal] = 'K';
				//vector<pair<unsigned, unsigned>> turns = possible_turns[ID];
				for (int i = -1; i < 2; ++i)
				{
					for (int j = -1; j < 2; ++j)
					{
						if ((vertical + i > 0) && (vertical + i < 11) && (horizontal + j > 0) && (horizontal + j < 11))
						{
							if (squares.first[vertical + i][horizontal + j] == '.')
							{
								squares.first[vertical + i][horizontal + j] = 'w';
								/*vector<pair<int, int>>::iterator it;
								if ((it = find(turns.begin(), turns.end(), make_pair(vertical + i, horizontal + j))) != turns.end())
								{
								        arr.erase(arr.begin() + distance(turns.begin(), it);
								}*/

							}
						}
					}
				}
				//possible_turns[ID] = turns;
				reply = "Killed";
				int v = vertical, h = horizontal;
				while ((v > 1) && (squares.first[v][h] == 'K'))
				{
					--v;
				}
				if (squares.first[v][h] == 'X')
				{
					reply = "Wounded";
				}
				if (reply == "Killed")
				{
					v = vertical; h = horizontal;
					while ((v < 10) && (squares.first[v][h] == 'K'))
					{
						++v;
					}
					if (squares.first[v][h] == 'X')
					{
						reply = "Wounded";
					}
					if (reply == "Killed")
					{
						v = vertical; h = horizontal;
                                        	while ((h > 1) && (squares.first[v][h] == 'K'))
                                        	{
                                                	--h;
                                        	}
						if (squares.first[v][h] == 'X')
                                        	{
                                                	reply = "Wounded";
                                        	}
						if (reply == "Killed")
						{
							v = vertical; h = horizontal;
							while ((h < 10) && (squares.first[v][h] == 'K'))
							{
								++h;
							}
							if (squares.first[v][h] == 'X')
							{
								reply = "Wounded";
							}
						}
					}
				}
				if (reply == "Killed")					
				{
					amount[ID] = make_pair(++amount[ID].first, amount[ID].second);
					if (amount[ID].first == 10)
					{	
						reply = "Won";
						statistics[ID] = make_pair(++statistics[ID].first, statistics[ID].second);
					}
				}
			}
			else if ((squares.first[vertical][horizontal] == 'K') || (squares.first[vertical][horizontal] == 'w'))
			{
				reply = "Another";
			}
			else if (squares.first[vertical][horizontal] == '.')
			{
				reply = "Missed";
				squares.first[vertical][horizontal] = 'w';
			}
			fields[ID] = make_pair(squares.first, squares.second);
			send_message(reply, socket); 
		}
		else if (command == "Amount")
		{
			cout << "Amount: " << amount[ID].first << "\n";
			send_message("OK", socket);
		}
		else if (command == "Turns")
		{
			vector<pair<unsigned, unsigned> > turns = possible_turns[ID];
			for (int i = 0; i < turns.size(); i++)
			{
				cout << turns[i].first << " " << turns[i].second << "\n";
			}
			cout << "Length is " << turns.size() << "\n";
			send_message("Ok", socket);
		}
		else if ((command == "Do") || (command == "Killed"))
		{
			if (command == "Killed")
			{
				pair<vector<vector<char> >, vector<vector<char> > > squares = fields[ID];
				vector<pair<unsigned, unsigned> > turns = possible_turns[ID];
				unsigned vertical = last_commands[ID].first, horizontal = last_commands[ID].second;
				squares.second[vertical][horizontal] = 'K';
				//turns.erase(turns.begin() + distance(turns.begin(), find(turns.begin(), turns.end(), make_pair(vertical, horizontal))));
				for (int i = -1; i < 2; ++i)
				{
					for (int j = -1; j < 2; ++j)
					{
						if ((vertical + i > 0) && (vertical + i < 11) && (horizontal + j > 0) && (horizontal + j < 11))
						{
							vector<pair<unsigned, unsigned> >::iterator it;
							if ((it = find(turns.begin(), turns.end(), make_pair(vertical + i, horizontal + j))) != turns.end())
							{
								turns.erase(turns.begin() + distance(turns.begin(), it));
							}
							if (squares.second[vertical + i][horizontal + j] == '.')
							{
								squares.second[vertical + i][horizontal + j] = 'w';
								/*vector<pair<unsigned, unsigned>>::iterator it;
								if ((it = find(turns.begin(), turns.end(), make_pair(vertical + i, horizontal + j))) != turns.end())
								{
									turns.erase(turns.begin() + distance(turns.begin(), it));
								}*/
							}
						}
					}
				}
				fields[ID] = make_pair(squares.first, squares.second);
				possible_turns[ID] = turns;
				finishing[ID] = false;
				variants[ID].clear();
				variants[ID].push_back(make_pair(1, 0));
				variants[ID].push_back(make_pair(-1, 0));
				variants[ID].push_back(make_pair(0, 1));
				variants[ID].push_back(make_pair(0, -1));
				amount[ID] = make_pair(amount[ID].first, ++amount[ID].second);
				if (amount[ID].second == 10)
				{
					reply = "Lost";
					amount[ID] = make_pair(0, 0);
				}
			}
			if (!finishing[ID])
			{
				int length = possible_turns[ID].size();
				srand(time(0));
				int number = rand() % length;
				vector<pair<unsigned, unsigned> > coordinates = possible_turns[ID];
				pair<unsigned, unsigned> turn = coordinates[number];
				cout << "Turn is " << turn.first << " " << turn.second << "\n";
				coordinates.erase(coordinates.begin() + number);
				possible_turns[ID] = coordinates;
				last_commands[ID] = turn;
				reply = "Try" + to_string(turn.first) + to_string(turn.second);
				cout << "Tried coordinates " << turn.first << " " << turn.second << "\n";
			}
			else
			{
				int length, number, k = 1;
				vector<pair<unsigned, unsigned> > positions;
                        	pair<unsigned, unsigned> turn;
				do
				{
					length = variants[ID].size();
					number = rand() % length;
				        positions = variants[ID];
					turn = positions[number];
               				positions.erase(positions.begin() + number);
				}
				while ((length > 0) && (!((last_commands[ID].first + turn.first > 0) && (last_commands[ID].first + turn.first < 11) && (last_commands[ID].second + turn.second > 0) 
					&& (last_commands[ID].second + turn.second < 11))));
				pair<vector<vector<char> >, vector<vector<char> > > squares = fields[ID];
				vector<pair<unsigned, unsigned> > turns = possible_turns[ID];
				unsigned vertical, horizontal;
				do
				{
					send_message("Try" + to_string(last_commands[ID].first + turn.first * k) + to_string(last_commands[ID].second + turn.second * k), socket);
					++k;
					zmq::message_t answer;
					socket.recv(&answer);
					string string(static_cast<char*>(answer.data()), answer.size());
					reply = string.substr(0, string.find(" "));		
					vertical = last_commands[ID].first + turn.first * k, horizontal = last_commands[ID].second + turn.second * k;
					turns.erase(turns.begin() + distance(turns.begin(), find(turns.begin(), turns.end(), make_pair(vertical, horizontal))));
					if ((reply == "Wounded") || (reply == "Killed"))
					{
						squares.second[vertical][horizontal] = 'K';
						for (int i = -1; i < 2; ++i)
						{
							for (int j = -1; j < 2; ++j)
							{
								if ((vertical + i > 0) && (vertical + i < 11) && (horizontal + j > 0) && (horizontal + j < 11))
								{
									vector<pair<unsigned, unsigned> >::iterator it;
                                                                        if ((it = find(turns.begin(), turns.end(), make_pair(vertical + i, horizontal + j))) != turns.end())
                                                                        {
                                                                                turns.erase(turns.begin() + distance(turns.begin(), it));
                                                                        }
									if (squares.second[vertical + i][horizontal + j] == '.')
									{
										squares.second[vertical + i][horizontal + j] = 'w';
									}
								}
							}
						}
					}
					else if (reply == "Missed")
					{
						squares.second[vertical][horizontal] = 'w';
					}
					//fields[ID] = {squares.first, squares.second};
				}
				while (reply == "Wounded");	
				fields[ID] = make_pair(squares.first, squares.second);
				possible_turns[ID] = turns;
				variants[ID] = positions;
				if (reply == "Missed")
				{
					reply = "Do";
				}
				else if (reply == "Killed")
				{
					finishing[ID] = false;
					variants[ID].clear();
					variants[ID].push_back(make_pair(1, 0));
					variants[ID].push_back(make_pair(-1, 0));
					variants[ID].push_back(make_pair(0, 1));
					variants[ID].push_back(make_pair(0, -1));
					amount[ID] = make_pair(amount[ID].first, ++amount[ID].second);
					if (amount[ID].second == 10)
					{
						reply = "Lost";
						amount[ID] = make_pair(0, 0);
					}
					else
					{
						int length = possible_turns[ID].size();
						srand(time(0));
						int number = rand() % length;
						vector<pair<unsigned, unsigned> > coordinates = possible_turns[ID];
						pair<unsigned, unsigned> turn = coordinates[number];
						coordinates.erase(coordinates.begin() + number);
						possible_turns[ID] = coordinates;
						last_commands[ID] = turn;
						reply = "Try" + to_string(turn.first) + to_string(turn.second);
						cout << "Tried coordinates " << turn.first << " " << turn.second << "\n";
					}
				}
			}
			send_message(reply, socket);	
		}
		else if (command == "Missed")
		{
			pair<vector<vector<char> >, vector<vector<char> > > squares = fields[ID];
                        squares.second[last_commands[ID].first][last_commands[ID].second] = 'w';
                        fields[ID] = make_pair(squares.first, squares.second);
			send_message("Do", socket);
		}
		else if (command == "Wounded")
		{
			finishing[ID] = true;
			int length, number, k = 1;
			vector<pair<unsigned, unsigned> > positions;
			pair<unsigned, unsigned> turn;
			do
			{
				length = variants[ID].size();
				number = rand() % length;
				positions = variants[ID];
				turn = positions[number];
				positions.erase(positions.begin() + number);
			}
			while ((length > 0) && (!((last_commands[ID].first + turn.first > 0) && (last_commands[ID].first + turn.first < 11) && (last_commands[ID].second + turn.second > 0)
				&& (last_commands[ID].second + turn.second < 11))));
			pair<vector<vector<char> >, vector<vector<char> > > squares = fields[ID];
			vector<pair<unsigned, unsigned> > turns = possible_turns[ID];
                        unsigned vertical, horizontal;
			do
			{
				send_message("Try" + to_string(last_commands[ID].first + turn.first * k) + to_string(last_commands[ID].second + turn.second * k), socket);
				++k;
				zmq::message_t answer;
				socket.recv(&answer);
				string string(static_cast<char*>(answer.data()), answer.size());
				reply = string.substr(0, string.find(" "));
				vertical = last_commands[ID].first + turn.first * k, horizontal = last_commands[ID].second + turn.second * k;
				turns.erase(turns.begin() + distance(turns.begin(), find(turns.begin(), turns.end(), make_pair(vertical, horizontal))));
				if ((reply == "Wounded") || (reply == "Killed"))
				{
					squares.second[vertical][horizontal] = 'K';
					for (int i = -1; i < 2; ++i)
					{
						for (int j = -1; j < 2; ++j)
						{
							if ((vertical + i > 0) && (vertical + i < 11) && (horizontal + j > 0) && (horizontal + j < 11))
							{
								vector<pair<unsigned, unsigned> >::iterator it;
								if ((it = find(turns.begin(), turns.end(), make_pair(vertical + i, horizontal + j))) != turns.end())
								{
									turns.erase(turns.begin() + distance(turns.begin(), it));
								}
								if (squares.second[vertical + i][horizontal + j] == '.')
								{
									squares.second[vertical + i][horizontal + j] = 'w';
									vector<pair<unsigned, unsigned> >::iterator it;
									/*if ((it = find(turns.begin(), turns.end(), make_pair(vertical + i, horizontal + j))) != turns.end())
									{
										turns.erase(turns.begin() + distance(turns.begin(), it));
									}*/
								}
							}
						}
					}
				}
				else if (reply == "Missed")
				{
					squares.second[vertical][horizontal] = 'w';
				}
				//fields[ID] = {squares.first, squares.second};
			}
			while (reply == "Wounded");
			fields[ID] = make_pair(squares.first, squares.second);
			variants[ID] = positions;
			if (reply == "Missed")
			{
				reply = "Do";
			}
			else if (reply == "Killed")
			{
				finishing[ID] = false;
				variants[ID].clear();
				variants[ID].push_back(make_pair(1, 0));
				variants[ID].push_back(make_pair(-1, 0));
				variants[ID].push_back(make_pair(0, 1));
				variants[ID].push_back(make_pair(0, -1));
				amount[ID] = make_pair(amount[ID].first, ++amount[ID].second);
				if (amount[ID].second == 10)
				{
					reply = "Lost";
					amount[ID] = make_pair(0, 0);
					statistics[ID] = make_pair(statistics[ID].first, ++statistics[ID].second);
				}
				else
				{
					int length = possible_turns[ID].size();
					srand(time(0));
					int number = rand() % length;
					vector<pair<unsigned, unsigned> > coordinates = possible_turns[ID];
					pair<unsigned, unsigned> turn = coordinates[number];
					coordinates.erase(coordinates.begin() + number);
					possible_turns[ID] = coordinates;
					last_commands[ID] = turn;
					reply = "Try" + to_string(turn.first) + to_string(turn.second);
					cout << "Tried coordinates " << turn.first << " " << turn.second << "\n";
				}
			}
			send_message(reply, socket);
		}

	}
	return 0;

}