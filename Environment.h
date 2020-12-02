#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <Windows.h>
#include "NeuralCircuits.h"

using namespace std;


class Agent
{
	int range[2];
public:
	string name;
	int pos[2];
	Brain brain = Brain();
	bool life = true;
	int points = 0;

	Agent(string new_name)
	{
		name = new_name;
	}

	void set_pos(int x, int y)
	{
		pos[0] = x;
		pos[1] = y;
	}

	void set_initial(int xlim, int ylim)
	{
		range[0] = xlim;
		range[1] = ylim;
	}

	void change()
	{
		pos[0] = (int) (((double)rand() / (RAND_MAX)) * range[0]);
		pos[1] = (int) (((double)rand() / (RAND_MAX)) * range[1]);
	}

	void run(int angle, bool reward)
	{
		brain.run(angle, reward);
		move();
	}

	void move()
	{
		//cout << "active: " << brain.motor.active << endl;
		if (brain.motor.active == 1)
		{
			pos[0]++;
		}
		else if (brain.motor.active == 2)
		{
			pos[0]--;
		}
		else if (brain.motor.active == 3)
		{
			pos[1]++;
		} 
		else if (brain.motor.active == 4)
		{
			pos[1]--;
		}
	}
};



struct Food
{
	int pos[2];
	int range[2];

	void set_pos(int xlim, int ylim)
	{
		pos[0] = xlim / 2;
		pos[1] = ylim / 2;
		range[0] = xlim;
		range[1] = ylim;
	}

	void reset()
	{
		pos[0] = (float) range[0] / 2;
		pos[1] = (float) range[1] / 2;
	}

	void change()
	{
		pos[0] = (int)(((double)rand() / (RAND_MAX)) * range[0]);
		pos[1] = (int)(((double)rand() / (RAND_MAX)) * range[1]);
		
	}
};

class Space
{
	int size[2];
	vector <vector <char>> soil;
	bool reward = false;

	float angle = 0;
	int ang_sin = 0;
	int ang_cos = 0;
	int dx = 0;
	int dy = 0;
	float distance = 0;
	bool go = true;
	string dead = " -is dead    ";

	int x0;
	int y0;

	float forangle = (float) 4 / 36;

	void build_soil()
	{
		for (int i = 0; i < size[0]; i++)
		{
			vector <char> row;
			for (int j = 0; j < size[1]; j++)
			{
				row.push_back(' ');
			}
			soil.push_back(row);
		}
	}

	void measurement()
	{
		dx = agent.pos[0] - cherry.pos[0];
		dy = cherry.pos[1] - agent.pos[1];
		distance = sqrt((float) pow(dx, 2) + (float) pow(dy, 2));
		//try
		
		ang_sin = (acos((float) dy / distance))* 57.324;
		
		/*catch (const runtime_error & e)
		{
			ang_sin = 0;
		}*/

		//try
		
		ang_cos = (acos((float) dx / distance)) * 57.324;
		
		//catch (const runtime_error & e)
		//{
		//ang_cos = 0;
		//}
		
		/*if (ang_sin <= 90  && ang_cos < 90)
		{
			angle = ang_cos;
		}
		else if (ang_sin <= 90 && ang_cos > 90)
		{
			angle = ang_cos;
		}*/

		//cout << "\nang sin " << ang_sin << " ; ang cos " << ang_cos << "\ndistance " << distance << endl;;
		if (ang_sin <= 90 && ang_cos <= 90)
		{
			angle = ang_cos;
		}
		else if (ang_sin <= 90 && ang_cos > 90)
		{
			angle = 360 - ang_sin;
		}
		else if (ang_sin > 90 && ang_cos <= 90)
		{
			angle = ang_sin;
		}
		else if (ang_sin >= 90 && ang_cos > 90)
		{
			angle = 360 - ang_sin;
		}
	}

	void check_boundaries()
	{
		if (0 > agent.pos[0] || agent.pos[0] > size[0] || 0 > agent.pos[1] || agent.pos[1] > size[1]) 
		{
			agent.life = false;
			go = false;
		}
	}

	void reset()
	{
		agent.change();
		//agent.set_pos(x0, y0);
		agent.life = true;
		cherry.reset();
		go = true;

		system("cls");
		
		Sleep(20);
	}

public:

	Agent agent = Agent("Alexandra");
	Food cherry = Food();

	Space(int width, int height, int x, int y) {
		size[0] = width;
		size[1] = height;
		x0 = x;
		y0 = y;
		build_soil();

		agent.set_initial(width, height);
	    agent.set_pos(x, y);
		agent.change();
		cherry.set_pos(width, height);
	}

	void update()
	{

		measurement();

		agent.run(angle * forangle, reward);
		check_boundaries();

		if (agent.pos[0] == cherry.pos[0] && agent.pos[1] == cherry.pos[1]) 
		{
			reward = true;
			agent.points++;
			reset();
		}
		else
		{
			reward = false;
		}
	}

	void check1()
	{
		
		cherry.change();
		agent.change();
		
		cout << "pos agent " << agent.pos[0] << " " << agent.pos[1] << endl;
		cout << "pos food  " << cherry.pos[0] << " " << cherry.pos[1] << endl;

		cout << "dx " << agent.pos[0] - cherry.pos[0] << "  dy " << cherry.pos[1] - agent.pos[1] << endl;
		
		measurement();
		cout << "\nangle " << angle << endl;

		display(false);
	}

	void display(bool single = true)
	{
		if (single)
		{
			system("cls");
		}
		cout << "\nnpoints " << agent.points << " | x " << agent.pos[0] << " y " << agent.pos[1] << " | angle: " << angle <<
			" | action: " << agent.brain.motor.active << "\n" << endl;

		for (int y = 0; y < size[1]; y++)  // y axis
		{
			for (int x = 0; x < size[0]; x++)  // x axis
			{
				if (y == cherry.pos[1] && x == cherry.pos[0])
					cout << '@';
				else if (y == agent.pos[1] && x == agent.pos[0])
					cout << 'X';
				else if (x == 0 || x == size[0]-1)
					cout << '.';
				else if (y == 0 || y == size[1]-1)
					cout << '.';
				else
					cout << soil[x][y];
			}
			cout << endl;
		}
		//Sleep(30);
	}

	void run(int time = 200, int epochs = 1000, bool show_space = true, bool show_both = false)
	{
		for (int epoch = 0; epoch < epochs; epoch++)
		{
			reset();
			for (int t = 0; t < time; t++)
			{
				update();
				if (!go)
				{
					break;
				}
				
				if (show_space)
				{
					display();
				}
				else if (show_both)
				{
					
					//Sleep(100);
					agent.brain.show_all();
					cout << endl;
					cout << "the Agent is alive, angle " << angle << " " << "stimulation current " << angle * forangle << endl;
					display(false);
					Sleep(100);
				}
				else
				{
					//Sleep(10);
					agent.brain.show_all();
				}
				
				
			}
			reset();
		}
		system("clf");
		cout << "\nfinal score: " << agent.points << endl;
		cin.get();
	}
};
