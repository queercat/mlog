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

	while (std::getline(*config_file, line)) {
		std::cout << line << std::endl;
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