#include<iostream>


using namespace std;
// extern позволяет компилятору знать о типах и именах глобальных переменных без действительного создания этих переменных
extern "C" float SinIntegral(float a, float b, float e) 
{
	float square = 0;
	for (float i = a; i <= b; i += e) {
		square += e * sin(i);
	}
	return square;
}


extern "C" int GCF(int a, int b)
{
	while (a != 0 && b != 0) {
		if (a > b) {
			a = a % b;
		} else {
			b = b % a;
		}
	}
	return a + b;
}


/*
int main()
{

	int a, b;
	float a1, b1, e;

	cin >> a; 
	cout << endl;
	cin >> b; 
	cout << endl;
	cin >> a1; 
	cout << endl;
	cin >> b1; 
	cout << endl;
	cin >> e; 
	cout << endl;

	cout << SinIntegral(a1, b1, e) << endl;
	cout << GCF(a, b) << endl;

	return 0;
}
*/

