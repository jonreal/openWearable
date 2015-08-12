#ifndef _CONTROL_H_
#define _CONTROL_H_

/* structures ------------------------------------------------------------- */
typedef struct{
  int codeVersion;
  float frq_hz;
  suseconds_t dt_us;
  FILE* f_log;
} config_t;

typedef struct{
  int numOfFuncs;
  int (*func[5])(void);
} cleanup_t;

typedef struct{
  int control_loop_started;
  int FB_cntlr;
  int FF_cntlr;
} cntlr_flgs_t;

typedef struct{
  struct timeval start_time;
  struct timeval t_now;
  struct timeval t_end;
  double time_stamp;
  float cpu_time;
  float pos;
  float vel;
  float pos_0;
  float Kp;
  float Kd;
  float cmd;
  float FF_gain;
  float FF_period;
  float FF_phase;
  int adc_value[7];
} anklestate_t;

/* globals ---------------------------------------------------------------- */
extern int debug;

/* prototypes ------------------------------------------------------------- */
int init_control(const float frq_hz, FILE* f);
int start_control(void);
void add_func_to_cleanup(int (*FuncPtr)(void));
void control_cleanup(int signum);
void control_loop_cb(int signum);
int update_state(void);
int update_control(void);
int set_pos_0(float);
int set_Kp(float);
int set_Kd(float);
float get_Kp(void);
float get_Kd(void);
float get_pos_0(void);
int read_pos(void);
void log_data(int signum);
float encoder_to_angle(uint8_t MSB, uint8_t LSB);
int create_log_file(void);
void set_start_time(void);
void set_time_stamp(void);
void set_cpu_time(void);
float get_time_stamp(struct timeval t_now);
float get_time_interval(struct timeval *t1, struct timeval *t2);
int set_FB_cntrl(int is_FB_on);
int set_FF_cntrl(int is_FF_on);
int is_FF_on(void);
int is_FB_on(void);
#endif
