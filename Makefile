CFLAGS = -std=c++20 -O2 -Werror -pthread -Wmultichar
BUILDFOLDER = src/build

.PHONY: all test clean

all: parallax

$(BUILDFOLDER):
	mkdir -p $(BUILDFOLDER)

parallax: src/main.cpp | $(BUILDFOLDER)
	g++ $(CFLAGS) -o $(BUILDFOLDER)/parallax src/main.cpp

output: $(BUILDFOLDER)/out.asm | $(BUILDFOLDER)
	nasm -felf64 -g -Fdwarf $(BUILDFOLDER)/out.asm -o $(BUILDFOLDER)/out.o
	ld $(BUILDFOLDER)/out.o -o out

test: parallax
	./$(BUILDFOLDER)/parallax languageFile.px

clean:
	rm -rf $(BUILDFOLDER)
	rm -f parallax out

	