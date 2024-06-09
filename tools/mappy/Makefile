APP := mappy

SRC := $(APP).c
LIB := -lglfw -lGLEW -lGL -lcglm
CFLAGS := -Wall -Wextra -Werror -std=c99 -ggdb3 -O0 -I../../include

default:
	gcc $(CFLAGS) $(SRC) -o $(APP) $(LIB)

clean:
	rm -rf $(APP) .cache/ compile_commands.json
