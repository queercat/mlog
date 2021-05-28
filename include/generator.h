#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <tuple>
#include <ctime>

#include <assert.h>
#include <stdlib.h>

#include "post.h"

namespace fs = std::filesystem;

class Generator {
	private:
		std::string blog_location;
		std::string config_location;
		std::unordered_map<std::string, std::string> config;

		std::vector<Post> posts;

		std::fstream open_file(const std::string&);

		void read_config(std::fstream*, std::vector<std::string>*);
		void parse_config(std::vector<std::string>*);

		void generate_file_names(std::vector<std::string>*);
		void generate_page_from_post(const std::string&);
		void generate_posts_page();

		void generate_config();

	public:
		void generate_blog(const std::string&);

		std::string get_blog_location();
		std::unordered_map<std::string, std::string>* get_config();
};

std::string Generator::get_blog_location() {
	return this->blog_location;
}

std::unordered_map<std::string, std::string>* Generator::get_config() {
	return &(this->config);
}

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

	std::string valid_params[3] = {"{", ":", "}"};

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
	// TODO: Set this to use the config.
	std::string posts_location = this->blog_location + "posts/";

	for (const auto & entry : fs::directory_iterator(posts_location)) {
		file_names->push_back(entry.path().string());
	}
}

void Generator::generate_page_from_post(const std::string& post_location) {
	std::fstream post_file = this->open_file(post_location);
	Post post;

	Post* success = post.generate_post_from_file(&post_file, &(this->config));

	assert(success != NULL);

	this->posts.push_back(*success);

	post_file.close();
}

void Generator::generate_posts_page() {
	std::vector<std::string> html_body;

	std::string header;

	try {
		header = this->config.at("header_path");
	}

	catch (...) {
		header = "";
	}

	/* Generate the header if it exists */
	if (header != "") {
		std::fstream header_file;
		std::string header_path = fs::current_path().string() + "/" + this->config.at("include_directory") + header;

		header_file.open(header_path, std::fstream::in);

		std::string header_line;

		while(std::getline(header_file, header_line)) {
			html_body.push_back(header_line);
		}

		header_file.close();
	}

	/* Add the style */
	std::string style_path = this->config.at("blog_style_directory") + this->config.at("style_default");
	std::string style = "<link rel=\"stylesheet\" href=\"" + style_path + "\">";

	html_body.push_back(style);

	/* Add the title */
	std::string title = "<h1>posts</h1>";

	html_body.push_back(title);

	for (Post post : this->posts) {
		std::string post_name = post.get_post_name();
		std::string post_path = this->config.at("posts_directory") + post_name + ".html";
		std::string post_date = post.get_post_date();

		std::vector<std::string>* keywords_vector = post.get_post_keywords();
		std::string keywords = "--> {";

		std::string is_new;

		// Get the local time.
		time_t t = time(NULL);
		tm* time = localtime(&t);

		/* If you use a different time schema please implement that here */
		int day_now = time->tm_mday;
		int month_now = time->tm_mon + 1;
		int year_now = time->tm_year + 1900;

		std::string seperator = "_";

		int date_times[3];

		std::string post_date_tmp = post_date;

		int index = 0;
		while(post_date_tmp != "") {
			int sep_pos = post_date_tmp.find(seperator);

			date_times[index] = atoi(post_date_tmp.substr(0, sep_pos).c_str());
			post_date_tmp = post_date_tmp.substr(sep_pos + 1, post_date_tmp.length());

			++index;

			if (sep_pos < 0) {
				break;
			}
		}

		if ((year_now - date_times[2] == 0) && (month_now - date_times[0] == 0) && (day_now - date_times[1] <= 7)) {
			is_new = "*new*";
		}

		for (std::string &keyword : *keywords_vector) {
			keywords += keyword + ", ";
		}

		keywords = keywords.substr(0, keywords.length() - 2);
		keywords += "}";

		/* Technically non-optimal but such a small N that it really doesn't matter. */

		if (keywords_vector->empty()) {
			keywords = "";
		}

		std::string list_html = "<ul>- <a href=\"" + post_path + "\">" + post_name + "</a> <" + post_date + "> " + is_new + " " + keywords + "</ul>"; 
		html_body.push_back(list_html);
	}

	std::fstream posts_file;
	std::string posts_file_path = fs::current_path().string() + "/" + config.at("output_directory") + "posts.html";
	auto file_arguments = (std::fstream::in | std::fstream::out | std::fstream::app);

	posts_file.open(posts_file_path, file_arguments);

	for (std::string &html_line : html_body) {
		posts_file << html_line + "\n";
	}

	posts_file.close();
}

void Generator::generate_blog(const std::string& blog_location) {
	this->blog_location = blog_location;
	this->generate_config();

	assert(!this->config.empty());

	std::vector<std::string> file_names;

	this->generate_file_names(&file_names);

	assert(!file_names.empty());

	for (std::string file_name : file_names) {
		this->generate_page_from_post(file_name);
	}

	assert(!this->posts.empty());

	this->generate_posts_page();
}