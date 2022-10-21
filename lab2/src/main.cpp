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

int main(){

	fstream file_1;

	string name;
	cout<<"Enter filename: "<<endl;

	cin >> name;


	int truba[2];
	int truba_2[2];
	int pipe_1[2];
	int pipe_2[2];

	if (pipe(pipe_1) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	if (pipe(pipe_2) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}


	string string_r;

	pid_t id = fork();

	if (id == -1){
		perror("fork");
		cout << "1"; 
		exit(EXIT_FAILURE);
	} 

	else if (id == 0) {
		truba[0] = pipe_1[0];
		truba[1] = pipe_1[1];
		truba_2[0] = pipe_2[0];
		truba_2[1] = pipe_2[1];


		execl("./child_", to_string(truba[0]).c_str(), to_string(truba[1]).c_str(), to_string(truba_2[0]).c_str(), to_string(truba_2[1]).c_str(), name.c_str(), NULL);

	} 

	else {
		cout<<"Enter amount of strings: ";
		int amount;
		cin >> amount;
		cout << endl;
		for (int i = 0; i < amount; ++i) {
			cin >> string_r;
			int s_size = string_r.size();
			char str_array[s_size];
			for (int k = 0; k < s_size; ++k) {
				str_array[k] = string_r[k];
			}

			write(pipe_1[1], &s_size, sizeof(int));
			write(pipe_1[1], str_array, sizeof(char)*s_size);

			int flag_;

			read(pipe_2[0], &flag_, sizeof(int));

			//cout << truba_2[0] << endl;
			if (flag_ == 1) {
				cout << "The string does not fit the rule" << endl;
			}
		}
	}

	close(pipe_1[0]);
	close(pipe_1[1]);
	close(pipe_2[0]);
	close(pipe_2[1]);

	return 0;

}
