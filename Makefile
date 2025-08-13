CC = c++
CFLAGS = -Wall -Wextra -Werror -MMD -g
# CFLAGS = -MMD -g
VFLAG = -std=c++98

INCLUDESDIR = ./includes
SRCSDIR = ./srcs
BUILDDIR = ./build

SRC = main.cpp
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

-include $(DEP)

.PHONY: all clean fclean re
