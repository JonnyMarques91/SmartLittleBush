
#include <asf.h>

/* M�scaras da Interrup��o */
#define TC			TC0
#define CHANNEL		0
#define ID_TC		ID_TC0
#define TC_Handler  TC0_Handler
#define TC_IRQn     TC0_IRQn

/* M�scaras PIO */

/* Prot�tipos de Sub-Rotinas e Fun��es */
void presence_cfg();
void presence_interrupt();

/* Declara��o de vari�veis Globais */


uint16_t uCNTseg = 0;	//Contador - 1 Segundo

/**
 *  Configure Timer Counter 0 to generate an interrupt every ...
 */
static void tc_config(uint32_t freq_desejada)
{
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t counts;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();
	
	pmc_enable_periph_clk(ID_TC);
	
	tc_find_mck_divisor( freq_desejada, ul_sysclk, &ul_div, &ul_tcclks,	BOARD_MCK);
	
	tc_init(TC, CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	
	counts = (ul_sysclk/ul_div)/freq_desejada;
	
	tc_write_rc(TC, CHANNEL, counts);

	NVIC_ClearPendingIRQ(TC_IRQn);
	NVIC_SetPriority(TC_IRQn, 15);
	NVIC_EnableIRQ(TC_IRQn);
	
	// Enable interrupts for this TC, and start the TC.
	tc_enable_interrupt(TC,	CHANNEL, TC_IER_CPCS);
	tc_start(TC, CHANNEL);
}


// Interrupt service routine
/* Rotina de Interrup��o - TC */
void TC_Handler(void)
{
//	uCNTseg++;
	tc_get_status(TC,CHANNEL);
	LED_Off(LED0_GPIO);
// 	if (uCNTseg>2)
// 	{
// 		uCNTseg = 0;
// 		pio_set_pin_low(LED0_GPIO);
// 		LED_Toggle(LED0_GPIO);
// 	}
	
}

int main (void)
{
	/* Chamada de rotinas de inicializa��o */
	sysclk_init();
	board_init();
	presence_cfg();

	/* Insert application code here, after the board has been initialized. */
	/* Gera Interrup��o ap�s T (s) */
//	tc_config(1);
	
	while(1)
	{
		
	}
}

/* Configura��o da Interrup��o - Sensor de Presen�a */
void presence_cfg()
{
	pio_set_input(PIOB, PIO_PB3, PIO_PULLUP | PIO_DEBOUNCE);
	pio_handler_set(PIOB, ID_PIOB, PIO_PB3, PIO_IT_RISE_EDGE, presence_interrupt);
	pio_enable_interrupt(PIOB, PIO_PB3);

	NVIC_SetPriority (PIOB_IRQn,14);
	NVIC_EnableIRQ(PIOB_IRQn);
}

/* A��o - Interrup��o do Sensor de Presen�a */
/*
	- Deve chamar rotinas de leitura de TODOS os sensores;
	- Enviar dados via Bluetooth para o celular do usu�rio;
*/
void presence_interrupt()
{
//	LED_Toggle(LED0_GPIO);	
	LED_On(LED0_GPIO);
	tc_config(0.1);
}