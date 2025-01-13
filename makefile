
COMPILER = clang
CFILES = src/*.c
SOURCE_LIBS = -Ilib/
MACOS_OUT = -o "bin/raylib-pong"
MACOS_OPT =  -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a

default:
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) $(MACOS_OUT) $(MACOS_OPT)
