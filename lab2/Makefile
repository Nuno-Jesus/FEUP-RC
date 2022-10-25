CC = gcc
FLAGS = 
DEPS = .

TARGET = alarm.o app_layer.o file.o link_layer.o port.o state_machine.o utils.o
NAME = project

%.o : %.c 
	$(CC) $(FLAGS) -c $< -o $@ -I $(DEPS)

all: receiver transmitter

receiver: $(TARGET)
	$(CC) $(FLAGS) receiver.c -o $(NAME) $(TARGET) -I $(DEPS)

transmitter: $(TARGET)
	$(CC) $(FLAGS) transmitter.c -o $(NAME) $(TARGET) -I $(DEPS)

clean:
	rm -f $(TARGET)

fclean: clean
	rm -f $(NAME)

re: fclean all


	