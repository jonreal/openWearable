#ifndef _CONTROL_H_
#define _CONTROL_H_

/* structures ------------------------------------------------------------- */
typedef struct{

  int codeVersion;

  struct timeval start_time;
  float frq_hz;
  suseconds_t dt_us;

  FILE* f_log;

} config_t;

typedef struct{

  int control_loop_started;
  int FB_cntlr;
  int FF_cntlr;
} cntlr_flgs_t;

typedef struct{

  double time_stamp;

  float pos;
  float vel;
  float pos_0;

  float motor_cur;
  float motor_vel;

  float Kp;
  float Kd;
  float cmd;

  float FF_gain;
  float FF_period;
  float FF_phase;

} anklestate_t;

/* globals ---------------------------------------------------------------- */
extern const int debug;

/* prototypes ------------------------------------------------------------- */
int init_control(const float frq_hz, FILE* f);
int start_control(void);
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
int get_pos(volatile float *pos);
float encoder_to_angle(uint8_t MSB, uint8_t LSB);
int create_log_file(void);
float get_time_stamp(struct timeval t_now);
float get_time_interval(struct timeval t1, struct timeval t2);
suseconds_t get_time_interval_us(struct timeval t1, struct timeval t2);
void ui_menu_cb(void);
void ui_input(void);
int set_FB_cntrl(int is_FB_on);
int set_FF_cntrl(int is_FF_on);
int is_FF_on(void);
int is_FB_on(void);
#endif
