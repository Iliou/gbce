NAME	=	gbce

CFLAGS	=	-Wall -Wextra

all: $(NAME)

$(NAME):

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
