# compiler
CC = g++
# compiler flags
CFLAGS = -std=c++14 -Wall -pedantic -g
# libraries
LIBS = -lcrypto

# the build target executable:
TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CSFLAGS) -o $(TARGET) $(TARGET).cpp  $(LIBS)

clean:
	$(RM) $(TARGET)

run:
	./$(TARGET)
