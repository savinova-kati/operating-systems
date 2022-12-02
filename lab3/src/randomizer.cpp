#include <iostream>
#include <ctime>
#include<fstream> 
#include<vector>                   
using namespace std;

int main(int argc, char* argv[])
{
    ofstream out;
    out.open("filename.txt");
    srand(time(0));
    char randomm[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    const int file_size = 100000; // размер файла
    for (int i = 0; i < file_size; i++) {
        for (int j = 0; j < 32; j++) {
            int x = rand() % 16;
            out << randomm[x];
        }
        out << endl;
    }
    
    cout << "runtime = " << clock()/1000.0 << endl; // время работы программы                  
    return 0;
}