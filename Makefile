SOURCES = main.c physics.c graphics.c utilities.c audio.c resources.c \
		  tick.c scenes.c object_init.c
VPATH = src

CFLAGS = -Wall -Werror

ALLEGRO_LIBS = allegro-5.0 allegro_main-5.0 allegro_image-5.0 \
			   allegro_font-5.0 allegro_ttf-5.0 allegro_memfile-5.0
PORTAUDIO_LIBS = portaudio-2.0
CFLAGS += `pkg-config --libs --cflags $(ALLEGRO_LIBS) $(PORTAUDIO_LIBS)`

newtonsapple: $(SOURCES)
# $^: prerequisite names
	$(CC) -o newtonsapple $^ $(CFLAGS)
