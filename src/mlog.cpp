#include <iostream>
#include <string>

#include "../include/generator.h"

/**
 * This is a (May's) static blog generator.
 * It parses a config file and then runs over the blog_post directory.
 * For each file found it generates a corresponding HTML file.
 * Those files are then written into a blog directory.
 **/

int main() {
	std::string blog_location = "blog/";
	Generator generator;

	generator.generate_blog(blog_location);

	return 0;
}