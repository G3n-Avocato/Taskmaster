NAME = Supervisor

OBJ_DIR = obj

SRCS = supervisor/main.c \
		parser_boot/parser.c \
		parser_boot/parser_config.c \
		supervisor/supervisor.c \
		process/process.c \
		process/process_init.c \
		process/process_init_utils.c \
		process/free_process.c \
		parser_boot/free_parser.c \
		supervisor/logger.c \
		supervisor_ctrl/supervisor_ctrl.c \
		supervisor_ctrl/supervisor_ctrl_utils.c \
		supervisor_ctrl/supervisor_ctrl_cmds.c \
		supervisor_ctrl_reload/supervisor_ctrl_reload.c \
		supervisor_ctrl_reload/supervisor_ctrl_struct_reload.c \
		supervisor_ctrl_reload/reload_parser.c \
		supervisor_ctrl_reload/reload_parser_config.c \
		supervisor/logrotate.c \
		parser_boot/signal_handler.c 

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
