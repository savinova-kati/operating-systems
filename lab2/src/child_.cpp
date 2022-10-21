#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>


using namespace std;


int main(int argc, char *argv[]){	

	string filename = argv[4];
	int truba[2];
	int truba_2[2];
	int flag1 = 1;
	int flag2 = 22;

	truba[0] = stoi(argv[0]);
	truba[1] = stoi(argv[1]);

	truba_2[0] = stoi(argv[2]);
	truba_2[1] = stoi(argv[3]);

	
	fstream file_1;
	file_1.open(filename, fstream::in | fstream::out | fstream::app);

	while(true) {
		int stroka_size;

		read(truba[0], &stroka_size, sizeof(int));
		char stroka[stroka_size];
		read(truba[0], &stroka, sizeof(char)*stroka_size);

		string result;
		for (int i = 0; i < stroka_size; i++) {
			result.push_back(stroka[i]);
		}
		if (stroka[0] >= 65 && stroka[0] <= 90) {
			file_1 << result << endl;
			cout << "Added string " << result << " to file!" << endl;

			write(truba_2[1], &flag2, sizeof(int));

			//cout << truba_2[1] << endl;
		} else {

			write(truba_2[1], &flag1, sizeof(int));

			//cout << truba_2[1] << endl;
		}

	}

	return 0;





}