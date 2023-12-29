CC = gcc

TARGET_EXEC ?= solver
BUILD_DIR ?= ./build
SRC_DIR ?= ./src

CFLAGS := -Ofast -mavx2 -march=native `pkg-config --cflags --libs gtk+-3.0 sdl2 SDL2_image cairo` -ffast-math -lSDL2 -fsanitize=address
LDFLAGS := -lm `pkg-config --cflags --libs gtk+-3.0 sdl2 SDL2_image cairo` -lSDL2 -fsanitize=address
SRCS := $(shell find $(SRC_DIR) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all : $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC) : $(OBJS)
	$(CC) $(OBJS) -o $@ ${LDFLAGS}

$(BUILD_DIR)/%.c.o : %.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ ${CFLAGS}


.PHONY: clean

clean:
	rm -f ${BUILD_DIR}/${TARGET_EXEC}
	rm -d -r ${BUILD_DIR}/src
