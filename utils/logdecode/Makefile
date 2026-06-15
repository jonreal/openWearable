# logdecode - openWearable binary log -> CSV (host/board tool)
CC = cc
CFLAGS = -std=gnu99 -Wall -O2

# Define USE_HDF5 and link -lhdf5 to add an HDF5 writer (not built by default).
all: logdecode

logdecode: logdecode.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) logdecode
