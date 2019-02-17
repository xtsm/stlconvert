CFLAGS = -std=gnu++11 -pedantic -Wall -Wextra -Weffc++ -Werror

stlconvert: main.cpp
	g++ $(CFLAGS) main.cpp -o stlconvert
