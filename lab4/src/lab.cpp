#include <iostream>
#include <string>
#include <sys/types.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <semaphore.h>
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <sys/mman.h> 
#include <cstdio>

using namespace std;
int flag_ = 0;

int child(string filename, char *mapped, string sem_file) {
	int count = 1;
	fstream file_1;
	file_1.open(filename, fstream::in | fstream::out | fstream::app);
	sem_t *semaphore = sem_open(sem_file.c_str(), 1);

	while (true) {

		if (sem_wait(semaphore) == -1) {
			perror("Semaphore error");
			exit(EXIT_FAILURE);
		}

		if (mapped[count] == '!') {
			break;
		}

		int str_size = (int)mapped[count];

		int start = count;
		char mas[str_size];
		int i = 0;

		for(; count < start + str_size; count++) {
			mas[i] = mapped[count + 1];
			i += 1;
		}

		string result;

		if (mas[0] >= 65 && mas[0] <= 90) {
			for(int i = 0; i < str_size; i++) {
				result.push_back(mas[i]);
				file_1 << mas[i];
			}
			file_1 << endl;
			cout << "Added string " << result << " to file!" << endl;
		} else {
			mapped[0] = 1;
		}
		sem_post(semaphore);
		
		count++;

        }
	return 0;
}

int main () 
{
	string filename;
	int flag;
	int strings_size;
	string sem_file = "a.semaphore";

	cout << "Enter name of file ";
	cin >> filename;
	cout << endl;

	cout<<"Enter amount of strings: ";
	int amount;
	cin >> amount;
	cout << endl;

	const int mapsize = amount*256;

	int flaccess = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; //права семафора

	sem_t *semaphore = sem_open(sem_file.c_str(), O_CREAT, flaccess, 0);

	if (semaphore == SEM_FAILED) {
		perror("Semaphore error");
		exit(EXIT_FAILURE);
	}

	char *mapped = (char *)mmap(0, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	pid_t id = fork();

	if (id == -1){

		perror("fork");
		cout << "1"; 
		exit(EXIT_FAILURE);
	} 

	else if (id == 0) {

		child(filename, mapped, sem_file);
		return 0;
		//execl("./child_", to_string(truba[0]).c_str(), to_string(truba[1]).c_str(), to_string(truba_2[0]).c_str(), to_string(truba_2[1]).c_str(), name.c_str(), NULL);

	} 

	if (id != 0) {
		string string_r;
		int start = 1;
		mapped[0] = 0;
		for (int i = 0; i < amount + 1; ++i) {
			if (i == amount) {
				mapped[start] = '!';
				if (mapped[0] == 1) {
					cout << "The string does not fit the rule" << endl;
					mapped[0] = 0;
				}
				sem_post(semaphore);
				break;
			}
			cin >> string_r;
			for (int j = 0; j < string_r.size() + 1; j++){
				if (j == 0) {
					mapped[start] = (char)string_r.size();
					continue;
				}
				mapped[start + j] = string_r[j - 1];
			}
			sem_post(semaphore); //разблакировка семафора
			sem_wait(semaphore);
			if (mapped[0] == 1) {
				cout << "The string does not fit the rule" << endl;
				mapped[0] = 0;
			}
			start += string_r.size() + 1;
		}
	}
	munmap(mapped, mapsize);
	sem_close(semaphore);
	sem_unlink(sem_file.c_str());

	return 0;
}











