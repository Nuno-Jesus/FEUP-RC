CC = gcc
FLAGS = 
DEPS = .

TARGET = alarm.o app_layer.o file.o link_layer.o port.o state_machine.o utils.o
EXEC1 = receiver
EXEC2 = transmitter

%.o : %.c 
	$(CC) $(FLAGS) -c $< -o $@ -I $(DEPS)

all: receiver transmitter clean

receiver: $(TARGET)
	$(CC) $(FLAGS) receiver.c -o $(EXEC1) $(TARGET) -I $(DEPS)

transmitter: $(TARGET)
	$(CC) $(FLAGS) transmitter.c -o $(EXEC2) $(TARGET) -I $(DEPS)

clean:
	rm -f $(TARGET)

fclean: clean
	rm -f $(EXEC1) $(EXEC2)

re: fclean all


	