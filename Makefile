NAME = Supervisor

SRCS = logger.cpp			\
	  	parser.cpp			\
		process.cpp			\
		process_utils.cpp	\
		supervisor.cpp		\
		main.cpp

OBJS = $(SRCS:.cpp=.o)

CC = c++

CFLAGS = -Wall -Wextra -Werror

YAML_CPP_DIR = /usr/local/include/yaml-cpp
YAML_CPP_LIB_DIR = /usr/local/lib

INCLUDES = -I$(YAML_CPP_DIR)/include
LIBS = -L$(YAML_CPP_LIB_DIR) -lyaml-cpp

all : $(NAME)

$(NAME): $(OBJS)
	$(CC)  $(OBJS) -o $(NAME) $(LIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all