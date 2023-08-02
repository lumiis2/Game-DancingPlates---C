ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: tp1.exe

tp1.exe: tp1.o
	gcc -o tp1.exe tp1.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

tp1.o: tp1.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c tp1.c

clean:
	del tp1.o
	del tp1.exe

