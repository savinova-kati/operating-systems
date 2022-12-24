#include<iostream>
#include<stdio.h>
#include<cmath>
#include"lib.h"
#include"config.h"

using namespace std;

int main()
{
	cout << "ID компьютера: " << COMP_ID << endl;
	cout << "Версия компьютера: " << COMP_VER << endl;
	cout << "Дата компиляции: "<< TIME_NOW << endl;
	cout << "Записывайте комманды в виде: <command> <arg1> <arg2> ... <argn>" << endl;
	cout << "Если вы хотите посчитать интеграл функции sin(x) на отрезке [a, b] с шагом e, введите: 1 <a> <b> <e> " << endl;
	cout << "Если вы хотите найти наибольший общий делитель двух натуральных чисел, введите: 2 <a> <b> " << endl;
	int command;
	while(cin >> command) {
		if (command == 2) {
			int a, b;
			cin >> a >> b;
			int res2 = GCF(a, b);
			cout << "Наибольший общий делитель " << a << " и " << b << " - " << res2 << endl;
		} else if (command == 1) {
			float a1, b1, e;
			cin >> a1 >> b1 >> e;
			float res1 = SinIntegral(a1, b1, e);
			cout << "Интеграл функции sin(x) на отрезке [" << a1 << ", " << b1 << "] с шагом " << e << " - " << res1 << endl; 
		} else {
			cout << "Неверно введенная команда. Повторите ввод" << endl;
		}
	}

}