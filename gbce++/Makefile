NAME	=	gbce

SRC	=	src/main.cpp \
		src/Cpu.cpp \
		src/Cartridge.cpp \

OBJ	=	$(SRC:.cpp=.o)

CXXFLAGS=	-Wall -Wextra -std=c++23

CPPFLAGS=	-iquote include

all: $(NAME)

release: CXXFLAGS += -O2
release: $(NAME)

debug: CXXFLAGS += -g3
debug: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all release debug clean fclean re
