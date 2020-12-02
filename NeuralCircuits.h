#pragma once

#include <vector>
#include <iostream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <Windows.h>

using namespace std;

class Neuron {

private:
    // parameters
    float rest = -70;
    float threshold = -55;
    float tau = 30;

    // variables
    float voltage = rest;
    float v = rest;
    char spike = 0;


 

public:
    float Run(float current) {
        voltage += (rest - voltage) / tau + current;
        v = voltage;
        Spiking();
        return v;
    }

    void Spiking() {
        if (voltage > threshold) {
            voltage = rest;
            spike = true;
        }
        else {
            spike = false;
        }

        Show();
    }

    void Show() {

        cout << "voltage: " << voltage << "\n" << endl;
        if (spike) {
            cout << "---------" << endl;
            cout << "|0000000|" << endl;
            cout << "|0000000|" << endl;
            cout << "|0000000|" << endl;
            cout << "---------" << endl;
        }
        else {
            cout << "---------" << endl;
            cout << "|       |" << endl;
            cout << "|       |" << endl;
            cout << "|       |" << endl;
            cout << "---------" << endl;
        }
    }
};


class Weights
{

private:

    float maxw = 20;
    float rangen(float p)
    {
        return (float)((double)rand() / (RAND_MAX)) < p;
    }

    void generate(float sparse = 0.4, float intensity = 5)
    {
        for (int i = 0; i < dim[0]; i++)
        {
            vector <float> row;
            for (int j = 0; j < dim[1]; j++)
            {
                row.push_back(rangen(sparse) * (float)((double)rand() / (RAND_MAX)) * intensity);
                //row.push_back(1);
            }
            values.push_back(row);
            output.push_back(0);
        }
    }

    void inhibitory(int n, int intensity=-20)
    {
        for (int i = 0; i < n; i++)
        {
            vector <float> row;
            for (int j = 0; j < n; j++)
            {
                row.push_back(intensity);
            }
            values.push_back(row);
            output.push_back(0);
        }
    }

public:
    int dim[2];
    vector <vector <float>> values;
    vector <float> output;

    Weights(int d1, int d2, float sparse = 0.5, float intensity = 5, bool inhibition = false, float mw = 3)
    {
        dim[0] = d1;
        dim[1] = d2;
        maxw = mw;
        if (!inhibition)
        {
            generate(sparse, intensity);
        }
        else
        {
            inhibitory(d1);
        }
        
    }

    template <typename T>
    void dot_produt(const vector <T> vec)
    {
        for (int j = 0; j < dim[0]; j++)
        {
            output[j] = inner_product(values[j].begin(), values[j].end(), vec.begin(), 0.0);
        }
    }

    void print_matrix()
    {
        cout << "\nmatrix: " << endl;
        for (int row = 0; row < dim[0]; row++)
        {
            for (int col = 0; col < dim[1]; col++)
            {
                cout << " " << values[row][col];
            }
            cout << endl;
        }
    }

    void print_vec(vector <float>& onevec, string title = "a vector", string dir = "horizontal")
    {
        cout << "\n" << title << endl;

        if (dir == "horizontal")
        {
            for (int entry = 0; entry < onevec.size(); entry++)
            {
                cout << " " << onevec[entry];
            }
        }
        else
        {
            for (int entry = 0; entry < onevec.size(); entry++)
            {
                cout << " " << onevec[entry] << endl;
            }
        }

        cout << endl;
    }

    float signum(float number) 
    {
        return (number > 1.0) - (number < 1.0);
    }

    void update(vector <vector <float>> ct, float dopamine)
    {
        for (int row = 0; row < dim[0]; row++)
        {
            for (int col = 0; col < dim[1]; col++)
            {

                values[row][col] += ct[row][col] * dopamine;
                if ( values[row][col] > maxw || values[row][col] < -maxw)
                {
                    values[row][col] = signum(values[row][col]) * maxw;
                }
                else if (abs(values[row][col]) < 0.2)
                {
                    values[row][col] = 0.0;
                }
            }
        }
    }
};


class Trace
{
private:
    float A = 10;
    float tau = 30;

    void generate()
    {
        for (int i = 0; i < dim; i++)
        {
            ut.push_back(i);
        }
    }

public:
    int dim;
    vector <float> ut;

    Trace(int d1, float A = 1, float tau = 10)
    {
        A = A;
        tau = tau;
        dim = d1;
        generate();
    }

    void Decay(vector <char> spikes)
    {
        for (int i = 0; i < dim; i++)
        {
            ut[i] += -ut[i] / tau + A * spikes[i];
      
        }
    }

};

class STDP
{

private:
    //parameters
    const float Aplus = 0.07;
    const float Aminus = -Aplus;
    const float tau = 10;

    void generate()
    {
        for (int i = 0; i < dim[0]; i++)
        {
            vector <float> row;
            for (int j = 0; j < dim[1]; j++)
            {
                row.push_back(0);
            }
            ct.push_back(row);
        }
    }

public:
    int dim[2];
    vector <vector <float>> ct;

    STDP(int d1, int d2)
    {
        dim[0] = d1;
        dim[1] = d2;

        generate();
    }


    void update(vector <char>& preS, vector <char>& postS, vector <float>& xt, vector <float>& yt)
    {
        for (int i = 0; i < dim[0]; i++)
        {
            for (int j = 0; j < dim[1]; j++)
            {
                ct[i][j] += -ct[i][j] / tau + Aplus * xt[j] * postS[i] + Aminus * yt[i] * preS[j];
            }
        }
    }
};



class SensoryNetwork {
private:

    static const int N = 40;
    const int quarter = N/4;

    // neuronal dynamics
    const float threshold = -55;
    const float rest = -70;
    const int tau = 30;
    int distance;
    
    float voltages[N] = { rest };
    float current[N];
    char flashes[N] = { ' ' };

    // visual parameters
    int stdv;
    int radius;

    // set the initial values
    void set_net() {
        for (int i = 0; i < N; i++)
        {
            spikes.push_back(0);
            list.push_back(i);
        }
    }
    
    // gaussian kernel
    float kernel(float angle, int id) {
        return exp(-0.5 * pow((id - angle) / stdv, 2)) / (stdv * 2.4778);
    }

    void dispersion(float angle) {
        for (int id: list)
        {
            current[id] = kernel(angle, id);
        }
        
        float max_ = *max_element(current, current+N);
        for (int id : list) 
        {
            current[id] = current[id] / max_;
        }
    }

    float rangen(float p)
    {
        return (float) ((double)rand() / (RAND_MAX)) < p;
    }
    
public:
    
    vector <int> list = {};
    vector <char> spikes;
    
    SensoryNetwork(int std=20, int d=20) {
 
        stdv = std;
        distance = d;

        set_net();
    }

    void run(float angle) {
        dispersion(angle);
        for (int i: list)
        {
            voltages[i] += (rest - voltages[i]) / tau + rangen(current[i]) * distance;
            if (voltages[i] >= threshold)
            {
                spikes[i] = 1;
                flashes[i] = '#';
                voltages[i] = rest;
                continue;
            }
            spikes[i] = 0;
            flashes[i] = '.';
            
        }
    }

    void show() 
    {
        cout << endl;
        for (int j = 0; j < N; j++)
        {
            cout << flashes[j];
        } 
        cout << endl;
    }
};


class Pool
{
private:

    // population
    static const int N = 100;
    const int root = (int) sqrt(N);
    int list[N];

    Weights weights = Weights(N, N, 0.05, 3);
   
    // neuronal dynamics
    const float threshold = -55;
    const int rest = -70;
    const int tau = 10;
    float noise_lv;

    float voltages[N] = { (float) rest };
    char flashes[N] = { ' ' };

    int count = 0;


    // set the initial values
    void set_net() 
    {
        for (int i = 0; i < N; i++)
        {
            spikes.push_back(0);
            list[i] = i;
        }
    }

    float rangen(float& p)
    {
        return (float)((double)rand() / (RAND_MAX)) < p;
    }

public:
    vector <char> spikes;

    Pool(float noise)
    {
        noise_lv = noise;
        set_net();
    }

    void printV()
    {
        cout << "voltages: \n" << endl;
        for (int u = 0; u < N; u++)
        {
            cout << " " << voltages[u];
        }
    }

    void run(vector <float>& stimuli)
    {   
        //system("cls");
        weights.dot_produt(spikes);
        for (int i = 0; i < N; i++)
        {
            voltages[i] -= (voltages[i] - rest) / tau;

            //current[i] = rangen(noise_lv);
            //cout << current[i] ;

            voltages[i] += rangen(noise_lv) * 20 + weights.output[i] + stimuli[i];
            
            
            if (voltages[i] > threshold) {
                flashes[i] = '#';
                spikes[i] = 1;
                voltages[i] = rest;
                continue;
            }            
            spikes[i] = 0;
            flashes[i] = '.';
        }
        //cout << endl;
        //Sleep(300);
    }


    void show()
    {
        count = 0;
        cout << endl;
        for (int row = 0; row < root; row++)
        {
            for (int id = 0; id < root; id++)
            {
                cout << ' '<< flashes[count];
                count++;
            }
            cout << endl;
        }
        cout << endl;
    }
};


class PreMotor
{
    static const int N = 10;
    const int threshold = -55;
    const int rest = -70;
    const int tau = 30;

    Weights weights = Weights(N, N, 0.3);
    Weights weights_out = Weights(1, N, 1);

    float voltages[N] = { (float)rest };
    float v = (float)rest;
    char flashes[N] = { ' ' };
    char flash = ' ';

    void set()
    {
        for (int i = 0; i < N; i++)
        {
            spikes.push_back(i);
        }
    }


public:
    vector <char> spikes;
    char outspike;

    void run(vector <float> inputs, char inhibition = 0)
    {
        weights.dot_produt(spikes);
        for (int i = 0; i < 4; i++)
        {
            // premotor
            voltages[i] -= (voltages[i] - rest) / tau;
            voltages[i] += inputs[i] + weights.output[i];

            if (voltages[i] > threshold) {
                flashes[i] = '#';
                spikes[i] = 1;
                voltages[i] = rest;

                continue;
            }
            spikes[i] = 0;
            flashes[i] = '.';
        }

        // motor output
        weights_out.dot_produt(spikes);
        v -= (v - rest) / tau;
        v += weights_out.output[0] - 20 * inhibition;

        if (v > threshold)
        {
            flash = '#';
            outspike = 1;
            v = rest;
        }
        else
        {
            flash = ' ';
            outspike = 0;
        }
    }

    void show()
    {
        cout << endl;
        for (char f : flashes)
        {
            cout <<f;
        }
        cout << endl;
    }

};


class Motor
{
private:

    const int threshold = -55;
    const int rest = -70;
    const int tau = 30;

    Weights weights = Weights(4, 4, 1, 0, true);
    //Weights weights_pre = Weights(4, premotor.getN, 1))


    float voltages[4] = { (float) rest };
    char flashes[4] = { ' ' };

public:
    vector <char> spikes = { 0, 0, 0, 0 };
    int active = 0;
    PreMotor premotor = PreMotor();

    void run(vector <float> inputs)
    {   
        active = 0;
        //cout << "\nout volt " << endl;
        for (int i = 0; i < 4; i++)
        {
            voltages[i] -= (voltages[i] - rest) / tau - inputs[i];

            voltages[i] -= (voltages[i] - rest) * (spikes[0] * (i != 0) + spikes[1] * (i != 1)
                + spikes[2] * (i != 2) + spikes[3] * (i != 3)); //+ weights.output[i];

            //cout << " " << voltages[i];
            if (voltages[i] > threshold) {
                flashes[i] = '#';
                spikes[i] = 1;
                voltages[i] = rest;
                active = i+1;
                //weights.dot_produt(spikes);

                continue;
            }
            
            spikes[i] = 0;
            flashes[i] = '.';            
        }
        //Sleep(500);
    }

    void show()
    {
        cout << endl;
        for (char f : flashes)
        {
            cout << ' ' << f << ' ';
        }
        cout << endl;
    }

};


class Brain
{
private:

    int n_sight = 40;
    int n_pool = 100;

    Weights weightsIN = Weights(n_pool, n_sight, 0.2, 0);
    Weights weightsOUT = Weights(4, n_pool, 0.25, 15);

    Trace Xt_sight = Trace(n_sight, 0.1, 3);
    Trace Xt_pool = Trace(n_pool, 0.1, 3);
    Trace Xt_mot = Trace(4, 0.1, 3);
    
    STDP updt_in = STDP(n_pool, n_sight);
    STDP updt_out = STDP(4, n_pool);

    // dopamine

    const float dinj = 0.3;
    const int tau = 1;

    float dopamine = 0;


public:
    SensoryNetwork sight = SensoryNetwork(2);
    Pool pool = Pool(0.03);
    Motor motor = Motor();

    void run(int angle, bool reward=false)
    {

        // forward
        sight.run(angle);
        
        weightsIN.dot_produt(sight.spikes);
        pool.run(weightsIN.output);
      
        weightsOUT.dot_produt(pool.spikes);
        motor.run(weightsOUT.output);

        // trace update
        Xt_sight.Decay(sight.spikes);
        Xt_pool.Decay(pool.spikes);
        //Xt_mot.Decay(motor.spikes);

        // stpd
        updt_in.update(sight.spikes, pool.spikes, Xt_sight.ut, Xt_pool.ut);
        updt_out.update(pool.spikes, motor.spikes, Xt_pool.ut, Xt_mot.ut);

        weightsIN.update(updt_in.ct, dopamine);
        //weightsOUT.update(updt_out.ct, dopamine);

        dopamine += -dopamine / tau + dinj * reward;
        if (reward)
        {
            cout << "Rewarded!\n dopamine: " << dopamine << " inj " << dinj * reward << endl;
            Sleep(1000);
        }
    }


    void show_all()
    {
        system("cls");
        cout << "out: " << motor.active << endl;
        sight.show();
        pool.show();
        motor.show();
        Sleep(100);
    }

};

