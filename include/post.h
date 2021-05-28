#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <iostream>

#include <assert.h>

namespace fs = std::filesystem;

class Post {
	private:
		std::string title;

		std::string output_location;
		std::string include_location;
		std::string style_location; // The location of the directory to the CSS (/styles/)
		std::string post_output_location;

		std::string header_location;
		std::string footer_location;

		std::string file_name;
		std::string post_name;
		std::string style_name; // The name of the CSS (style.css)

		std::vector<std::string> post_head;
		std::vector<std::string> post_body;
		std::vector<std::string> html_body;

		/* Meta information inside the post */
		std::vector<std::string> keywords;
		std::string post_date;

		void generate_file_name();
		void parse_file(std::fstream*);
		void generate_html();
		void write_file();

		void parse_config(std::unordered_map<std::string, std::string>*);

	public:
		Post* generate_post_from_file(std::fstream*, std::unordered_map<std::string, std::string>*);
		
		std::vector<std::string>* get_post_keywords();
		std::string get_post_date();
		std::string get_post_name();
};

std::vector<std::string>* Post::get_post_keywords() {
	return &(this->keywords);
}

std::string Post::get_post_date() {
	return this->post_date;
}

std::string Post::get_post_name() {
	return this->post_name;
}

void Post::generate_file_name() {
	this->file_name = this->post_name + ".html";
}

void Post::parse_file(std::fstream* post_file) {
	std::string buffer_line;

	bool is_post_body = false;
	bool is_post_head = true;

	while(std::getline(*post_file, buffer_line)) {
		if (buffer_line == "{BEGIN_POST}") {
			is_post_body = true;
			is_post_head = false;
			continue;
		}

		if (buffer_line == "{END_POST}") {
			is_post_body = false;
		}

		if (is_post_head) {
			this->post_head.push_back(buffer_line);
		}

		if (is_post_body) {
			this->post_body.push_back(buffer_line);
		}
	}

	assert(!post_body.empty());
}

void Post::generate_html() {
	std::string valid_params[3] = {"{", ":", "}"};
	for (std::string &line : this->post_head) {
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
			std::string argument = line.substr(positions[0] + 1, positions[2] - 1);
		
			int seperator_position = (int)argument.find(":");

			std::string key = argument.substr(0, seperator_position);
			std::string val = argument.substr(seperator_position + 1, argument.length());

			/* BEGIN UGLY ENUM FOR SPECIFIC BEHAVIORS */
			if (key == "keywords") {
				while(!val.empty()) {
					int kword_position = (int)val.find(",");

					this->keywords.push_back(val.substr(0, kword_position));

					if (kword_position < 0) {
						break;
					}

					val = val.substr(kword_position + 1, val.length());
				}
			}

			if (key == "title") {
				this->title = val;
			}

			if (key == "post_name") {
				this->post_name = val;
				this->generate_file_name();

				assert(this->file_name != "");
			}

			if (key == "date") {
				this->post_date = val;
			}

			if (key == "style_name") {
				this->style_name = val;

				// As this is already set, if one isn't found it defaults to the one from our config.
				assert(this->style_name != "");
			}
		}
	}

	/* Adding header if it exists */
	if (this->header_location != "") {
		std::fstream header;
		std::string header_path = fs::current_path().string() + "/" + this->include_location + this->header_location;

		header.open(header_path, std::fstream::in);

		std::string header_line;

		while(std::getline(header, header_line)) {
			this->html_body.push_back(header_line);
		}

		header.close();
	}

	/* Adding some style! */
	std::string style = "<link rel=\"stylesheet\" href=\"" + style_location + style_name + "\">";

	this->html_body.push_back(style);

	/* Adding the title! */
	std::string title = "<h2>" + this->title + "</h2>";

	this->html_body.push_back(title);

	for (std::string &line : this->post_body) {
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

		// lowers the entire string.
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);

		if (is_valid) {
			std::string argument = line.substr(positions[0] + 1, positions[2] - 1);
		
			int seperator_position = (int)argument.find(":");

			std::string key = argument.substr(0, seperator_position);
			std::string val = argument.substr(seperator_position + 1, argument.length());

			/* BEGIN UGLY ENUM FOR SPECIFIC BEHAVIORS */

			if (key == "img") {
				this->html_body.push_back("<img src=\"" + val + "\"></img>");
			}

			else if (key == "header") {
				this->html_body.push_back("<h2>" + val + "</h2>");
			}

			else if (key == "element") {
				if (val == "---") {
					this->html_body.push_back("<p>------------</p>");
				}
			}
		} 

		else if (line == "") {
			this->html_body.push_back("<br>");
		}

		else {
			this->html_body.push_back("<p>" + line + "</p>");
		}
	}

	/* Adding footer if it exists */
	if (this->footer_location != "") {
		std::fstream footer;
		std::string footer_path = this->include_location + this->footer_location;

		footer.open(footer_path, std::fstream::in);

		std::string footer_line;

		while(std::getline(footer, footer_line)) {
			this->html_body.push_back(footer_line);
		}

		footer.close();
	}	
}

void Post::write_file() {
	std::fstream file;

	std::string output_path = fs::current_path().string() + "/" + this->output_location + this->post_output_location + this->file_name;
	auto open_arguments = (std::fstream::in | std::fstream::out | std::fstream::trunc);

	std::cout << output_path << std::endl;

	file.open(output_path, open_arguments);

	for (std::string &line : this->html_body) {
		file << line << "\n";
	}

	file.close();
}

void Post::parse_config(std::unordered_map<std::string, std::string>* config) {
	this->include_location = config->at("include_directory");
	this->output_location = config->at("output_directory");
	this->post_output_location = config->at("posts_directory");
	this->style_location = config->at("style_directory");

	this->style_name = config->at("style_default");

	std::string tmp_header_location;
	std::string tmp_footer_location;

	try {
		tmp_header_location = config->at("header_path");
	}

	catch (...) {
		tmp_header_location = "";
	}

	this->header_location = tmp_header_location;

	try {
		tmp_footer_location = config->at("footer_path");
	}

	catch (...) {
		tmp_footer_location = "";
	}

	this->footer_location = tmp_footer_location;
}

Post* Post::generate_post_from_file(std::fstream* post_file, std::unordered_map<std::string, std::string>* config) {
	assert(config != NULL);

	this->parse_config(config);

	assert(this->include_location != "");

	this->parse_file(post_file);
	this->generate_html();
	this->write_file();

	return this;
}