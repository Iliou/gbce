NAME	=	gbce

SRC	=	src/main.c \
		src/cpu.c \
		src/cartridge.c \
		src/ins.c \
		src/mapper.c

OBJ	=	$(SRC:.c=.o)

CFLAGS	=	-Wall -Wextra -std=gnu23

CPPFLAGS=	-iquote include

all: $(NAME)

release: CFLAGS += -O2
release: $(NAME)

debug: CFLAGS += -g3 -Wno-unused
debug: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
