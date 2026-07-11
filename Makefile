CFLAGS = -std=c++17 -O2

parallax: src/main.cpp
	g++ $(CFLAGS) -o parallax src/main.cpp


.PHONY: test clean

test: parallax
	./parallax

clean:
	rm -f parallax