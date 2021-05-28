all:
	g++ src/mlog.cpp include/generator.h include/post.h -o bin/mlog -std=c++17
	./bin/mlog

build:
	g++ src/mlog.cpp include/generator.h include/post.h -o bin/mlog -std=c++17

run:
	./bin/mlog
