CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD

BASEEDIR = ./base
BOTSDIR = ./bots
BUILDDIR = ./build
BINDIR = ./bin

BOTBASE_SRC = $(BASEEDIR)/BotBase.cpp
BOTBASE_OBJ = $(BUILDDIR)/BotBase.o
BOTBASE_HEADER = $(BASEEDIR)/BotBase.hpp
BOTBASE_DEP = $(BOTBASE_OBJ:.o=.d)

# List of available bots (Update here once new bots are added)
BOT_SRC_NAMES = BotHuhCat.cpp
BOTS = $(BOT_SRC_NAMES:.cpp=)
BOT_SRCS = $(addprefix $(BOTSDIR)/, $(BOT_SRC_NAMES))
BOT_OBJS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(BOTS)))
BOT_DEPS = $(BOT_OBJS:.o=.d)
BOT_BINS = $(addprefix $(BINDIR)/, $(BOTS))

all: $(BOT_BINS)

# Create build and bin directory
$(BUILDDIR) $(BINDIR):
	@mkdir -p $@

# Pattern rule for building bot executables
$(BINDIR)/%: $(BUILDDIR)/%.o $(BOTBASE_OBJ) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(BASEEDIR) -o $@ $< $(BOTBASE_OBJ)

# Pattern rule for building bot object files
$(BUILDDIR)/%.o: $(BOTSDIR)/%.cpp $(BOTBASE_HEADER) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(BASEEDIR) -c $< -o $@

# Build IRC bot base class object file
$(BOTBASE_OBJ): $(BOTBASE_SRC) $(BOTBASE_HEADER) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(BASEEDIR) -c $< -o $@

-include $(BOTBASE_DEP) $(BOT_DEPS)

clean:
	rm -rf $(BUILDDIR)

fclean: clean
	rm -rf $(BINDIR)

re : fclean all

list-bots:
	@echo "Available bots:"
	@for bot in $(BOTS); do \
		echo "  $$bot"; \
	done

help:
	@echo "Available targets:"
	@echo "  all         - Build all bots"
	@echo "  clean       - Remove build and bin directories"
	@echo "  list-bots   - List all available bots"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "To build a specific bot: make $(BINDIR)/<botname>"
	@echo "Or use convenience target: make <botname>"
	@echo ""
	@echo "Available bots: $(BOTS)"
	@echo ""
	@echo "Directory structure:"
	@echo "  Base files: $(BASEEDIR)/"
	@echo "  Bot files:  $(BOTSDIR)/"
	@echo "  Build files: $(BUILDDIR)/"
	@echo "  Binaries:   $(BINDIR)/"

.PHONY: all clean help list-bots

huhcat: $(BINDIR)/BotHuhCat
