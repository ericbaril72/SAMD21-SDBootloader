/**
 * \file
 *
 * \brief SAM D20 Master SPI Bootloader
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */


#include <asf.h>
#include "conf_bootloader.h"
#include "memories_initialization.h"
#include <conf_clocks.h>

/** Size of the file to write/read. */
#define DATA_SIZE 512
//#define SERIAL_DEBUG 1
//#define SD_MMC_DEBUG 1

/* Function prototypes */
static void program_memory(uint32_t address, uint8_t *buffer, uint16_t len);
static void start_application(void);
static void check_boot_mode(void);
static void cdc_uart_init(void);

extern uint32_t __sketch_vectors_ptr;
struct usart_module cdc_uart_module;



/**
 * \brief Function for checking whether to enter boot mode or application mode
 *
 * This function will check the state of BOOT_LOAD_PIN. If it is pressed, it
 * continues execution in bootloader mode. Else, it reads the first location
 * from the application section and checks whether it is 0xFFFFFFFF. If yes,
 * then the application section is empty and it waits indefinitely there. If
 * not, it jumps to the application and starts execution from there.
 * Access to direct peripheral registers are made in this routine to enable
 * quick decision on application or bootloader mode.
 */
static void check_boot_mode(void)
{
	uint32_t app_check_address;
	uint32_t *app_check_address_ptr;
	volatile bool boot_en;	

	/* Check if WDT is locked */
	if (!(WDT->CTRL.reg & WDT_CTRL_ALWAYSON)) {
		/* Disable the Watchdog module */
		WDT->CTRL.reg &= ~WDT_CTRL_ENABLE;
	}

	/* Enable the input mode in Boot GPIO Pin  PB02 @ group1 */
	PORT->Group[0].DIRCLR.reg = (1U << PIN_PA04);
	PORT->Group[0].PINCFG[PIN_PB02].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	PORT->Group[0].OUTSET.reg = (1U << PIN_PA04);
	/* Read the BOOT_LOAD_PIN PA04 status */
	boot_en = (PORT->Group[0].IN.reg) & (1U << PIN_PA04);
	
	/*
	* Read the first location of application section which contains the address of stack pointer.
	* If it is 0xFFFFFFFF then the application section is empty.
	*/
	app_check_address = APP_START_ADDRESS;
	app_check_address_ptr = (uint32_t *) app_check_address;
	
	if(*app_check_address_ptr != 0xFFFFFFFF){	// If an app is Loaded ...
	//if(1){	// Bypass APP presence checking
				/* Check the BOOT pin  and is reset called by WDT */
		if ((PM->RCAUSE.reg & PM_RCAUSE_WDT)) {
			
			/* Pointer to the Application Section */
			void (*application_code_entry)(void);
			/* Rebase the Stack Pointer */
			__set_MSP(*(uint32_t *) APP_START_ADDRESS);							
			/* Rebase the vector table base address */
			SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);	
			/* Load the Reset Handler address of the application */
			application_code_entry = (void (*)(void))(unsigned *) (*(unsigned *)(APP_START_ADDRESS + 4));							
			/* Jump to user Reset Handler in the application */
			application_code_entry();											
		}
	}
}



/**
 * \brief Main application
 */
int main(void)
{
	uint32_t len = 0;
	uint32_t remaining_length = 0;
	uint32_t curr_prog_addr;
	uint32_t byte_read;
	uint8_t  buff[DATA_SIZE] = {0};
	struct nvm_config config;
	
	
	/* Check switch state to enter boot mode or application mode */
	check_boot_mode();
	
	/* Initialize system */
	system_init();
	
		
#ifdef SERIAL_DEBUG
	for (len=1;len<10;len++) {
		delay_ms(100);
		PORT->Group[0].OUTTGL.reg = (1<<PIN_PA17);
	}
#endif
	
	
	cdc_uart_init();
	printf("\f");
	printf("\n\r-------------------------------------\n\r");
	printf(    "--- Bootloader Rev 1.13 \n\r");
	printf(    "--- Compiled:  %s @ %s \n\r", __DATE__, __TIME__);
	printf(    "-------------------------------------\n\r");
	printf(    "Reset Cause: ");
	int rcause =PM->RCAUSE.reg;
	printf("%d  ",rcause);
	switch(rcause){
		case 1:
			printf("POWER ON\r\n");
			break;
		case 16:
			printf("Reset Button\r\n");
			break;
		case 32:
			printf("Watch Dog\r\n");
			break;
		case 64:
			printf("System Reset\r\n");
			break;
	}
		
	PORT->Group[0].DIRCLR.reg = (1U << PIN_PA02);
	PORT->Group[0].PINCFG[PIN_PA02].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	PORT->Group[0].OUTSET.reg = (1U << PIN_PA02);
	
	
	/* Intialize the SD memory device */
	memories_initialization();
	/* Get NVM default configuration and load the same */
	nvm_get_config_defaults(&config);
	config.manual_page_write = false;
	nvm_set_config(&config);

	/* File name to be Flashed */
	TCHAR file_name[16] = "0:MDL.bin";
	TCHAR bufile_name[16] = "0:fd.bin";
	
	/* Declare these as static to avoid stack usage.
	* They each contain an array of maximum sector size.
	*/
	static FATFS fs;
	static FIL file_object;
	volatile char factoryDefaultFile;
	
	/* Read the PIN PA04 to select Factory default file vs current file */
	factoryDefaultFile = (PORT->Group[0].IN.reg) & (1U << PIN_PA02);
	
	f_mount(0, &fs);
	
	int result;
	if (factoryDefaultFile) {
		result = f_open(&file_object, (char const *)file_name, FA_OPEN_EXISTING | FA_READ);
		printf(    "--> Updating Firmware to %s\n\r",file_name);
	}
	else {
		result = f_open(&file_object, (char const *)bufile_name, FA_OPEN_EXISTING | FA_READ);
		printf(    "--> Updating Firmware to %s\n\r",bufile_name);
	}
	
		
	/* Get the length to be programmed */
	len = file_object.fsize;
	remaining_length = len;
	
		
	if (result==0){		// File found
		curr_prog_addr = APP_START_ADDRESS;
		
#ifdef SERIAL_DEBUG		
		printf("\r\n\tRe-FLASHING at addr :  0x%06X\r\n",curr_prog_addr);
#endif	
		do {
			f_read(&file_object, buff, min(DATA_SIZE, len), &byte_read);	/* Read data of DATA_SIZE */
			program_memory(curr_prog_addr, buff, min(DATA_SIZE, len));		/* Program the read data into Flash */
			
			curr_prog_addr += min(DATA_SIZE, len);		/* Increment the current programming address */
			remaining_length -= min(DATA_SIZE, len);	/* Calculate remaining length */
			len = remaining_length;						/* Update the length to remaining length to be programmed */
			port_pin_toggle_output_level(BOOT_LED);	
		} while (len != 0);
		
		port_pin_set_output_level(BOOT_LED, true);
		f_close(&file_object);
		f_mount(0, NULL);
		
		
#ifdef SERIAL_DEBUG		
		printf("\r\n\t                 to :  0x%06X\r\n",curr_prog_addr);
		printf("\t                          %i Bytes\r\n",curr_prog_addr-APP_START_ADDRESS);
#endif

		printf(    "--> Done ! Restarting application ...\n\r");
		printf(    "-------------------------------------\n\r");
		//check_boot_mode();		// Seems to have some issues with pre-activated CLOCKS and jumping stright to app vs 
		start_application();		// Waiting for a WD reset
	}
	
	else {	// Print out File Open error
		
#ifdef SERIAL_DEBUG
		printf("file error: %i",result);
#endif
		f_close(&file_object);
		f_mount(0, NULL);
		
	}
	
	delay_ms(700);
	for(int i=3;i>0;i--){
		port_pin_set_output_level(BOOT_LED, true);
		delay_ms(100);
		port_pin_set_output_level(BOOT_LED, false);
		delay_ms(200);
	}
	start_application();
	while(1);

}



/**
 * \brief Function for programming data to Flash
 *
 * This function will check whether the data is greater than Flash page size.
 * If it is greater, it splits and writes pagewise.
 *
 * \param address address of the Flash page to be programmed
 * \param buffer  pointer to the buffer containing data to be programmed
 * \param len     length of the data to be programmed to Flash
 */
static void program_memory(uint32_t address, uint8_t *buffer, uint16_t len)
{
	/* Check if length is greater than Flash page size */
	if (len > NVMCTRL_PAGE_SIZE) {
		uint32_t offset = 0;
		while (len > NVMCTRL_PAGE_SIZE) {
			/* Check if it is first page of a row */
			if ((address & 0xFF) == 0) {
				/* Erase row */
				nvm_erase_row(address);
			}
			/* Write one page data to flash */
			
			nvm_write_buffer(address, buffer + offset, NVMCTRL_PAGE_SIZE);
			/* Increment the address to be programmed */
			address += NVMCTRL_PAGE_SIZE;
			/* Increment the offset of the buffer containing data */
			offset += NVMCTRL_PAGE_SIZE;
			/* Decrement the length */
			len -= NVMCTRL_PAGE_SIZE;
		}
		/* Check if there is data remaining to be programmed*/
		if (len > 0) {
			/* Write the data to flash */
			nvm_write_buffer(address, buffer + offset, len);
		}
	} else {
		/* Check if it is first page of a row) */
		if ((address & 0xFF) == 0) {
			/* Erase row */
			nvm_erase_row(address);
		}
		/* Write the data to flash */
		nvm_write_buffer(address, buffer, len);
	}
}



/**
 * \brief Function for starting application
 *
 * This function will configure the WDT module and enable it. The LED is
 * kept toggling till WDT reset occurs.
 */


static void start_application(void)
{
	struct wdt_conf wdt_config;
	wdt_get_config_defaults(&wdt_config);
	/* Set the required clock source and timeout period */
	//wdt_config.clock_source = GCLK_GENERATOR_4;
	//wdt_config.timeout_period = WDT_PERIOD_16384CLK;
	//wdt_config.timeout_period = WDT_PERIOD_8CLK;
	/* Initialize and enable the Watchdog with the user settings */
	wdt_set_config(&wdt_config);
	
	while(1){
		
		PORT->Group[0].OUTTGL.reg = (1<<17);
#ifdef SERIAL_DEBUG
		//printf(".");
		//delay_ms(200);
#endif	
		
	}
}


static void cdc_uart_init(void)
{
	struct usart_config usart_conf;
	usart_get_config_defaults(&usart_conf);
	
	usart_conf.mux_setting = USART_RX_3_TX_2_XCK_3;
	usart_conf.pinmux_pad0 = PINMUX_UNUSED;
	usart_conf.pinmux_pad1 = PINMUX_UNUSED;
	usart_conf.pinmux_pad2 = PINMUX_PA10C_SERCOM0_PAD2;		//  TX
	usart_conf.pinmux_pad3 = PINMUX_PA11C_SERCOM0_PAD3;		//	RX
	usart_conf.baudrate    = 115200;
	usart_conf.generator_source = GCLK_GENERATOR_0;
	stdio_serial_init(&cdc_uart_module, SERCOM0, &usart_conf);
	usart_enable(&cdc_uart_module);
}