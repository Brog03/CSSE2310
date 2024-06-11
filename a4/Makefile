CFLAGS=-Wall -Wextra -pedantic -std=gnu99 -I/local/courses/csse2310/include
LIBRARIES= -I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a4 
CC=gcc

# Files used for uqimageclient
CLIENT_FILE = uqimageclient
CLIENT_C_FILES = uqimageclient.c connect.c error_client.c
CLIENT_O_FILES = uqimageclient.o connect.o error_client.o

# Files used for uqimageproc
PROC_FILE = uqimageproc
PROC_C_FILES = uqimageproc.c error_server.c server.c image.c
PROC_O_FILES = uqimageproc.o error_server.o server.o image.o

# Makes both uqimageclient and uqimageproc
all: client proc

# Cleans both uqimageclient and uqimageproc
clean: 
	rm -f $(PROC_FILE) $(CLIENT_FILE) *.o

clean-tests:
	rm -f *.stdout *.stderr

# Cleans, remakes both uqimageclient and uqimageproc for debugging
debug: clean
debug: CFLAGS += -g3
debug: all


#################
# Uqimageclient #
#################

# Makes just uqimageclient
client: $(CLIENT_FILE)

# Links all files for uqimageclient
$(CLIENT_FILE): $(CLIENT_O_FILES)
	$(CC) $(CFLAGS) $(LIBRARIES) $(CLIENT_O_FILES) -o $(CLIENT_FILE)

# Compiles all files for uqimageclient
$(CLIENT_O_FILES): 
	$(CC) $(CFLAGS) $(LIBRARIES) $(CLIENT_C_FILES) -c

# Cleans uqimageclient
clean-client:
	rm -f $(CLIENT_FILE) $(CLIENT_O_FILES)

# Cleans, and re-makes uqimageclient for debugging
debug-client: clean-client
debug-client: CFLAGS += -g3
debug-client: client


###############
# Uqimageproc #
###############

# Makes just uqimageproc
proc: CFLAGS += -pthread -lfreeimage -lcsse2310_freeimage
proc: $(PROC_FILE)

# Links all object files for uqimageproc
$(PROC_FILE): $(PROC_O_FILES)
	$(CC) $(CFLAGS) $(LIBRARIES) $(PROC_O_FILES) -o $(PROC_FILE)

# Compiles all c files for uqimageproc
$(PROC_O_FILES):
	$(CC) $(CFLAGS) $(LIBRARIES) $(PROC_C_FILES) -c

# Cleans uqimageproc
clean-proc:
	rm -f $(PROC_FILE) $(PROC_O_FILES)

# Cleans and remakes uqimageproc for debugging
debug-proc: clean-proc
debug-proc: CFLAGS += -g3
debug-proc: proc
