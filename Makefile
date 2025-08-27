NAME = Supervisor

OBJ_DIR = obj

SRCS = main.c \
		parser.c \
		parser_config.c \
		supervisor.c \
		process.c \
		process_init.c \
		process_init_utils.c \
		free_process.c \
		free_parser.c \
		logger.c \
		supervisor_ctrl.c \
		supervisor_ctrl_utils.c \
		supervisor_ctrl_cmds.c \
		supervisor_ctrl_reload.c \
		supervisor_ctrl_struct_reload.c \
		logrotate.c

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

CFLAGS = -g3 -Wall -Werror -Wextra

LDFLAGS = -lyaml

CC = gcc

all: build $(NAME)

build: 
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(sort $(dir $(OBJS)))

%.o: %.c
		@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ_DIR) $(OBJS)
		@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)
		@printf "Compil end!\n"

$(OBJ_DIR)/%.o: ./%.c
		@$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

clean:
		@rm -rf $(OBJ_DIR)
		@printf "clean good!\n"

fclean: clean
		@rm -rf $(NAME)
		@printf "fclean good!\n"

re: fclean all

.PHONY: all clean fclean re 
