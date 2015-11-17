#include <stdint.h>

#include "soc_AM33XX.h"
#include "hw_types.h"
#include "pwmdriver.h"

void pwmInit(void)
{
  /* T_tbclk = PWM_CLK/CLK_DIV
   *
   * For up-down-count mode
   *    T_pwm = 2 x TBPRD x T_tbclk
   *    F_pwm = 1 / T_pwm
   *
   * PWM resolution
   *    Res (%) = F_pwm/F_sysclkout X 100
   *    Res (bits) = log2(F_pwm/F_sysclkout) */

  /* TODO: make this a param */
  uint16_t pwm_prd = 10000;

  /* Disable Global enable interrupts */
  //CT_INTC.GER = 0;

  /**** PWM SS 2 registers *****/

  /* CLKCONFIG: ePWMCLK_EN = 0x1 - enable pwm clk
   *            eQEPCLKSTOP_REQ = 0x1 - disable qep clk
   *            eCAPCLKSTOP_REQ = 0x1 - disable cap clk */
  HWREG(SOC_PWMSS2_REGS + 0x8) = (0x1 << 8) | (0x1 << 5) | (0x1 << 1);

  /**** EPWM registers *****/

  /* CMPCTL: SHDWAMODE = 0x1 - no shadow mode */
  HWREGH(SOC_EPWM_2_REGS + 0xE) = (0x1 << 4);

  /* CMPA: compare reg */
  HWREGH(SOC_EPWM_2_REGS + 0x12) = pwm_prd;

  /* CMPAHR: high res compare reg */
  HWREGH(SOC_EPWM_2_REGS + 0x10) = 0x1;

  /* AQCTLA:  CAD = 0x1 - when cnt = compare (increasing) force EPWMxA high
   *          CAU = 0x2 - when cnt = compare (decreasing) force EPWMxA high */
  HWREGH(SOC_EPWM_2_REGS + 0x16) = (0x1 << 6) | (0x2 << 4);

  /* TBCTL: PRDLD = 0x1 - shadow prd off
   *        CTRMODE = 0x2 - up-down-cnt mode */
  HWREGH(SOC_EPWM_2_REGS) = (0x1 << 3) | 0x2;

  /* TBPRD: set period */
  HWREGH(SOC_EPWM_2_REGS + 0xA) = pwm_prd;

  /*ETCLR: clear interrupts */
  HWREGH(SOC_EPWM_2_REGS + 0x38) = 0x1;

  /*ETSEL: INTEN = 0x1 -  enable pwm interrupt */
  HWREGH(SOC_EPWM_2_REGS + 0x32) = (0x1 << 3);

  /* Globaly enable interrupts */
  //CT_INTC.GER = 1;

}

void pwmCleanUp(void)
{
  /* TODO: fix this */

  /* TBPRD: period = 0 */
  HWREGH(SOC_EPWM_2_REGS + 0xA) = 0x1;

  /* CMPA: compare reg */
  HWREGH(SOC_EPWM_2_REGS + 0x12) = 0x1;
}

void pwmSetCmpValue(uint16_t cmpvalue) 
{

  /* CMPA: compare register */
  HWREGH(SOC_EPWM_2_REGS + 0x12) = cmpvalue;
}

