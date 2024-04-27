#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "msg.h"


#include "periph/gpio.h"
#include "xtimer.h"
#include "thread.h"
#include "periph/adc.h"
#include <inttypes.h>

#include "cpu.h"
#include "board.h"
#include "periph/pwm.h"
#include "servo.h"

//servo
#define DEV         PWM_DEV(0)
#define CHANNEL     0
#define SERVO_MIN        (1000U)
#define SERVO_MAX        (2000U)



int main(void){
//servo
static servo_t servo;
 servo_init(&servo, DEV, CHANNEL, SERVO_MIN, SERVO_MAX);

 servo_set(&servo, SERVO_MAX); 
 xtimer_sleep(2);
 servo_set(&servo, SERVO_MIN);
xtimer_sleep(2);
servo_set(&servo, SERVO_MAX); 
 xtimer_sleep(2);
 servo_set(&servo, SERVO_MIN);
xtimer_sleep(2);
servo_set(&servo, SERVO_MAX); 
 xtimer_sleep(2);
 servo_set(&servo, SERVO_MIN);
xtimer_sleep(2);
servo_set(&servo, SERVO_MAX); 
 xtimer_sleep(2);
 servo_set(&servo, SERVO_MIN);
xtimer_sleep(2);
	
return 0;
}
