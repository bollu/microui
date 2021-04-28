.PHONY: docs
all: renderer.out docs

docs:
	docco microui-header.h microui-source.c


renderer.out:
	gcc main.c renderer.c ./microui-source.c \
      -I. -Wall -std=c11 -pedantic `sdl2-config --libs` -lGL -lm -O3 -g -o renderer.out
