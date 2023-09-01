all: comet-vm

CC = clang

SRC_C = interpreter.c uleb128.c printer.c main.c 
SRC_H = interpreter.h uleb128.h opcodes.h printer.h

comet-vm: ${SRC_C} ${SRC_H}
	${CC} ${SRC_C}
	stat --format=%s a.out

format:
	clang-format -i ${SRC_C} ${SRC_H}

clean:
	rm -rf *~ a.out

diff: clean
	git difftool HEAD



