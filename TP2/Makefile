#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ COLORS _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
RESET 	= \033[0m
BLACK 	= \033[1;30m
RED 	= \033[1;31m
GREEN 	= \033[1;32m
YELLOW 	= \033[1;33m
BLUE 	= \033[1;34m
PURPLE 	= \033[1;35m
CYAN 	= \033[1;36m
WHITE 	= \033[1;37m

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ COMMANDS _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
CC = cc
RM = rm -f

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ FLAGS _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
CFLAGS = -Wall -Wextra -Werror
MKFLAGS = --no-print-directory

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ FOLDERS _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
DEPS = .

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ FILES _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
TARGET = utils.o entity.o str.o connection.o
NAME = download
MAIN = main.c

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ RULES _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
all: $(NAME)

$(NAME): $(TARGET)
	$(CC) $(CFLAGS) $(MAIN) $(TARGET) -o $(NAME) -I $(DEPS)
	echo "[$(CYAN) Linked $(RESET)] $(GREEN)$(MAIN)$(RESET) with *.o"

debug: fclean $(TARGET)
	$(CC) $(CFLAGS) $(MAIN) $(TARGET) -o $(NAME) -I $(DEPS) -D DEBUG
	echo "[$(CYAN) Linked $(RESET)] $(GREEN)$(MAIN)$(RESET) with $(GREEN)$(TARGET)$(RESET)"

%.o : %.c 
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DEPS)
	echo "[$(CYAN)Compiled$(RESET)] $(CFLAGS) $(GREEN)$<$(RESET)"

clean:
	$(RM) $(TARGET)

fclean: clean
	$(RM) $(NAME) 

re: fclean all

.SILENT:


	