
##### DIRECTORIES

INC  = include
SRC  = source
BLD  = build





##### LIBRARIES

HDR  = ${INC}/sdliv.h
LIB  = -L/usr/lib
LIB += -lpthread
LIB += -lSDL2
LIB += -lSDL2_image
LIB += -lSDL2_ttf




##### COMPILER OPTIONS

CFLG = -Wall -std=c++11 -g
CINC = -I${INC}
COPT = ${CFLG} ${CINC}
CC   = g++ ${COPT}





##### BUILD OBJECTS 

OBJ   = ${BLD}/main.o
OBJ  += ${BLD}/log.o
OBJ  += ${BLD}/constants.o
OBJ  += ${BLD}/Window.o
OBJ  += ${BLD}/Element.o
OBJ  += ${BLD}/Font.o
EXE   = sdliv





##### RECIPES

all: ${BLD} ${OBJ} ${EXE}

${BLD}:
	mkdir -p ${BLD}

clean:
	rm -f ${OBJ}
	rm -f ${EXE}
	rmdir ${BLD}




##### App OBJECT FILES

${BLD}/Window.o: ${SRC}/Window.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/Element.o: ${SRC}/Element.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/Font.o: ${SRC}/Font.cpp ${HDR}
	${CC} -o $@ -c $<





##### MISC OBJECT FILES

${BLD}/constants.o: ${SRC}/constants.cpp ${HDR}
	${CC}/ -o $@ -c $<

${BLD}/log.o: ${SRC}/log.cpp ${HDR}
	${CC}/ -o $@ -c $<

${BLD}/main.o: ${SRC}/main.cpp ${HDR}
	${CC} -o $@ -c $<





##### EXECUTABLE

${EXE}: ${OBJ}
	${CC} -o $@ $^ ${LIB}





