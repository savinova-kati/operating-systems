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
            x[1 - v] -= l + 1, p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]]='/', x [v]+=2, p[x[0]][x[1]]='/', x[v]--, x[1 - v]++;
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
	socket.setsockopt(ZMQ_SNDTIMEO, (int)milliseconds); // устанавливает флаги и время ожидания от сервера
	//Это ассоциативный контейнер, который работает по принципу — [ключ — значение]
	map<int, pair<unsigned, unsigned> >statistics; 
	map<int, pair<unsigned, unsigned> > amount;
	map<int, pair<vector<vector<char> >, vector<vector<char> > > > fields;
	map<int, vector<pair<unsigned, unsigned> > > possible_turns;
	map<int, pair<unsigned, unsigned> > last_commands;
	map<int, bool> finishing;
	map<int, vector<pair<unsigned, unsigned> > > variants; // после удачного попадания сервер пойдет на соседние клетки
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
			if (statistics.find(ID) != statistics.end()) // чтобы не совпадали ID у пользователей
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
		else if (command == "Get") // вывод поля сервера
		{
			print(fields[ID].first);
			send_message("OK", socket);	
		}
		else if (command == "Exit") // все чистим
		{
			if (statistics.find(ID) != statistics.end()) 
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
		else if (command.substr(0, 3) == "Try") // substr возвращает с 0 символа длинной 3
		{
			string coordinates = message.substr(0, message.find(" ")); // считывается что-то в духе Try21 2522
			cout << coordinates << "\n";
			unsigned horizontal = unsigned(coordinates[3]) - unsigned('0') + 1; // переход из букв в цифры
		    unsigned vertical = unsigned(coordinates[4]) - unsigned('0') + 1;
			cout << horizontal << " " << vertical << "\n";
			pair<vector<vector<char> >, vector<vector<char> > > squares = fields[ID];
			if (squares.first[vertical][horizontal] == 'X') // если попали в сервекский корабль меняем X на К
			{
				squares.first[vertical][horizontal] = 'K';
				
				reply = "Killed";
				int v = vertical, h = horizontal; // начинаем проверку по вертикали и горизонтали убили мы корабль или подбили
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
					amount[ID] = make_pair(++amount[ID].first, amount[ID].second); // увеличиваем число кораблей убитых сервером
					if (amount[ID].first == 10)
					{	
						reply = "Won";
						statistics[ID] = make_pair(++statistics[ID].first, statistics[ID].second);
					}
				}
			}
			else if ((squares.first[vertical][horizontal] == 'K') || (squares.first[vertical][horizontal] == 'w'))
			{
				reply = "Another"; // уже стреляли сюда
			}
			else if (squares.first[vertical][horizontal] == '.')
			{
				reply = "Missed";
				squares.first[vertical][horizontal] = 'w';
			}
			fields[ID] = make_pair(squares.first, squares.second);
			send_message(reply, socket); 
		}
		else if (command == "Amount") // количество убитых кораблей у сервера
		{
			cout << "Amount: " << amount[ID].first << "\n";
			send_message("OK", socket);
		}
		else if (command == "Turns") // оставшиеся шаги
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
			if (command == "Killed") // попадание сервера и убийство корабля
			{
				pair<vector<vector<char> >, vector<vector<char> > > squares = fields[ID];
				vector<pair<unsigned, unsigned> > turns = possible_turns[ID];
				unsigned vertical = last_commands[ID].first, horizontal = last_commands[ID].second; // последняя введенная команда по горизонтали и вертикали
				squares.second[vertical][horizontal] = 'K'; // ставим в поле пользователя
				
						if ((vertical >= 0) && (vertical < 10) && (horizontal >= 0) && (horizontal  < 10)) // удаление из turns с помощью итераторов
						{
							vector<pair<unsigned, unsigned> >::iterator it;
							if ((it = find(turns.begin(), turns.end(), make_pair(vertical, horizontal))) != turns.end())
							{
								turns.erase(turns.begin() + distance(turns.begin(), it));
								//Вышеупомянутая функция erase () используется для удаления нескольких элементов из вектора на основе позиции, указанной в первом и втором аргументах этой функции.
							}
							if (squares.second[vertical][horizontal] == '.')
							{
								squares.second[vertical][horizontal] = 'w';
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
				amount[ID] = make_pair(amount[ID].first, ++amount[ID].second); // +1 убитый корабль сервером
				if (amount[ID].second == 10)
				{
					reply = "Lost";
					amount[ID] = make_pair(0, 0);
				}
			}
			if (!finishing[ID]) // компуктер рандомно стреляет если у пользователчя предыдущий ход был неудачным 
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
			else //если ход был удачный
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
				while ((length > 0) && (!((last_commands[ID].first + turn.first >= 0) && (last_commands[ID].first + turn.first < 10) && (last_commands[ID].second + turn.second >= 0) 
					&& (last_commands[ID].second + turn.second < 10))));


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
								if ((vertical >= 0) && (vertical < 10) && (horizontal >= 0) && (horizontal  < 10))
								{
									squares.second[vertical][horizontal] = 'K';
									vector<pair<unsigned, unsigned> >::iterator it;
                                        if ((it = find(turns.begin(), turns.end(), make_pair(vertical, horizontal))) != turns.end())
                                            {
                                                turns.erase(turns.begin() + distance(turns.begin(), it));
                                            }
									if (squares.second[vertical][horizontal] == '.')
									{
										squares.second[vertical][horizontal] = 'w';
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
		else if (command == "Missed") // сервер промахнулся
		{
			pair<vector<vector<char> >, vector<vector<char> > > squares = fields[ID];
			cout << "1" << endl;
            squares.second[last_commands[ID].first][last_commands[ID].second] = 'w';
            cout << "2" << endl;
            fields[ID] = make_pair(squares.first, squares.second);
            cout << "3" << endl;
			send_message("Do", socket);
		}
		else if (command == "Wounded") // сервер после подбития корабля
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
			while ((length > 0) && (!((last_commands[ID].first + turn.first >= 0) && (last_commands[ID].first + turn.first < 10) && (last_commands[ID].second + turn.second >= 0)
				&& (last_commands[ID].second + turn.second < 10))));
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

							if ((vertical >= 0) && (vertical < 10) && (horizontal >= 0) && (horizontal  < 10))
							{
								squares.second[vertical][horizontal] = 'K';
								vector<pair<unsigned, unsigned> >::iterator it;
								if ((it = find(turns.begin(), turns.end(), make_pair(vertical, horizontal))) != turns.end())
								{
									turns.erase(turns.begin() + distance(turns.begin(), it));
								}
								if (squares.second[vertical][horizontal] == '.')
								{
									squares.second[vertical][horizontal] = 'w';
									vector<pair<unsigned, unsigned> >::iterator it;
								}
							}
				}
				else if (reply == "Missed")
				{
					squares.second[vertical][horizontal] = 'w';
				}
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
				else // ход сервера после убийства коробля пользователя в случае если игра еще не закончилась
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


// command - то что пришло