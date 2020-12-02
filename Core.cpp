#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include "NeuralCircuits.h"
#include "Environment.h"
#include <Windows.h>
#include <random>
#include <ctime>


using namespace std;


int main() {

    srand(time(NULL));

    vector <int> y = { 3, 4, 5, 2, 1, 4 };


    Space mercury = Space(20, 10, 3, 5);
    mercury.run(50, 1000, false, true);
    
    cout << endl;
    return 0;
}

