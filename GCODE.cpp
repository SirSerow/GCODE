/*=================================================
	Program operation sequence:
		1. Read G-CODE file ( 5 different .txt test files are included )
		START WHILE LOOP
		{
			2. Scan code line
			3. Define divider
			4. Separate words
			5. Read the GXX value (circular and linear are are similar in this case)
			6. Read coordinates
				4.1. Generate an exception in case of an input mistake
			7. Calculate new coordinates
			8. Set new coordinates
			9. Repeat untill the end of an input file
		}
		10. Print result
  =================================================*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

vector<string> standart_commands = {													//Create vector for standart commands
	"G00",
	"G01",
	"G02",
	"G03"
};

void set_start_coordinates(vector<float>& start_pos) {									//Following function inputs start coordinates
	cout << "Enter start coordinates [X,Y,Z]\n";
	cout << "X:\n";
	cin >> start_pos[0];
	cout << "Y:\n";
	cin >> start_pos[1];
	cout << "Z:\n";
	cin >> start_pos[2];
}

void divide(const string& s, char delim, vector<string>& elems) {						//Following function divides string into separate words
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

vector<string> sep(const string& s, char delim) {										//Following function creates vector for separate words (commands)
	vector<string> elems;
	divide(s, delim, elems);
	return elems;
}

void move(vector<float>& disp_values, vector<float>& previous_position) {				//Following function changes current coordinates of manipulator
	for (int i = 0; i < 3; i++)
		previous_position[i] += disp_values[i];
}

float convert(string a) {																//Following function converts XYZ G-code command to readable float value
	a.erase(0, 1);
	float val_float = stof(a);
	return val_float;
}

vector<float> read_disp(vector<string>& elems) {										//Following function scans displacement values and returns a vector of float values
	bool invalid_input = 1;
	vector<float> disps(3);
	for (int i = 1; i < 4; i++) {
		if (elems[i].at(0) != 'X' && elems[i].at(0) != 'Y' && elems[i].at(0) != 'Z') {
			throw invalid_input;
			break;
		}
		else if (elems[i].at(0) == 'X')
			disps[0] = convert(elems[i]);
		else if (elems[i].at(0) == 'Y')
			disps[1] = convert(elems[i]);
		else if (elems[i].at(0) == 'Z')
			disps[2] = convert(elems[i]);
	}
	return disps;
}

bool check_command(string input) {														//Following function G-code command type
	int i = 0;
	bool encounter = 0;
	for (i = 0; i < standart_commands.size(); i++) {
		if (!input.compare(standart_commands[i]))
			encounter = 1;
	}
	return encounter;
}

char set_divider(string line) {															//Following function defines inline dividers, two formats avaliable:
	int cntr;																			//Format 1: G00 X1 Y10 Z-7
	char semicolon = ';';																//Format 2: X10;Y20;Z0;
	char space = ' ';
	size_t found = line.find(space);
	if (found != string::npos)
		return space;
	else
		return semicolon;
}

int main() {

	vector<string> commands;															//Create a vector for inline commands
	vector<float> disps(3);																//Create a vector for displacement values
	vector<float> current_pos(3);														//Create a vector for manipulator position coordinates
	char divider;																		//Create a Char variable for divider storage

	set_start_coordinates(current_pos);													//Get manipulator's start coordinates

	string filename = "GCODE_test4.txt";												//Specify input file

	ifstream input;

	input.open(filename);																//Open the file						

	if (!input.is_open())																//Check if it is possible to open the file
		cout << "Could not open the file\n";


	while (input.peek() != EOF) {														//Repeat untill the file ends
		string line;																//Initialize a string for storing the input line

		getline(input, line, '\n');												//Read a line from file

		divider = set_divider(line);													//Define a divider

		commands = sep(line, divider);													//Split line on words

		if (divider == ';')																//Add movement type command if the divider is not a space
			commands.insert(commands.begin(), "G01");

		if (check_command(commands[0])) {												//Read the first command in the line, if the command means movement => proceed
			try {
				read_disp(commands).swap(disps);										//Check corectness of coordinates input format
			}
			catch (bool invalid_input) {												//Generate an exception in case of the wrong input
				std::cout << "ERROR SCRIPT\n";											//Output the exception message
				break;
			}

			move(disps, current_pos);													//Set new coordinates of the manipulator
		}
		else
			continue;
	}

	for (auto i : current_pos)															//Output the result
		cout << i << ' ';

	input.close();

	return 0;
}