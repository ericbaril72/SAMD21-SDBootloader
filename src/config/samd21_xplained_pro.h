#ifndef SAMD21_XPLAINED_PRO_H_INCLUDED
#define SAMD21_XPLAINED_PRO_H_INCLUDED

#include <conf_board.h>
#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

void system_board_init(void);


#define LED0_PIN                  PIN_PA17

#define WINC1500CS				  PIN_PA06
#define WINC1500CS_INACTIVE		  true
#define WINC1500RST				  PIN_PA08
#define WINC1500RST_ACTIVE		  false
#define FT800CS				  PIN_PA06
#define FT800CS_INACTIVE		  true



#ifdef __cplusplus
}
#endif

#endif  /* SAMD21_XPLAINED_PRO_H_INCLUDED */
