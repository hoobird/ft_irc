CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -std=c++98

INCLUDESDIR = ./includes
SRCSDIR = ./srcs
BUILDDIR = ./build

ALLCMDSHEADER = $(INCLUDESDIR)/AllCommands.hpp

SRC = main.cpp Server.cpp Client.cpp NetworkManager.cpp DataStore.cpp \
	  Channel.cpp MessageParser.cpp MessageBuilder.cpp \
	  CommandHandler.cpp CommandBase.cpp CommandFactory.cpp \
	  CommandNICK.cpp CommandUSER.cpp CommandPASS.cpp CommandPING.cpp \
	  CommandPRIVMSG.cpp CommandMODE.cpp CommandJOIN.cpp CommandPART.cpp \
	  CommandTOPIC.cpp CommandKICK.cpp CommandINVITE.cpp CommandQUIT.cpp \
	  CommandNOTICE.cpp CTCPHandler.cpp \
	  Logger.cpp
DEP = $(OBJ:.o=.d)
OBJ = $(addprefix $(BUILDDIR)/, $(notdir $(SRC:.cpp=.o)))
NAME = ircserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(BUILDDIR)/%.o: $(SRCSDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDESDIR) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

valgrind: CXXFLAGS += -g
valgrind: re
	valgrind --log-file=valgrind_report.txt --show-leak-kinds=all --leak-check=full --track-fds=yes ./ircserv 8080 pass

-include $(DEP)

.PHONY: all clean fclean re
