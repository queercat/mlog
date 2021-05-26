#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
namespace fs = std::filesystem;

#include <assert.h>

class Generator {
	private:
		std::string blog_location;
		std::string config_location;
		std::unordered_map<std::string, std::string> config;

		std::fstream open_file(const std::string&);

		void read_config(std::fstream*, std::vector<std::string>*);
		void parse_config(std::vector<std::string>*);

		void generate_file_names(std::vector<std::string>*);

		void generate_config();

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
 * @note See "doc/reference_config.mconfig" for an example.
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
			arguments->push_back(line.substr(positions[0] + 1, positions[2] - 1));
		}
	}
}

/**
 * @desc parse_config : Reads the config vector into a umap.
 */
void Generator::parse_config(std::vector<std::string>* config_arguments) {
	std::string argument;

	while (!config_arguments->empty()) {
		argument = config_arguments->back();
		config_arguments->pop_back();

		// Split the string doing some substr funstuffs.
		int seperator_position = (int)argument.find(":");

		std::string key = argument.substr(0, seperator_position);
		std::string val = argument.substr(seperator_position + 1, argument.length());

		this->config[key] = val;
	}
}

void Generator::generate_config() {
	std::string config_location = this->blog_location + "config.mconfig";

	this->config_location = config_location;

	std::fstream config_file = this->open_file(config_location);

	assert(config_file.is_open());

	std::vector<std::string> config_arguments;

	this->read_config(&config_file, &config_arguments);

	config_file.close();

	assert(!config_arguments.empty());

	this->parse_config(&config_arguments);

	assert(config_arguments.empty());
}

void Generator::generate_file_names(std::vector<std::string>* file_names) {
	std::string posts_location = this->blog_location + "posts/";

	for (const auto & entry : fs::directory_iterator(posts_location)) {
		file_names->push_back(entry.path().string());
	}
}

void Generator::generate_blog(const std::string& blog_location) {
	this->blog_location = blog_location;
	this->generate_config();

	assert(!this->config.empty());

	std::vector<std::string> file_names;

	this->generate_file_names(&file_names);

	assert(!file_names.empty());
}