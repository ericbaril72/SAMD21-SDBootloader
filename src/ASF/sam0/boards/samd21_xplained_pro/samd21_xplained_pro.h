#ifndef SAMD21_XPLAINED_PRO_H_INCLUDED
#define SAMD21_XPLAINED_PRO_H_INCLUDED

#include <conf_board.h>
#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

void system_board_init(void);

/** Name string macro */
#define BOARD_NAME                "SAMD21_XPLAINED_PRO"

/** \name Resonator definitions
 *  @{ */
//#define BOARD_FREQ_SLCK_XTAL      (32768U)
//#define BOARD_FREQ_SLCK_BYPASS    (32768U)
//#define BOARD_FREQ_MAINCK_XTAL    0 /* Not Mounted */
//#define BOARD_FREQ_MAINCK_BYPASS  0 /* Not Mounted */
//#define BOARD_MCK                 CHIP_FREQ_CPU_MAX
//#define BOARD_OSC_STARTUP_US      15625
/** @} */


/** Number of on-board LEDs */
#define LED_COUNT                 1

/* name LED0 definitions */
#define LED0_PIN                  PIN_PA17
#define LED0_ACTIVE               true
#define LED0_INACTIVE             !LED0_ACTIVE

#define WINC1500CS				  PIN_PA06
#define WINC1500CS_INACTIVE		  true
#define WINC1500RST				  PIN_PA08
#define WINC1500RST_ACTIVE		  false
#define FT800CS				  PIN_PA06
#define FT800CS_INACTIVE		  true

#define LED_0_NAME                "LED0 (yellow)"
#define LED_0_PIN                 LED0_PIN
#define LED_0_ACTIVE              LED0_ACTIVE
#define LED_0_INACTIVE            LED0_INACTIVE
#define LED0_GPIO                 LED0_PIN
#define LED0                      LED0_PIN


/** \name SW0 definitions
 *  @{ */
#define BUTTON_COUNT 1

#define SW0_PIN                   PIN_PB02  
#define SW0_ACTIVE                false
#define SW0_INACTIVE              !SW0_ACTIVE
#define BUTTON_0_NAME             "SW0"
#define BUTTON_0_PIN              SW0_PIN
#define BUTTON_0_ACTIVE           SW0_ACTIVE
#define BUTTON_0_INACTIVE         SW0_INACTIVE


#define LED_Off(led_gpio)		port_pin_set_output_level(led_gpio,true)
#define LED_On(led_gpio)		port_pin_set_output_level(led_gpio,false)
#define LED_Toggle(led_gpio)	port_pin_toggle_output_level(led_gpio)


#ifdef __cplusplus
}
#endif

#endif  /* SAMD21_XPLAINED_PRO_H_INCLUDED */
