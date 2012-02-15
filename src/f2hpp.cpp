/*
 * f2hpp.cpp
 * Copyright (C) 2012 David Jolly
 * ----------------------
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/*
 * Supported commands
 */
enum CMD { NOT_CMD, HELP_CMD, VERSION_CMD, INPUT_CMD, OUTPUT_CMD };

/*
 * Bytes between newline
 */
const unsigned int DIVIDER = 20;

/*
 * Messages
 */
const std::string HELP("-h\t\tDisplay help information\n-v\t\tDisplay version information\n-f [input]\tSpecify input file path\n-o [output]\tSpecify output file path");
const std::string USAGE("Usage: f2hpp [-h | -v] [-f input | -o output]");
const std::string VERSION("f2hpp 0.1.0 -- Copyright (C) 2012 David Jolly");
const std::string WARRANTY("This is free software. There is NO warranty.");

/*
 * Prototypes
 */
bool gen_file(const std::string &in_path, const std::string &out_path);
int is_command(const std::string &str);
void to_uppercase(std::string &str);

/*
 * Convert a bin file into a hpp file
 */
bool gen_file(const std::string &in_path, const std::string &out_path) {
	unsigned char ch;
	size_t pos, size, count = 0;
	std::string format_header, format_define, format_class;
	
	// attempt to open input file
	std::ifstream in(in_path.c_str(), std::ios::in);
	if(!in.is_open()) {
		std::cerr << "Failed to open input file \'" << in_path << "\'" << std::endl;
		return false;
	}

	// attempt to create output file
	std::ofstream out(out_path.c_str(), std::ios::out | std::ios::trunc);
	if(!out.is_open()) {
		std::cerr << "Failed to create output file \'" << out_path << "\'" << std::endl;
		return false;
	}

	// retrieve input file size (in bytes)
	in.seekg(NULL, std::ios::end);
	size = in.tellg();
	in.seekg(NULL);

	// add header to output file
	pos = out_path.find_last_of("/\\");
	format_header = out_path.substr(pos + 1);
	out << "/*\n * " << format_header << "\n * Automatically generated using f2hpp\n */\n\n";

	// add define to output file
	format_define = format_header;
	to_uppercase(format_define);
	out << "#ifndef " << format_define << std::endl << "#define " << format_define << std::endl << std::endl;

	// add class definition to output file
	pos = format_header.find_last_of(".");
	format_class = format_header.substr(0, pos);
	out << "class " << format_class << " {\npublic:\n\n";
	out << "\tunsigned long DATA_SIZE = " << size << ";\n";
	out << "\tunsigned char DATA[DATA_SIZE] = {\n\t\t";
	
	// iterate through data and append it to output file
	while(in) {
		if(count
				&& !(count % DIVIDER))
			out << "\n\t\t";
		in >> ch;
		out << "0x" << std::hex << (unsigned int) ch << ", ";
		++count;
	}

	// add end
	out << "\n\t};\n};\n\n#endif";
	out.close();
	in.close();
	return true;
}

/*
 * Returns command type
 */
int is_command(const std::string &str) {
	if(str == "-h")
		return HELP_CMD;
	else if(str == "-v")
		return VERSION_CMD;
	else if(str == "-f")
		return INPUT_CMD;
	else if(str == "-o")
		return OUTPUT_CMD;
	return NOT_CMD;
}

/*
 * Convert a string to uppercase
 */
void to_uppercase(std::string &str) {
	for(unsigned int i = 0; i < str.size(); ++i) {		
		if(str.at(i) >= 'a' && str.at(i) <= 'z')
			str.at(i) -= 32;
		else if(str.at(i) == '.')
			str.at(i) = '_';
	}
	str += '_';
}

/*
 * Main
 */
int main(int argc, char *argv[]) {
	int type;
	std::string in, out;
	std::vector<std::string> cmds;
	bool in_cmd = false, out_cmd = false;
	std::vector<std::string>::iterator cmd_iter;
	
	// print usage message
	if(argc <= 1) {
		std::cerr << USAGE << std::endl;
		return 1;
	}
	
	// aggregate commands in vector
	for(unsigned int i = 1; i < argc; ++i)
		cmds.push_back(std::string(argv[i]));

	// iterate through input
	for(cmd_iter = cmds.begin(); cmd_iter != cmds.end(); ++cmd_iter) {

		// parse as a command
		if(type = is_command(*cmd_iter)) {
			switch(type) {
				case HELP_CMD:
					std::cout << VERSION << std::endl << WARRANTY << std::endl << std::endl << HELP << std::endl << std::endl;
					return 1;		
				case VERSION_CMD:
					std::cout << VERSION << std::endl << WARRANTY << std::endl << std::endl;
					return 1;
				case INPUT_CMD:
					if(cmd_iter + 1 == cmds.end()) {
						std::cerr << "Missing operand following \'" << *cmd_iter << "\'" << std::endl << std::endl;
						return 1;
					}
					in_cmd = true;
					break;	
				case OUTPUT_CMD:
					if(cmd_iter + 1 == cmds.end()) {
						std::cerr << "Missing operand following \'" << *cmd_iter << "\'" << std::endl << std::endl;
						return 1;
					}
					out_cmd = true;
					break;	
				default: 
					std::cerr << "Unknown command: " << *cmd_iter << std::endl << std::endl;
					return 1;
			}		

		// parse as an argument
		} else {
			if(in_cmd) {
				in.assign(*cmd_iter);
				in_cmd = false;
			} else if(out_cmd) {
				out.assign(*cmd_iter);
				out_cmd = false;
			} else {
				std::cerr << "Operator \'" << *cmd_iter << "\' not associated with a command" << std::endl << std::endl;
				return 1;
			}
		} 
	}

	// generate file
	if(!gen_file(in, out))
		std::cerr << "Operation failed." << std::endl;
	else
		std::cout << "Operation successful." << std::endl;
	std::cout << std::endl;
	return 0;
}
