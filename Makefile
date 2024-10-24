CXX = g++
CXXFLAGS = -g -std=c++20 -Wall -Werror=vla -MMD
EXEC = cc3k
OBJECTS = defs.o entity.o character.o enemies.o item.o gamemap.o player.o hero.o main.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}