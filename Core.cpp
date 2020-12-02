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

    Space playground = Space(20, 10, 3, 5);
    playground.run(50, 1000, false, true);
    
    cout << endl;
    return 0;
}

