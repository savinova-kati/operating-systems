#include <zmq.hpp>
#include <zmq.h> // библиотека для создания многопоточных приложений
#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>

using namespace std;

void send_message(string message_string, zmq::socket_t& socket) // отправка сообщений на сервер
{
    zmq::message_t message_back(message_string.size());
    memcpy(message_back.data(), message_string.c_str(), message_string.size()); // копирующая содержимое одной области памяти в другую.
    if(!socket.send(message_back))
    {
        cout << "Не удается отправить сообщение. " << getpid() << "\n";
    }
}

string recieve_message(zmq::socket_t& socket) // получение сообщений с сервера
{
	zmq::message_t reply;
	if (!socket.recv(&reply)) // чтение данных из сокета
	{
		cout << "Не удается получить сообщение от сервера\n";
		exit(1);
	}
	string message(static_cast<char*>(reply.data()), reply.size());
	return message;
}

void random(vector<vector<char> >& p)
{
    int j=-1, k, v, l, x[2], y;
    srand(time(0)); // srand() используется для установки начала последовательности, генерируемой функцией rand() 
    for(l=4; l>0; l--)
        for(k=5;k-l;k--)
        {
            v = 1&rand();
            do for (x[v] = 1 + rand() % 10, x[1 - v] = 1 + rand() % 7, y = j = 0; j - l; y |= p[x[0]][x[1]] != '.', x[1 - v]++, j++); while(y);
            x[1 - v] -= l + 1, p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]] = '/', x [v] += 2, p[x[0]][x[1]] = '/', x[v]--, x[1 - v]++;
            for (j = -1; ++j - l; p[x[0]][x[1]] = 'X', x[v]--, p[x[0]][x[1]] = '/', x[v] += 2, p[x[0]][x[1]] = '/', x[v]--, x[1 - v]++);
            p[x[0]][x[1]] = '/', x[v]--, p[x[0]][x[1]] = '/', x[v]+=2, p[x[0]][x[1]] = '/';
        }
        for (int i = 0; i < 12; ++i)
        {
                replace(p[i].begin(), p[i].end(), '/', '.');
        }
}

void flood(vector<vector<char> >& p)
{
	for (int i = 0; i < 12; i++)
	{
		p[i].clear();
		p[i] = vector<char>(12, '.');
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
	zmq::context_t context (1); // Класс context_t инкапсулирует функции, связанные с инициализацией и завершением контекста
	zmq::socket_t socket (context, ZMQ_REQ); // обмен данными между клиентом и сервером
	//Клиент использует ZMQ_REQ для отправки сообщений и получения ответов от сервера.
	string port;
	cout << "Введите номер игры, к которой хотите присоединиться\n";
	cin >> port;
        cout << "Подключение к серверу…" << endl;
	unsigned milliseconds;
        cout << "Введите время, в течение которого сокет может ожидать ответа от сервера.\n"; //Со́кет — название программного интерфейса для обеспечения обмена данными между процессами.
        cin >> milliseconds;
        socket.setsockopt(ZMQ_SNDTIMEO, (int)milliseconds); //Устанавливает тайм-аут для операции отправки на сокете.
	socket.setsockopt(ZMQ_RCVTIMEO, (int)milliseconds); //Устанавливает тайм-аут для операции получения на сокете.
        socket.connect ("tcp://localhost:" + port);
	send_message("ID " + to_string(getpid()), socket); // getpid() возвращяет идентификатор процесса
	zmq::message_t reply;

	recieve_message(socket); // Ждем чтобы сервер начлал игру
	cout << "Если вы готовы начинать игру, введите 'Begin' " << endl;
	vector<vector<char> > my_field(12, vector<char> (12, '.'));
       	vector<vector<char> > server_field(12, vector<char> (12, '.'));
        string command;
	bool playing = false;
	while (cin >> command)
	{
		if (command == "Begin")
		{
			playing = true;
			int number;
			flood(my_field); //функция заливки
			flood(server_field);
			cout << "Для начала давайте построим вашу площадку для игры.\n";
			cout << "Вы хотите расставить корабли самостоятельно или сгенерировать случайную комбинацию? Если первое, введите 1, иначе - 2.\n";
			do
			{
				cin >> number;
				if ((number < 1) || (number > 2))
				{
					cout << "Ошибка, вы можете ввести только 1 или 2\n";
				}
			}
			while ((number < 1) || (number > 2));
			if (number == 1)
			{
				int amount = 0, amounts[4], v1, v2;
				for (int i = 0; i < 4; ++i)
				{
					amounts[i] = 0;
				}
				char h1, h2;
				cout << "Всего вам нужно ввести кораблей, 1 четырехпалубный, 2 трехпалубных, 3 двухпалубных и 4 однопалубных.\n";
				cout << "Для создание корабля введите комбинацию из четырех символов, например, А 4 А 5 или С 7 D 7\n";
				while (amount < 10)
				{
					cin >> h1 >> v1 >> h2 >> v2;
					if ((v1 < 1) || (v2 < 1) || (v1 > 10) || (v2 > 10))
					{
						cout << "Вы вышли за границу поля. Число должно быть больше 0 и меньше 11.\n";
						continue;
					}
					if (!((h1 >= 'A') && (h1 <= 'J') && (h2 >= 'A') && (h2 <= 'J')))
					{
						cout << "Вы вышли за границу поля. Буквы должны быть не меньше А и не больше J.\n";
						continue;
					}
					if ((v1 != v2) && (h1 != h2))
					{
						cout << "Корабль должен быть параллелен одной из осей координат\n";
						continue;
					}
					if ((v1 - v2 > 4) || (h1 - h2 > 4))
					{
						cout << "Корабль слишком большой, он не может быть больше 4\n";
						continue;
					}
					int ih1 = int(h1) - int('A') + 1, ih2 = int(h2) - int('A') + 1;
					if (v2 < v1)
					{
						swap(v1, v2);
					}
					if (ih2 < ih1)
					{
						swap(ih1, ih2);
					}
					if (v1 == v2)
					{
						bool possible = true;
						for (int i = ih1 - 1; i < ih2 + 2; ++i)
						{
						        for (int j = -1; j < 2; ++j)
						        {
							        if ((v1 + j > 0) && (v1 + j < 11) && (i > 0) && (i < 11))
							        {
								        if (my_field[v1 + j][i] == 'X')
								        {
									        possible = false;
									        break;
								        }
							        }
						        }
							if (!possible)
							{
								break;
							}		
						}
						if (!possible)
						{
							cout << "Невозможно так ставить корабль, так как он будет соприкасаться с другим.\n";
							continue;
						}
						else
						{
							int number = ih2 - ih1;
							if (amounts[number] == 4 - number)
							{
								cout << "У вас уже достаточно кораблей этого типа.\n";
								continue;
							}
							++amounts[number];							
							for (int i = ih1; i < ih2 + 1; ++i)
							{
								my_field[v1][i] = 'X';
							}
							++amount;
							cout << "Отлично, мы построили поле с вашими кораблями!\n";
						}
					}
					else if (ih1 == ih2)
					{
						bool possible = true;
                                                for (int i = v1 - 1; i < v2 + 2; ++i)
                                                {
                                                        for (int j = -1; j < 2; ++j)
                                                        {
                                                                if ((ih1 + j > 0) && (ih1 + j < 11) && (i > 0) && (i < 11))
                                                                {
                                                                        if (my_field[i][ih1 + j] == 'X')
                                                                        {
                                                                                possible = false;
                                                                                break;
                                                                        }
                                                                }
                                                        }
                                                        if (!possible)
                                                        {
                                                                break;
                                                        }
                                                }
                                                if (!possible)
                                                {
                                                        cout << "Невозможно так ставить корабль, так как он будет соприкасаться с другим.\n";
                                                        continue;
                                                }
                                                else
                                                {
							int number = v2 - v1;
                                                        if (amounts[number] == 4 - number)
                                                        {
                                                                cout << "У вас уже достаточно кораблей этого типа.\n";
                                                                continue;
                                                        }
							++amounts[number];
                                                        for (int i = v1; i < v2 + 1; ++i)
                                                        {
                                                                my_field[i][ih1] = 'X';
                                                        }
                                                        ++amount;
							cout << "Отлично, мы построили поле с вашими кораблями!\n";
                                                }
					}
				}
			}
			else if (number == 2)
			{
				cout << "Если вы хотите сохранить данную генерацию, введите 1. Введите 2, если хотите сгенерировать новое расположение кораблей.\n";
			        int indicator;
				do
				{
					flood(my_field);
					random(my_field);
					print(my_field);
					do
					{
						cin >> indicator;
						if ((indicator < 1) || (indicator > 2))
						{
							cout << "Вы можете ввести только 1 или 2\n";
						}
					}
					while ((indicator < 1) || (indicator > 2));
				}
				while (indicator != 1);
			}
			send_message("Begin " + to_string(getpid()), socket);
			recieve_message(socket);
			cout << "Выберите действие, которое хотите сделать.\n";
			cout << "Чтобы сделать ход введите 'Try'.\n";
			cout << "Чтобы закончить игру и выйти введите 'Exit'.\n";
			cout << "Чтобы посмотреть свою статистику введите 'Statistics'.\n";
			cout << "Чтобы посмотреть на свое поле введите 'My'.\n";
			cout << "Чтобы посмотреть поле сервера введите 'Server'.\n";
			continue;
		}
		else if (command == "Get") // показывает на сервере какие там корабли
		{
			send_message("Get " + to_string(getpid()), socket);
			recieve_message(socket);
		}
		if (command == "Exit")
		{
			send_message("Exit " + to_string(getpid()), socket);
			string reply = recieve_message(socket);
			cout << reply;
			return 0;
		}
		if (command == "Statistics")
		{
			send_message("Statistics " + to_string(getpid()), socket);
			string reply = recieve_message(socket);
			cout << "У вас " + reply.substr(0, reply.find(" ")) + " побед и " + reply.substr(reply.find(" ") + 1) + " проигрышей\n";
			cout << "Введите ход\n";
			continue;
		}
		if (command == "My")
		{
			cout << "Ваше поле\n";
			print(my_field);
			cout << "Введите ход\n";
			continue;
		}
		if (command == "Amount") 
		{
			send_message("Amount " + to_string(getpid()), socket);
			recieve_message(socket);
			continue;
		}
		if (command == "Server")
		{
			cout << "Поле сервера\n";
			print(server_field);
			cout << "Введите ход\n";
			continue;
		}
		if (command == "Turns") // Cколько ходов осталось
		{
			send_message("Turns " + to_string(getpid()), socket);
			recieve_message(socket);
		}
		if (command == "Try")
		{
			if (!playing)
			{
				cout << "На данный момент вы не начали игру\n";
				continue;
			}
			else
			{
				int v;
				char h;
				while (true)
				{
					cin >> h >> v;
					if (!((h >= 'A') && (h <= 'J')))
					{
						cout << "Буквы должны находится в интервале между A и J\n";
						continue;	
					}
					else if ((v < 1) || (v > 10))
					{
						cout << "Цифры должны находится в интервале от 1 до 10\n";
						continue;
					}
					break;
				}
				send_message("Try" + to_string(int(h) - int('A')) + to_string(v - 1) + " " + to_string(getpid()), socket);
				string reply = recieve_message(socket);
				cout << "Ваш ход пользователь: " << reply << "\n";
				if ((reply == "Killed") || (reply == "Wounded"))
				{
					server_field[v][int(h) - int('A') + 1] = 'K';
					if (reply == "Killed")
					{
						cout << "Вы убили корабль сервера!\n";
					}
					else
					{
						cout << "Вы подбили корабль сервера!\n";
					}
					cout << "Введите ход\n";
					continue;
				}
				if (reply == "Another")
				{
					cout << "Вы уже вводили эти координаты, введите новые\n";
					continue;
				}
				if (reply == "Won")
				{
					cout << "Вы победили, поздравляю!\n";
					playing = false;
					continue;
				}
				if (reply == "Missed") //при промахе пользователя серверу отправляем Do с возможностью сделать ход
				{
					server_field[v][int(h) - int('A') + 1] = 'w';
					send_message("Do " + to_string(getpid()), socket);	
					while (true)
		                        {
						reply = recieve_message(socket);
					        if (reply.substr(0, 3) == "Try")
                                                {
							cout << "Очередь сервера: " << char(int(reply[4] - int('0') + 'A')) << " " << int(reply[3]) - int('0') + 1 << "\n";
                                                        //reply = to_string(int(reply[4]) - int('0')) + " " + reply[3];
                                                }
						else
						{
							cout << "Ход сервера: " << reply << "\n";
						}
						if ((reply == "Lost") || (reply == "Do"))
						{
							break;
						}
						int hor = int(reply[4]) - int('0') + 1, ver = int(reply[3]) - int('0') + 1;
						if (my_field[ver][hor] == 'X')
						{
							reply = "Killed"; // проверка убил или попал счервер
							int v = ver, h = hor;
							my_field[v][h] = 'K';
							
							while ((v > 1) && (my_field[v][h] == 'K'))
							{
								--v;
							}
							if (my_field[v][h] == 'X')
							{
								reply = "Wounded";
							}
							if (reply == "Killed")
							{
								v = ver; h = hor;
								while ((v < 10) && (my_field[v][h] == 'K'))
								{
									++v;
								}
								if (my_field[v][h] == 'X')
								{
									reply = "Wounded";
								}
								if (reply == "Killed")
								{
									v = ver; h = hor;
									while ((h > 1) && (my_field[v][h] == 'K'))
									{
										--h;
									}
									if (my_field[v][h] == 'X')
									{
										reply = "Wounded";
									}
									if (reply == "Killed")
									{
										v = ver; h = hor;
										while ((h < 10) && (my_field[v][h] == 'K'))
										{
											++h;
										}
										if (my_field[v][h] == 'X')
										{
											reply = "Wounded";
										}
									}
								}
							}
						}
						else
						{
							reply = "Missed";
							my_field[ver][hor] = 'w';
						}
						cout << "Ваш ход сервер " << reply << "\n";
						send_message(reply + " " + to_string(getpid()), socket); // здесь можно получить Kill если сервер попал
						//cout << '0' << endl;
					}
					if (reply == "Lost")
					{
						cout << "Вы проиграли эту игру\n";
						playing = false;
						continue;
					}
				}
			}
		}
	}
        return 0;
}