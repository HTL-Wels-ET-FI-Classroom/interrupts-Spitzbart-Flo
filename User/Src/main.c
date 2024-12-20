/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static volatile int cntPress=0;
static volatile int farbe=0;
static volatile int time1=0;
static volatile int time2=0;
/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
	if((cntPress%2)==0){
		time1++;
	}else if((cntPress%2)==1){
		time2++;
	}
}
void EXTI0_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	cntPress++;
}
void EXTI3_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
	farbe++;
	if(farbe==3){
		farbe=0;
	}
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();

	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    HTL Wels");
	// printf Alternative
	LCD_SetPrintPosition(1, 0);
	printf(" Fischergasse 30");
	LCD_SetPrintPosition(2, 0);
	printf("   A-4600 Wels");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "Florian Spitzbart", CENTER_MODE);

	GPIO_InitTypeDef userButton;
	userButton.Alternate=0;
	userButton.Mode=GPIO_MODE_IT_RISING;
	userButton.Pull=GPIO_NOPULL;
	userButton.Pin=GPIO_PIN_0;
	userButton.Speed=GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOA, &userButton);

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	GPIO_InitTypeDef quelle;
	quelle.Alternate=0;
	quelle.Mode=GPIO_MODE_IT_RISING;
	quelle.Pull=GPIO_PULLUP;
	quelle.Pin=GPIO_PIN_3;
	quelle.Speed=GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOG, &quelle);

	HAL_NVIC_EnableIRQ(EXTI3_IRQn);






	int colour[3]={LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_GREEN};
	/* Infinite loop */
	while (1)
	{
		//execute main loop every 100ms
		//HAL_Delay(100);


		// display timer
		LCD_SetFont(&Font20);
		LCD_SetPrintPosition(5, 0);
		LCD_SetTextColor(colour[farbe]);
		printf("   Timer1: %.2f", time1/1000.0);
		LCD_SetPrintPosition(7, 0);
		printf("   Timer2: %.2f", time2/1000.0);


	}
}

/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if user button input (PA0) is high
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}

/**
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState (int* xCoord, int* yCoord) {
	void    BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
			*xCoord = TS_Calibration_GetX(*xCoord);
			*yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}


