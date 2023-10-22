
CFLAGS = $(shell sdl-config --cflags) -Wall -pedantic -ggdb
CXXFLAGS = $(CFLAGS)
LIBS = -lSDL_image $(shell sdl-config --libs)
OBJECTS = SFont.o

all: startmessage test1 test2 test3 SFontViewer endmessage

test1: $(OBJECTS) test1.o
	gcc $(CFLAGS) -o SFontTest1 $(OBJECTS) test1.o $(LIBS)

test2: $(OBJECTS) test2.o
	gcc $(CFLAGS) -o SFontTest2 $(OBJECTS) test2.o $(LIBS)

test3: $(OBJECTS) test3.o
	gcc $(CFLAGS) -o SFontTest3 $(OBJECTS) test3.o $(LIBS)

SFontViewer: $(OBJECTS) SFontViewer.o
	gcc $(CFLAGS) -o SFontViewer $(OBJECTS) SFontViewer.o $(LIBS)

clean:
	@rm -f *.o SFontTest* SFontViewer

startmessage:
	@echo ""
	@echo "These examples require libSDL_image. If you have any problems"
	@echo "or questions, please let me know. <karlb@gmx.net>"
	@echo ""

endmessage:
	@echo ""
	@echo "Thanks for trying SFont!"
	@echo ""
