
CC = cc

CLOG = ../lib
OUT = ./out

FLAGS = -Wall -Wextra

TESTS = $(addprefix $(OUT)/, $(basename $(wildcard ./*.c)))

all: $(TESTS)

$(OUT)/%: %.c | $(OUT)
	$(CC) $< -o $@ $(FLAGS) -I$(CLOG)

$(OUT):
	mkdir -p $@

.PHONY: clean
clean:
	rm -frv $(OUT)

