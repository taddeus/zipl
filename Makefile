CC = clang
CFLAGS = -std=c11 -Wall -Wextra -g

.PHONY: all
all: zipl

%: %.c
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)
