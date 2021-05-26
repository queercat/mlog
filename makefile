all:
	g++ src/mlog.cpp include/generator.h -o bin/mlog
	./bin/mlog

build:
	g++ src/mlog.cpp -o bin/mlog

run:
	./bin/mlog