#include <iostream>
#include "Çì´õ.h"
#include <vector>
#include <list>
#include <fstream>
#include <math.h>
#pragma warning(disable:4996)

using namespace std;
void parsing_txt();

input_forbox *inputs;
int number_of_states = NULL;
int number_of_inputbits = NULL;
int number_of_Aliveboxes = 0;
bool isadded = false;
void printing_txt();
bool **truthtable;


Box::Box()
{
	Alive = true;
}

Box::~Box()
{
}

void Box::comparison(bool **truthtable) 
{
	if (left_input.output != bottom_input.output)
	{
		this->Alive = false;
		truthtable[left_input.state_number][bottom_input.state_number] = false;
		truthtable[bottom_input.state_number][left_input.state_number] = false;
		isadded = true;

	}
}
void Box::comparison(bool **truthtable, bool Alive)
{
	if (Alive)
	{
		int i = 0;
		while (true)
		{
			
			if (truthtable[bottom_input.next_state[i]][left_input.next_state[i]] == false)
			{
				this->Alive = false;
				truthtable[left_input.state_number][bottom_input.state_number] = false;
				truthtable[bottom_input.state_number][left_input.state_number] = false;
				isadded = true;
				break;
			}
			if (i == (int)pow(2, number_of_inputbits) - 1)
			{
				break;
			}
			i++;
		}

		
		
	}

}

input_forbox::input_forbox()
{
	this->next_state = new int[(int)pow(2, number_of_inputbits)];

}
/*
input_forbox::input_forbox(int num_of_inputbits, int state_number, int output, int *next_state)
{
	next_state = new int[(int)pow(2, number_of_inputbits)];
	this->state_number = state_number;
	this->output = output;
	for (int i = 0; i <(int)pow(2, number_of_inputbits); i++)
	{
		this->next_state[i] = next_state[i];
	}
}*/
input_forbox::~input_forbox()
{
	//delete[] next_state;
}
void input_forbox::copy(int num_of_inputbits, int state_number, int output, int *next_state)
{
	//this->next_state = new int[(int)pow(2, number_of_inputbits)];
	this->state_number = state_number;
	this->output = output;
	for (int i = 0; i <(int)pow(2, number_of_inputbits); i++)
	{
		this->next_state[i] = next_state[i];
	}
}


int main()
{
	parsing_txt();
	Box **table;
	number_of_Aliveboxes = (number_of_states - 1)*(number_of_states) / 2;
	table = new Box*[number_of_states - 1];
	for (int i = 0; i < number_of_states - 1; i++)
	{
		table[i] = new Box[i + 1];
	}

	truthtable = new bool*[number_of_states];
	for (int i = 0; i < number_of_states; i++)
	{
		truthtable[i] = new bool[number_of_states];
	}

	for (int i = 0; i < number_of_states; i++)
	{
		for (int j = 0; j < number_of_states; j++)
		{
			truthtable[i][j] = true;
		}
	}

	for (int i = 0; i < number_of_states - 1; i++)
	{
		for (int j = 0; j < i + 1; j++)
		{
			table[i][j].left_input.copy(number_of_inputbits, inputs[i + 1].state_number, inputs[i + 1].output, inputs[i + 1].next_state);
			table[i][j].bottom_input.copy(number_of_inputbits, inputs[j].state_number, inputs[j].output, inputs[j].next_state);
			table[i][j].comparison(truthtable);
		}
	}
	while (true)
	{
		isadded = false;
		for (int i = 0; i < number_of_states - 1; i++)
		{
			for (int j = 0; j < i + 1; j++)
			{

				table[i][j].comparison(truthtable, table[i][j].Alive);
			}
		}
		if (isadded == false)
		{
			break;
		}
	}
	
	printing_txt();

	cout << "check the output.txt" << endl;
}

void printing_txt()
{
	ofstream outFile("output.txt");
	bool* Isused = new bool[number_of_states];
	cout << "********Output***********" << endl;
	for (int i = 0; i < number_of_states; i++)
	{
		Isused[i] = false;
	}
	for (int i = 0; i < number_of_states; i++)
	{
		if (Isused[i]==false)
		{

			outFile << i;
			cout << i;
			for (int j = i+1; j < number_of_states; j++)
			{
				if (truthtable[i][j]==true)
				{
					outFile <<" "<< j;
					cout << " " << j;
					Isused[j] = true;
				}
				if (j == number_of_states - 1)
				{
					outFile << endl;
					cout << endl;
				}
			}
		}
	}
	
	outFile.close();
}


void parsing_txt()
{
	int linecount = 0;
	char line_tmp[100];
	string filepath = "input.txt";
	ifstream openFile("input.txt");
	cout << "********Input***********" << endl;
	if (openFile.is_open())
	{
		string number;
		while (!openFile.eof())
		{
			openFile.getline(line_tmp, sizeof(line_tmp));
			cout << line_tmp << endl;
			switch (linecount)
			{
			case 0:
				if (atoi(line_tmp) == 0)
				{
					cout << "number of states occurs error" << endl;
				}
				else
				{
					number_of_states = atoi(line_tmp);
				}
				break;
			case 1:
				if (atoi(line_tmp) == 0)
				{
					cout << "number of input bits occurs error" << endl;
				}
				else
				{
					number_of_inputbits = atoi(line_tmp);
					inputs = new input_forbox[number_of_states];
				}
				break;
			default:
				char *ptr = strtok(line_tmp, " ");
				int count = 0;
				int output_tmp = NULL;
				int *next_state_tmp = new int[(int)pow(2, number_of_inputbits)];

				while (ptr != NULL)
				{
					if (count == (int)pow(2, number_of_inputbits))
					{
						output_tmp = atoi(ptr);
					}
					else
					{
						next_state_tmp[count] = atoi(ptr);
					}
					ptr = strtok(NULL, " ");
					count++;
				}
				input_forbox *input_tmp = new input_forbox();
				input_tmp->copy(number_of_inputbits, linecount - 2, output_tmp, next_state_tmp);
				inputs[linecount - 2] = *input_tmp;

				delete[] next_state_tmp;
				//delete[] input_tmp;
				break;
			}
			linecount++;
		}
		openFile.close();
	}


}