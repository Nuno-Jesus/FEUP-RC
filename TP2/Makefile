CC = gcc
FLAGS = 
DEPS = .

TARGET = 
NAME = download


all: $(NAME)

$(NAME): $(TARGET)
	$(CC) $(FLAGS) main.c $(TARGET) -o $(NAME) -I $(DEPS)

%.o : %.c 
	$(CC) $(FLAGS) -c $< -o $@ -I $(DEPS)

clean:
	rm -f $(TARGET)

fclean: clean
	rm -f $(NAME) 

re: fclean all


	