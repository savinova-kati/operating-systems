#include<iostream>


using namespace std;

extern "C" float SinIntegral(float a, float b, float e)
{
	float square = 0;
	for (float i = a; i < b; i += e) {
		square += e * ((sin(i) + sin(i + e)) / 2);
	}
	return square;
}


extern "C" int GCF(int a, int b)
{
	int max_del = 0;
	if (a > b) {
		for (int i = 1; i <= b; i++) {
			if (a % i == 0 && b % i == 0) {
				max_del = i;
			}
		}
	} else {
		for (int i = 1; i <= a; i++) {
			if (a % i == 0 && b % i == 0) {
				max_del = i;
			}
		}
		
	}
	return max_del;
}


/*int main()
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




