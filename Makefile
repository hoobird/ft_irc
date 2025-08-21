CC = c++
CFLAGS = -Wall -Wextra -Werror -MMD
VFLAG = -std=c++98

INCLUDESDIR = ./includes
SRCSDIR = ./srcs
BUILDDIR = ./build

SRC = main.cpp Server.cpp Client.cpp NetworkManager.cpp DataStore.cpp \
	  Channel.cpp MessageParser.cpp MessageBuilder.cpp 
DEP = $(OBJ:.o=.d)
OBJ = $(addprefix $(BUILDDIR)/, $(notdir $(SRC:.cpp=.o)))
NAME = ircserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(VFLAG) $(OBJ) -o $(NAME)

$(BUILDDIR)/%.o: $(SRCSDIR)/%.cpp | $(BUILDDIR)
	$(CC) $(CFLAGS) $(VFLAG) -I$(INCLUDESDIR) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

valgrind: CFLAGS += -g
valgrind: re
	valgrind --show-leak-kinds=all --leak-check=full --track-fds=yes ./ircserv 8080 pass

-include $(DEP)

.PHONY: all clean fclean re
