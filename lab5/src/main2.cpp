#include<iostream>
#include<stdio.h>
#include<cmath>
#include<dlfcn.h>

#include"config.h"
#include<string>

using namespace std;


int main()
{
	cout << "ID компьютера: " << COMP_ID << endl;
	cout << "Версия компьютера: " << COMP_VER << endl;
	cout << "Дата сборки: "<< TIME_NOW << endl;
	cout << "Сейчас вы находитесь в 1 реализации программы " << endl;
	cout << "Записывайте комманды в виде: <command> <arg1> <arg2> ... <argn>" << endl;
	cout << "Если вы хотите посчитать интеграл функции sin(x) на отрезке [a, b] с шагом e, введите 1 <a> <b> <e> " << endl;
	cout << "Если вы хотите найти наибольший общий делитель двух натуральных чисел, введите 2 <a> <b> " << endl;
	cout << "Если вы хотите поменять реализацию программы, введите 0 <a> <b> " << endl;
	
	int command;
	string lib1 = "./liblib1.dylib"; // хранятся динамические библиотеки
	string lib2 = "./liblib2.dylib";

	void* cur_lib = dlopen(lib1.c_str(), RTLD_LAZY); //загружает динамическую библиотеку
	//RTLD_LAZY, подразумевающим разрешение неопределенных символов в виде кода, содержащегося в исполняемой динамической библиотеке
	float (*SinIntegral)(float a, float b, float e);
	int (*GCF)(int a, int b);
	SinIntegral = (float(*)(float, float, float))dlsym(cur_lib, "SinIntegral");
    GCF = (int(*)(int, int))dlsym(cur_lib, "GCF");


    int id = 1;

	while(cin >> command) {
		if (command == 0) {
			dlclose(cur_lib);
			if (id == 1) {
				cur_lib = dlopen(lib2.c_str(), RTLD_LAZY);
				id = 2;
				cout << "Теперь вы находитесь во 2 реализации программы " << endl;
			} else {
				cur_lib = dlopen(lib1.c_str(), RTLD_LAZY);
				id = 1;
				cout << "Теперь вы находитесь в 1 реализации программы " << endl;
			}
			SinIntegral = (float(*)(float, float, float))dlsym(cur_lib, "SinIntegral");
    		GCF = (int(*)(int, int))dlsym(cur_lib, "GCF");

		} else if (command == 2) {
			int a, b;
			cin >> a >> b;
			int res2 = GCF(a, b);
			cout << "Наибольший общий делитель " << a << " и " << b << " - " << res2 << endl;
		} else if (command == 1) {
			float a1, b1, e;
			cin >> a1 >> b1 >> e;
			cout << a1 << " " << b1 << " " << e << endl;
			float res1 = SinIntegral(a1, b1, e);
			cout << "Интеграл функции sin(x) на отрезке [" << a1 << ", " << b1 << "] с шагом " << e << " - " << res1 << endl; 
		} else {
			cout << "Неверно введенная команда. Повторите ввод" << endl;
		}
	}



}