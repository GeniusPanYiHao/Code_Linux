FLAG=-I ~/third_part/include -std=c++11 -ljsoncpp

.PHONY:all
all:test searchTest libsearcher.so
test:test.cpp search.cpp
		g++ $^ -o $@ $(FLAG)
searchTest:searchTest.cpp search.cpp
		g++ $^ -o $@ $(FLAG)
libsearcher.so:search.cpp search.h
	g++ search.cpp -o libsearcher.so -shared -fPIC $(FLAG)
.PHONY:clean
clean:
		rm test searchTest libsearcher.so

