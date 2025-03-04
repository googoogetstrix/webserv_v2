NAME 					= webserv

CXX 					= c++

CXXFLAGS 				= -Wall -Wextra -Werror -std=c++98 -pedantic -g3 -O0

HEAD					= HttpRequest.hpp HttpResponse.hpp ServerConfig.hpp Webserv.hpp Logger.hpp Util.hpp \
						Connection.hpp ConnectionController.hpp 

SRC 					= HttpRequest.cpp HttpResponse.cpp ServerConfig.cpp Webserv.cpp Logger.cpp Util.cpp \
						Connection.cpp ConnectionController.cpp \
						main.cpp

OBJ 					= $(SRC:.cpp=.o)

all: 					$(NAME)

$(NAME): $(OBJ)
						@echo "\033[1m\033[32mcompiling...\033[0m"
						$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
						@echo "\033[1m\033[32m🖧🖧🖧  $(NAME) compiled.\033[0m"


%.o: %.cpp  			Makefile	$(HEAD)
						$(CXX) $(CXXFLAGS) -c  $< -o $@

clean:
						@echo "\033[1m\033[31mremoving object files \033[0m"
						rm -f $(OBJ)
fclean: 				clean
						@echo "\033[1m\033[31mremoving $(NAME) \033[0m"
						rm -f $(NAME)

re: 					fclean all

.PHONY: 				all clean fclean re
