#ifndef _TUI_H__
#define _TUI_H_


typedef struct{
  volatile sig_atomic_t io_ready;
  char input_char[2];
  char input_float[10];
} tui_t;


/* prototypes ------------------------------------------------------------- */
int init_tui(void);
void io_cb(int sig);
void tui_menu(void);
int start_tui(void);
int tui_cleanup(void);
#endif
