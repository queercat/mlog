#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

class Post {
	private:
		std::string title;

		std::string style_location;
		std::string header_location;
		std::string footer_location;

		std::string file_name;
		std::string post_name;

		std::vector<std::string> post_head;
		std::vector<std::string> post_body;
		std::vector<std::string> html_body;

		std::vector<std::string> keywords;

		void generate_file_name();
		void parse_file(std::fstream*);
		void generate_html();
		void write_file();

	public:
		int generate_post_from_file(std::fstream* post_file);
};

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
			}

			if (key == "style_location") {
				this->style_location = val;
			}
		}
	}

	// Insert the title.

	this->html_body.push_back("<h1>" + this->title + "</h1>");

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
}

void Post::write_file() {
	std::fstream file;

	file.open(fs::current_path().string() + "/TEST_OUTPUT.html", std::fstream::in | std::fstream::out | std::fstream::app);

	for (std::string &line : this->html_body) {
		file << line << "\n";
	}

	file.close();
}

int Post::generate_post_from_file(std::fstream* post_file) {
	this->parse_file(post_file);	
	this->generate_html();
	this->write_file();

	return 1;
}