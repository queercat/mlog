#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <assert.h>

class Generator {
	private:
		std::string blog_location;
		std::string config_location;

		std::fstream open_file(const std::string&);
		void read_config(std::fstream*, std::vector<std::string>*);

	public:
		void generate_blog(const std::string&);
};

std::fstream Generator::open_file(const std::string& file_location) {
	std::fstream file;

	file.open(file_location);

	return file;
}

/**
 * @desc read_config : reads a file pointer's arguments and puts it into an arguments vector.
 * See "doc/reference_config.mconfig" for an example.
 */
void Generator::read_config(std::fstream* config_file, std::vector<std::string>* arguments) {
	std::string line;

	std::string l_bracket = "{";
	std::string colon = ":";
	std::string r_bracket = "}";

	std::string valid_params[3] = {l_bracket, colon, r_bracket};

	while (std::getline(*config_file, line)) {
		// Check if the line is a valid instruction. Will contain [:].
		bool is_valid = true; 

		int positions[3]; // Same order as valid_params.

		for (int i = 0; i < 3; i++) {
			std::size_t found = line.find(valid_params[i]);

			if (found == std::string::npos) {
				is_valid = false;
				break;
			}

			positions[i] = (int)found;
		}

		if (is_valid) {
			std::cout << line.substr(positions[0] + 1, positions[2] - 1) << std::endl;
		}
	}
}

void Generator::generate_blog(const std::string& blog_location) {
	std::string config_location = blog_location + "config.mconfig";

	this->blog_location = blog_location;
	this->config_location = config_location;

	std::fstream config_file = this->open_file(config_location);

	assert(config_file.is_open());

	std::vector<std::string> arguments;

	this->read_config(&config_file, &arguments);
}