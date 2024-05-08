NAME = ircserv
CXX = c++
# CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SRC = ./main.cpp Server.cpp EventHandler.cpp Client.cpp
OBJS = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $^

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re :
	$(MAKE) fclean
	$(MAKE) all

.PHONY : all bonus clean fclean re
