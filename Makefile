
##### DIRECTORIES

INC  = include
SRC  = source
BLD  = build





##### LIBRARIES

HDR  = ${INC}/sdliv.h
LIB  = -L/usr/lib
LIB += -lSDL2
LIB += -lSDL2_image
LIB += -lSDL2_ttf
LIB += -lpthread




##### COMPILER OPTIONS

CFLG = -Wall -std=c++17 -g
CINC = -I${INC}
COPT = ${CFLG} ${CINC}
CC   = g++-8 ${COPT}





##### BUILD OBJECTS 

OBJ  = ${BLD}/main.o
OBJ += ${BLD}/log.o
OBJ += ${BLD}/constants.o
OBJ += ${BLD}/App.o
OBJ += ${BLD}/App_OnEvent.o
OBJ += ${BLD}/Window.o
OBJ += ${BLD}/Element.o
OBJ += ${BLD}/Font.o
OBJ += ${BLD}/FileHandler.o

EXE  = sdliv





##### RECIPES

all: ${BLD} ${OBJ} ${EXE}

${BLD}:
	mkdir -p ${BLD}

clean:
	rm -f ${OBJ}
	rm -f ${EXE}
	rmdir ${BLD}




##### App OBJECT FILES

${BLD}/App.o: ${SRC}/App.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/App_OnEvent.o: ${SRC}/App_OnEvent.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/Window.o: ${SRC}/Window.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/Element.o: ${SRC}/Element.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/Font.o: ${SRC}/Font.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/FileHandler.o: ${SRC}/FileHandler.cpp ${HDR}
	${CC} -o $@ -c $<




##### MISC OBJECT FILES

${BLD}/constants.o: ${SRC}/constants.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/log.o: ${SRC}/log.cpp ${HDR}
	${CC} -o $@ -c $<

${BLD}/main.o: ${SRC}/main.cpp ${HDR}
	${CC} -o $@ -c $<





##### EXECUTABLE

${EXE}: ${OBJ}
	${CC} -o $@ $^ ${LIB}






