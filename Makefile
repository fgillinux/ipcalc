CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = ipcalc
SRC = ipcalc.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

dist:
	tar -czvf ipcalc.tar.gz $(SRC) Makefile
