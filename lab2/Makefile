CC = gcc
FLAGS = -Wall -Wextra -Werror
DEPS = .

TARGET = alarm.c protocol.c state_machine.c utils.c
NAME = project #Mudar depois

%.o : %.c 
	$(CC) $(FLAGS) -c $< -o $@ -I $(DEPS)

all: receiver transmitter

receiver:
	$(CC) $(FLAGS) receiver.c -o $(NAME) $(TARGET) -I $(DEPS)

transmitter: 
	$(CC) $(FLAGS) transmitter.c -o $(NAME) $(TARGET) -I $(DEPS)

clean:
	rm -f $(TARGET)

fclean: clean
	rm -f $(NAME)

re: fclean all
	