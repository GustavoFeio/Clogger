
CC = cc

CLOG = ../src

FLAGS = -Wall -Wextra

TESTS = $(basename $(wildcard ./*.c))
FILES = $(addsuffix .c, $(TESTS))

OUT = ./out

all: $(TESTS)

%: %.c $(OUT)
	$(CC) $< -o $(OUT)/$@ $(FLAGS) -I$(CLOG)

$(OUT):
	mkdir $@

clean:
	rm -frv $(OUT)

