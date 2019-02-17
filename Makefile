CFLAGS = -std=gnu++11 -pedantic -Wall -Wextra -Weffc++ -Werror

.PHONY: debug

stlconvert: main.cpp
	g++ $(CFLAGS) main.cpp -o stlconvert

debug: main.cpp
	g++ $(CFLAGS) -g main.cpp -o stlconvert
