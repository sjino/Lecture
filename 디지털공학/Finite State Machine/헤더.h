#pragma once


class input_forbox
{
public:
	//input_forbox(int num_of_inputbits, int state_number, int output, int *next_state);
	input_forbox();
	~input_forbox();
	int *next_state;
	int state_number;
	int output;
	void copy(int num_of_inputbits, int state_number, int output, int *next_state);
private:

};


class Box
{
public:
	Box();
	~Box();
	input_forbox left_input;
	input_forbox bottom_input;
	//int position[2];
	void comparison(bool **truthtable);
	void comparison(bool **truthtable, bool Alive);
	bool Alive;
private:

};


