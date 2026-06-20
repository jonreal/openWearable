#ifndef _R5F_H_
#define _R5F_H_
// R5F runtime loader -- mirrors PruInit/board.c for the R5F pair. The cores boot
// the park firmware (DT firmware-name); R5fInit stops it (graceful handshake) and
// loads the OW control firmware "j7-r5f{0,1}-<app>-fw.ow" at runtime, then starts
// it -- exactly how PruInit loads the PRUs.
extern const char* const rp_r5f0;   // /dev/remoteproc node path, defined in board.c
extern const char* const rp_r5f1;
int R5fInit(char* suffix);          // stop park -> load control fw -> start (both cores)
int R5fCleanup(void);               // stop both R5F cores
#endif
