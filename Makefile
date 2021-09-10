CC = gcc
NAME = cws
CFLAGS = -Wall -I includes
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

all: $(NAME)

%.o: %.c
	@echo "\033[38;5;82m[\033[94m+\033[38;5;82m] Creation of the object \033[38;5;226m$< \033[32;5;85m -> \033[38;5;226m $@ \033[00m"
	@$(CC) $(CFLAGS) -o $@ -c $< -lmbedtls -lmbedcrypto -lmbedx509

$(NAME): $(OBJ)
	@echo "\033[38;5;82m[\033[94m+\033[38;5;82m] Compile \033[38;5;226m$< \033[32;5;85m -> \033[38;5;226m $@ \033[00m"
	@ar rc $(NAME).a $^
	@ranlib $(NAME).a

clean: $(OBJ)
	@echo "\033[38;5;82m[\033[94m+\033[38;5;82m] Cleaning \033[38;5;226m$^ \033[32;5;85m\033[00m"
	@rm $^