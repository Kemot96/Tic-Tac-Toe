/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2019 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"

/* USER CODE BEGIN Includes */
#include "SSD1331.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
volatile uint16_t Joystick[2];
uint8_t field[9];
uint8_t i;
uint8_t player = 0;
uint8_t win = 0;
uint8_t draw = 0;
uint8_t cursor = 0;
uint8_t move = 0;
uint8_t error = 0;
uint8_t scoreX = 0;
uint8_t scoreO = 0;
uint8_t Received;
uint8_t buffer[20];
uint8_t data[300]; // Tablica przechowujaca wysylana wiadomosc.
uint16_t size = 0; // Rozmiar wysylanej wiadomosci
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM16_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

static void drawOnScreen(bool gameInProgress) {
	uint16_t size = 0;
	char fieldChar[9];
	char currentPlayer;

	for (uint8_t i = 0; i < 9; i++) {
		if (field[i] == 0) {
			fieldChar[i] = 'X';
		} else if (field[i] == 1) {
			fieldChar[i] = 'O';
		} else if (field[i] == 2) {
			fieldChar[i] = ' ';
		} else {
			fieldChar[i] = ' ';
		}
	}

	if (player == 0 && gameInProgress == false) {
		currentPlayer = 'X';
	} else if (player == 1 && gameInProgress == false) {
		currentPlayer = 'O';
	} else if (player == 1 && gameInProgress == true) {
		currentPlayer = 'X';
	} else if (player == 0 && gameInProgress == true) {
		currentPlayer = 'O';
	}

	if (win == 1 && player == 0) {
		size =
				sprintf(data,
						"\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\n\r\x1b[31mGracz X wygral!\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (win == 1 && player == 1) {
		size =
				sprintf(data,
						"\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\n\r\x1b[32mGracz O wygral!\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (draw == 1) {
		size =
				sprintf(data,
						"\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\n\r\x1b[34mRemis!\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 0) {
		size =
				sprintf(data,
						"\n\r\x1b[31m%c|\x1b[0m%c|%c\n\r\x1b[31m-+\x1b[0m-+-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[31m\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 0) {
		size =
				sprintf(data,
						"\n\r\x1b[32m%c|\x1b[0m%c|%c\n\r\x1b[32m-+\x1b[0m-+-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[31m\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 1) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c\x1b[31m|%c|\x1b[0m%c\n\r-\x1b[31m+-+\x1b[0m-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 1) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c\x1b[32m|%c|\x1b[0m%c\n\r-\x1b[32m+-+\x1b[0m-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 2) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c\x1b[31m|%c\x1b[0m\n\r-+-\x1b[31m+-\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 2) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c\x1b[32m|%c\x1b[0m\n\r-+-\x1b[32m+-\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 3) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r\x1b[31m-+\x1b[0m-+-\n\r\x1b[31m%c|\x1b[0m%c|%c\n\r\x1b[31m-+\x1b[0m-+-\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 3) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r\x1b[32m-+\x1b[0m-+-\n\r\x1b[32m%c|\x1b[0m%c|%c\n\r\x1b[32m-+\x1b[0m-+-\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 4) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-\x1b[31m+-+\x1b[0m-\n\r%c\x1b[31m|%c|\x1b[0m%c\n\r-\x1b[31m+-+\x1b[0m-\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 4) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-\x1b[32m+-+\x1b[0m-\n\r%c\x1b[32m|%c|\x1b[0m%c\n\r-\x1b[32m+-+\x1b[0m-\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	} else if (currentPlayer == 'X' && cursor == 5) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-\x1b[31m+-\x1b[0m\n\r%c|%c\x1b[31m|%c\x1b[0m\n\r-+-\x1b[31m+-\x1b[0m\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 5) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-\x1b[32m+-\x1b[0m\n\r%c|%c\x1b[32m|%c\x1b[0m\n\r-+-\x1b[32m+-\x1b[0m\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	} else if (currentPlayer == 'X' && cursor == 6) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\x1b[31m-+\x1b[0m-+-\n\r\x1b[31m%c|\x1b[0m%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 6) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\x1b[32m-+\x1b[0m-+-\n\r\x1b[32m%c|\x1b[0m%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	} else if (currentPlayer == 'X' && cursor == 7) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-\x1b[31m+-+\x1b[0m-\n\r%c\x1b[31m|%c|\x1b[0m%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 7) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-\x1b[32m+-+\x1b[0m-\n\r%c\x1b[32m|%c|\x1b[0m%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	} else if (currentPlayer == 'X' && cursor == 8) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-+-\x1b[31m+-\x1b[0m\n\r%c|%c\x1b[31m|%c\x1b[0m\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 8) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-+-\x1b[32m+-\x1b[0m\n\r%c|%c\x1b[32m|%c\x1b[0m\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	}
	HAL_UART_Transmit_IT(&huart1, data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	HAL_GPIO_TogglePin(LD7_GPIO_Port, LD7_Pin); // Zmiana stanu pinu na diodzie LED

}

static void drawOnMove(bool gameInProgress) {
	uint16_t size = 0;
	char fieldChar[9];
	char currentPlayer;

	for (uint8_t i = 0; i < 9; i++) {
		if (field[i] == 0) {
			fieldChar[i] = 'X';
		} else if (field[i] == 1) {
			fieldChar[i] = 'O';
		} else if (field[i] == 2) {
			fieldChar[i] = ' ';
		} else {
			fieldChar[i] = ' ';
		}
	}

	if (player == 0 && gameInProgress == false) {
		currentPlayer = 'X';
	} else if (player == 1 && gameInProgress == false) {
		currentPlayer = 'O';
	} else if (player == 1 && gameInProgress == true) {
		currentPlayer = 'X';
	} else if (player == 0 && gameInProgress == true) {
		currentPlayer = 'O';
	}

	if (currentPlayer == 'X' && cursor == 0) {
		size =
				sprintf(data,
						"\n\r\x1b[31m%c|\x1b[0m%c|%c\n\r\x1b[31m-+\x1b[0m-+-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[31m\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 0) {
		size =
				sprintf(data,
						"\n\r\x1b[32m%c|\x1b[0m%c|%c\n\r\x1b[32m-+\x1b[0m-+-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[31m\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 1) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c\x1b[31m|%c|\x1b[0m%c\n\r-\x1b[31m+-+\x1b[0m-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 1) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c\x1b[32m|%c|\x1b[0m%c\n\r-\x1b[32m+-+\x1b[0m-\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 2) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c\x1b[31m|%c\x1b[0m\n\r-+-\x1b[31m+-\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 2) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c\x1b[32m|%c\x1b[0m\n\r-+-\x1b[32m+-\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 3) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r\x1b[31m-+\x1b[0m-+-\n\r\x1b[31m%c|\x1b[0m%c|%c\n\r\x1b[31m-+\x1b[0m-+-\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 3) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r\x1b[32m-+\x1b[0m-+-\n\r\x1b[32m%c|\x1b[0m%c|%c\n\r\x1b[32m-+\x1b[0m-+-\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'X' && cursor == 4) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-\x1b[31m+-+\x1b[0m-\n\r%c\x1b[31m|%c|\x1b[0m%c\n\r-\x1b[31m+-+\x1b[0m-\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 4) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-\x1b[32m+-+\x1b[0m-\n\r%c\x1b[32m|%c|\x1b[0m%c\n\r-\x1b[32m+-+\x1b[0m-\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	} else if (currentPlayer == 'X' && cursor == 5) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-\x1b[31m+-\x1b[0m\n\r%c|%c\x1b[31m|%c\x1b[0m\n\r-+-\x1b[31m+-\x1b[0m\n\r%c|%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 5) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-\x1b[32m+-\x1b[0m\n\r%c|%c\x1b[32m|%c\x1b[0m\n\r-+-\x1b[32m+-\x1b[0m\n\r%c|%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	} else if (currentPlayer == 'X' && cursor == 6) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\x1b[31m-+\x1b[0m-+-\n\r\x1b[31m%c|\x1b[0m%c|%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 6) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r\x1b[32m-+\x1b[0m-+-\n\r\x1b[32m%c|\x1b[0m%c|%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	} else if (currentPlayer == 'X' && cursor == 7) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-\x1b[31m+-+\x1b[0m-\n\r%c\x1b[31m|%c|\x1b[0m%c\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 7) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-\x1b[32m+-+\x1b[0m-\n\r%c\x1b[32m|%c|\x1b[0m%c\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	} else if (currentPlayer == 'X' && cursor == 8) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-+-\x1b[31m+-\x1b[0m\n\r%c|%c\x1b[31m|%c\x1b[0m\n\r\n\r\x1b[31mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);
	} else if (currentPlayer == 'O' && cursor == 8) {
		size =
				sprintf(data,
						"\x1b[0m\n\r%c|%c|%c\n\r-+-+-\n\r%c|%c|%c\n\r-+-\x1b[32m+-\x1b[0m\n\r%c|%c\x1b[32m|%c\x1b[0m\n\r\n\r\x1b[32mTura gracza %c!\n\r\x1b[0m\n\r",
						fieldChar[0], fieldChar[1], fieldChar[2], fieldChar[3],
						fieldChar[4], fieldChar[5], fieldChar[6], fieldChar[7],
						fieldChar[8], currentPlayer);

	}
	HAL_UART_Transmit_IT(&huart1, data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	HAL_GPIO_TogglePin(LD7_GPIO_Port, LD7_Pin); // Zmiana stanu pinu na diodzie LED

}

static void drawBoard() {
	ssd1331_draw_line(5, 20, 50, 20, PURPLE);
	ssd1331_draw_line(5, 35, 50, 35, PURPLE);
	ssd1331_draw_line(5, 50, 50, 50, PURPLE);
	ssd1331_draw_line(20, 5, 20, 50, PURPLE);
	ssd1331_draw_line(35, 5, 35, 50, PURPLE);
	ssd1331_draw_line(5, 5, 50, 5, PURPLE);
	ssd1331_draw_line(5, 5, 5, 50, PURPLE);
	ssd1331_draw_line(50, 5, 50, 50, PURPLE); //rysowanie planszy
}

static void writeScore() {
	ssd1331_display_string(55, 5, "Wynik:", FONT_1206, BROWN);
	ssd1331_display_string(55, 20, "X:", FONT_1206, BROWN);
	ssd1331_display_string(55, 35, "O:", FONT_1206, BROWN);
	if (scoreX >= 100) {
		ssd1331_display_num(70, 20, scoreX, 3, FONT_1206, BROWN);
	} else if (scoreX >= 10) {
		ssd1331_display_num(70, 20, scoreX, 2, FONT_1206, BROWN);
	} else if (scoreX < 10) {
		ssd1331_display_num(70, 20, scoreX, 1, FONT_1206, BROWN);
	}

	if (scoreO >= 100) {
		ssd1331_display_num(70, 35, scoreO, 3, FONT_1206, BROWN);
	} else if (scoreO >= 10) {
		ssd1331_display_num(70, 35, scoreO, 2, FONT_1206, BROWN);
	} else if (scoreO < 10) {
		ssd1331_display_num(70, 35, scoreO, 1, FONT_1206, BROWN);
	}

}

static void newGame() {
	HAL_Delay(2000);
	ssd1331_clear_screen(GREY);
	drawBoard();
	ssd1331_draw_line(5, 20, 20, 20, RED);
	ssd1331_draw_line(20, 5, 20, 20, RED);
	ssd1331_draw_line(5, 5, 5, 20, RED);
	ssd1331_draw_line(5, 5, 20, 5, RED); //poczatkowa pozycja kursora
	writeScore();
	win = 0;
	player = 0;
	cursor = 0;
	move = 0;
	draw = 0;
	for (i = 0; i < 9; i++) {
		field[i] = 2;
	}
	drawOnScreen(false);
}

static void checkWin() {
	if (player == 0) {
		if ((field[0] == 0 && field[1] == 0 && field[2] == 0)
				|| (field[3] == 0 && field[4] == 0 && field[5] == 0)
				|| (field[6] == 0 && field[7] == 0 && field[8] == 0)
				|| (field[0] == 0 && field[3] == 0 && field[6] == 0)
				|| (field[1] == 0 && field[4] == 0 && field[7] == 0)
				|| (field[2] == 0 && field[5] == 0 && field[8] == 0)
				|| (field[2] == 0 && field[4] == 0 && field[6] == 0)
				|| (field[0] == 0 && field[4] == 0 && field[8] == 0)) {
			win = 1;
			scoreX++;
			ssd1331_clear_screen(GREY);
			ssd1331_display_string(0, 0, "GRACZ X WYGRAL", FONT_1206, RED);
		}
	}
	if (player == 1) {
		if ((field[0] == 1 && field[1] == 1 && field[2] == 1)
				|| (field[3] == 1 && field[4] == 1 && field[5] == 1)
				|| (field[6] == 1 && field[7] == 1 && field[8] == 1)
				|| (field[0] == 1 && field[3] == 1 && field[6] == 1)
				|| (field[1] == 1 && field[4] == 1 && field[7] == 1)
				|| (field[2] == 1 && field[5] == 1 && field[8] == 1)
				|| (field[2] == 1 && field[4] == 1 && field[6] == 1)
				|| (field[0] == 1 && field[4] == 1 && field[8] == 1)) {
			win = 1;
			scoreO++;
			ssd1331_clear_screen(GREY);
			ssd1331_display_string(0, 0, "GRACZ O WYGRAL", FONT_1206, BROWN);
		}
	}
}

static void checkDraw() {
	if (field[0] != 2 && field[1] != 2 && field[2] != 2 && field[3] != 2
			&& field[4] != 2 && field[5] != 2 && field[6] != 2 && field[7] != 2
			&& field[8] != 2) {
		ssd1331_clear_screen(GREY);
		ssd1331_display_string(0, 0, "REMIS", FONT_1608, BLACK);
		draw = 1;
	}
}

static void drawCursor00() {
	drawBoard();
	ssd1331_draw_line(5, 20, 20, 20, RED);
	ssd1331_draw_line(20, 5, 20, 20, RED);
	ssd1331_draw_line(5, 5, 5, 20, RED);
	ssd1331_draw_line(5, 5, 20, 5, RED);
}

static void drawCursor10() {
	drawBoard();
	ssd1331_draw_line(35, 5, 35, 20, RED);
	ssd1331_draw_line(20, 20, 35, 20, RED);
	ssd1331_draw_line(20, 5, 20, 20, RED);
	ssd1331_draw_line(20, 5, 35, 5, RED);
}

static void drawCursor20() {
	drawBoard();
	ssd1331_draw_line(50, 5, 50, 20, RED);
	ssd1331_draw_line(35, 20, 50, 20, RED);
	ssd1331_draw_line(35, 5, 35, 20, RED);
	ssd1331_draw_line(35, 5, 50, 5, RED);
}

static void drawCursor30() {
	drawBoard();
	ssd1331_draw_line(5, 20, 20, 20, RED);
	ssd1331_draw_line(5, 20, 5, 35, RED);
	ssd1331_draw_line(5, 35, 20, 35, RED);
	ssd1331_draw_line(20, 20, 20, 35, RED);
}

static void drawCursor40() {
	drawBoard();
	ssd1331_draw_line(20, 20, 35, 20, RED);
	ssd1331_draw_line(20, 20, 20, 35, RED);
	ssd1331_draw_line(20, 35, 35, 35, RED);
	ssd1331_draw_line(35, 20, 35, 35, RED);
}

static void drawCursor50() {
	drawBoard();
	ssd1331_draw_line(35, 20, 50, 20, RED);
	ssd1331_draw_line(35, 20, 35, 35, RED);
	ssd1331_draw_line(35, 35, 50, 35, RED);
	ssd1331_draw_line(50, 20, 50, 35, RED);
}

static void drawCursor60() {
	drawBoard();
	ssd1331_draw_line(5, 35, 5, 50, RED);
	ssd1331_draw_line(5, 35, 20, 35, RED);
	ssd1331_draw_line(5, 50, 20, 50, RED);
	ssd1331_draw_line(20, 35, 20, 50, RED);
}

static void drawCursor70() {
	drawBoard();
	ssd1331_draw_line(20, 35, 35, 35, RED);
	ssd1331_draw_line(20, 35, 20, 50, RED);
	ssd1331_draw_line(20, 50, 35, 50, RED);
	ssd1331_draw_line(35, 35, 35, 50, RED);
}

static void drawCursor80() {
	drawBoard();
	ssd1331_draw_line(35, 35, 50, 35, RED);
	ssd1331_draw_line(35, 35, 35, 50, RED);
	ssd1331_draw_line(35, 50, 50, 50, RED);
	ssd1331_draw_line(50, 35, 50, 50, RED);
}

static void drawCursor01() {
	drawBoard();
	ssd1331_draw_line(5, 20, 20, 20, GREEN);
	ssd1331_draw_line(20, 5, 20, 20, GREEN);
	ssd1331_draw_line(5, 5, 5, 20, GREEN);
	ssd1331_draw_line(5, 5, 20, 5, GREEN);
}

static void drawCursor11() {
	drawBoard();
	ssd1331_draw_line(35, 5, 35, 20, GREEN);
	ssd1331_draw_line(20, 20, 35, 20, GREEN);
	ssd1331_draw_line(20, 5, 20, 20, GREEN);
	ssd1331_draw_line(20, 5, 35, 5, GREEN);
}

static void drawCursor21() {
	drawBoard();
	ssd1331_draw_line(50, 5, 50, 20, GREEN);
	ssd1331_draw_line(35, 20, 50, 20, GREEN);
	ssd1331_draw_line(35, 5, 35, 20, GREEN);
	ssd1331_draw_line(35, 5, 50, 5, GREEN);
}

static void drawCursor31() {
	drawBoard();
	ssd1331_draw_line(5, 20, 20, 20, GREEN);
	ssd1331_draw_line(5, 20, 5, 35, GREEN);
	ssd1331_draw_line(5, 35, 20, 35, GREEN);
	ssd1331_draw_line(20, 20, 20, 35, GREEN);
}

static void drawCursor41() {
	drawBoard();
	ssd1331_draw_line(20, 20, 35, 20, GREEN);
	ssd1331_draw_line(20, 20, 20, 35, GREEN);
	ssd1331_draw_line(20, 35, 35, 35, GREEN);
	ssd1331_draw_line(35, 20, 35, 35, GREEN);
}

static void drawCursor51() {
	drawBoard();
	ssd1331_draw_line(35, 20, 50, 20, GREEN);
	ssd1331_draw_line(35, 20, 35, 35, GREEN);
	ssd1331_draw_line(35, 35, 50, 35, GREEN);
	ssd1331_draw_line(50, 20, 50, 35, GREEN);
}

static void drawCursor61() {
	drawBoard();
	ssd1331_draw_line(5, 35, 5, 50, GREEN);
	ssd1331_draw_line(5, 35, 20, 35, GREEN);
	ssd1331_draw_line(5, 50, 20, 50, GREEN);
	ssd1331_draw_line(20, 35, 20, 50, GREEN);
}

static void drawCursor71() {
	drawBoard();
	ssd1331_draw_line(20, 35, 35, 35, GREEN);
	ssd1331_draw_line(20, 35, 20, 50, GREEN);
	ssd1331_draw_line(20, 50, 35, 50, GREEN);
	ssd1331_draw_line(35, 35, 35, 50, GREEN);
}

static void drawCursor81() {
	drawBoard();
	ssd1331_draw_line(35, 35, 50, 35, GREEN);
	ssd1331_draw_line(35, 35, 35, 50, GREEN);
	ssd1331_draw_line(35, 50, 50, 50, GREEN);
	ssd1331_draw_line(50, 35, 50, 50, GREEN);
}

static void moveCursorOnScreen() {
	if (player == 0) {

		if (cursor == 0) {
			drawCursor00();

		} else if (cursor == 1) {
			drawCursor10();

		} else if (cursor == 2) {
			drawCursor20();

		} else if (cursor == 3) {
			drawCursor30();

		} else if (cursor == 4) {
			drawCursor40();

		} else if (cursor == 5) {
			drawCursor50();

		} else if (cursor == 6) {
			drawCursor60();

		} else if (cursor == 7) {
			drawCursor70();

		} else if (cursor == 8) {
			drawCursor80();

		}
	} else {
		if (cursor == 0) {
			drawCursor01();

		} else if (cursor == 1) {
			drawCursor11();

		} else if (cursor == 2) {
			drawCursor21();

		} else if (cursor == 3) {
			drawCursor31();

		} else if (cursor == 4) {
			drawCursor41();

		} else if (cursor == 5) {
			drawCursor51();

		} else if (cursor == 6) {
			drawCursor61();

		} else if (cursor == 7) {
			drawCursor71();

		} else if (cursor == 8) {
			drawCursor81();

		}
	}
	drawOnMove(false);
}

static void insert() {
	if (player == 0) {
		if (cursor == 0) {
			if (field[0] == 2) {
				field[0] = 0;
				ssd1331_display_string(8, 5, "X", FONT_1608, BLACK);
				drawCursor01();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;
		} else if (cursor == 1) {
			if (field[1] == 2) {

				field[1] = 0;
				ssd1331_display_string(23, 5, "X", FONT_1608, BLACK);
				drawCursor11();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();

			} else
				error = 1;
		}

		else if (cursor == 2) {
			if (field[2] == 2) {

				field[2] = 0;
				ssd1331_display_string(38, 5, "X", FONT_1608, BLACK);
				drawCursor21();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 3) {
			if (field[3] == 2) {

				field[3] = 0;
				ssd1331_display_string(8, 20, "X", FONT_1608, BLACK);
				drawCursor31();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 4) {
			if (field[4] == 2) {

				field[4] = 0;
				ssd1331_display_string(23, 20, "X", FONT_1608, BLACK);
				drawCursor41();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 5) {
			if (field[5] == 2) {

				field[5] = 0;
				ssd1331_display_string(38, 20, "X", FONT_1608, BLACK);
				drawCursor51();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 6) {
			if (field[6] == 2) {

				field[6] = 0;
				ssd1331_display_string(8, 35, "X", FONT_1608, BLACK);
				drawCursor61();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 7) {
			if (field[7] == 2) {

				field[7] = 0;
				ssd1331_display_string(23, 35, "X", FONT_1608, BLACK);
				drawCursor71();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 8) {
			if (field[8] == 2) {

				field[8] = 0;
				ssd1331_display_string(38, 35, "X", FONT_1608, BLACK);
				drawCursor81();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

	} else if (player == 1) {
		if (cursor == 0) {
			if (field[0] == 2) {
				field[0] = 1;
				ssd1331_display_string(8, 5, "O", FONT_1608, BLACK);
				drawCursor00();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		} else if (cursor == 1) {
			if (field[1] == 2) {
				field[1] = 1;
				ssd1331_display_string(23, 5, "O", FONT_1608, BLACK);
				drawCursor10();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 2) {
			if (field[2] == 2) {
				field[2] = 1;
				ssd1331_display_string(38, 5, "O", FONT_1608, BLACK);
				drawCursor20();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 3) {
			if (field[3] == 2) {
				field[3] = 1;
				ssd1331_display_string(8, 20, "O", FONT_1608, BLACK);
				drawCursor30();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 4) {
			if (field[4] == 2) {
				field[4] = 1;
				ssd1331_display_string(23, 20, "O", FONT_1608, BLACK);
				drawCursor40();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 5) {
			if (field[5] == 2) {
				field[5] = 1;
				ssd1331_display_string(38, 20, "O", FONT_1608, BLACK);
				drawCursor50();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 6) {
			if (field[6] == 2) {
				field[6] = 1;
				ssd1331_display_string(8, 35, "O", FONT_1608, BLACK);
				drawCursor60();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 7) {
			if (field[7] == 2) {
				field[7] = 1;
				ssd1331_display_string(23, 35, "O", FONT_1608, BLACK);
				drawCursor70();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

		else if (cursor == 8) {
			if (field[8] == 2) {
				field[8] = 1;
				ssd1331_display_string(38, 35, "O", FONT_1608, BLACK);
				drawCursor80();
				checkWin();
				if (win == 1) {

				} else
					checkDraw();
			} else
				error = 1;

		}

	}
	if (error == 0) {
		drawOnScreen(true);
		if (player == 0)
			player = 1;
		else
			player = 0;
	} else {
		error = 0;
	}

	HAL_Delay(500);
	move = 0;
	if (win == 1 || draw == 1) {
		newGame();
	}

}

static void displayScoreboard() {
	uint16_t size = 0;

	size = sprintf(data,
			"\n\rWynik:\n\r\x1b[31mX: %d\n\r\x1b[32mO: %d\x1b[0m\n\r", scoreX,
			scoreO);

	HAL_UART_Transmit_IT(&huart1, data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	HAL_GPIO_TogglePin(LD7_GPIO_Port, LD7_Pin); // Zmiana stanu pinu na diodzie LED

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	move = 1;
}

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_SPI1_Init();
	MX_ADC1_Init();
	MX_USART1_UART_Init();
	MX_TIM16_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_UART_Receive_IT(&huart1, &Received, 1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) Joystick, 2);

	size =
			sprintf(data,
					"Sterowanie:\n\rw - kursor w gore\n\rs - kursor w dol\n\ra - kursor w lewo\n\rd - kursor w prawo\n\rf - zajmij pole\n\rg - pokaz tablice wynikow\n\r ");

	HAL_UART_Transmit_IT(&huart1, data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	ssd1331_init();
	ssd1331_clear_screen(GREY);
	drawBoard();
	ssd1331_draw_line(5, 20, 20, 20, RED);
	ssd1331_draw_line(20, 5, 20, 20, RED);
	ssd1331_draw_line(5, 5, 5, 20, RED);
	ssd1331_draw_line(5, 5, 20, 5, RED); //poczatkowa pozycja kursora
	writeScore();
	for (i = 0; i < 9; i++) {
		field[i] = 2;
	}
	drawOnScreen(false);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		buffer[0] = 0;
		HAL_UART_Receive(&huart1, buffer, 20, 100);
		if (buffer[0] == 'w') {
			if (cursor >= 3) {
				cursor -= 3;
				moveCursorOnScreen();
			}
			HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
		} else if (buffer[0] == 's') {
			if (cursor <= 5) {
				cursor += 3;
				moveCursorOnScreen();
			}
			HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
		} else if (buffer[0] == 'a') {
			if (cursor != 0 && cursor != 3 && cursor != 6) {
				cursor -= 1;
				moveCursorOnScreen();
			}
			HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
		} else if (buffer[0] == 'd') {
			if (cursor != 2 && cursor != 5 && cursor != 8) {
				cursor += 1;
				moveCursorOnScreen();
			}
			HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
		} else if (buffer[0] == 'f') {
			insert();
		} else if (buffer[0] == 'g') {
			displayScoreboard();
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		if (move == 1) {
			insert();

		}

		if (Joystick[0] > 3500) {
			if (cursor != 8) {
				cursor++;
			} else
				cursor = 0;

			moveCursorOnScreen();
			HAL_Delay(500);
		}
		if (Joystick[0] < 500) {
			if (cursor != 0) {
				cursor--;
			} else
				cursor = 8;

			moveCursorOnScreen();
			HAL_Delay(500);

		}

	}
	/* USER CODE END 3 */

}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1
			| RCC_PERIPHCLK_ADC12;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void) {

	ADC_MultiModeTypeDef multimode;
	ADC_ChannelConfTypeDef sConfig;

	/**Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 2;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.SamplingTime = ADC_SAMPLETIME_181CYCLES_5;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* SPI1 init function */
static void MX_SPI1_Init(void) {

	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* TIM16 init function */
static void MX_TIM16_Init(void) {

	htim16.Instance = TIM16;
	htim16.Init.Prescaler = 9999;
	htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim16.Init.Period = 3599;
	htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim16.Init.RepetitionCounter = 0;
	htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim16) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART1 init function */
static void MX_USART1_UART_Init(void) {

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/** 
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE()
	;

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 PA11   ------> USB_DM
 PA12   ------> USB_DP
 PD6   ------> USART2_RX
 PB6   ------> I2C1_SCL
 PB7   ------> I2C1_SDA
 */
static void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE()
	;
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOF_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	__HAL_RCC_GPIOD_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE,
			CS_I2C_SPI_Pin | RES_Pin | LD4_Pin | LD3_Pin | LD5_Pin | LD7_Pin
					| LD9_Pin | LD10_Pin | LD8_Pin | LD6_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : DRDY_Pin MEMS_INT3_Pin MEMS_INT4_Pin MEMS_INT2_Pin */
	GPIO_InitStruct.Pin = DRDY_Pin | MEMS_INT3_Pin | MEMS_INT4_Pin
			| MEMS_INT2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : CS_I2C_SPI_Pin RES_Pin LD4_Pin LD3_Pin
	 LD5_Pin LD7_Pin LD9_Pin LD10_Pin
	 LD8_Pin LD6_Pin */
	GPIO_InitStruct.Pin = CS_I2C_SPI_Pin | RES_Pin | LD4_Pin | LD3_Pin | LD5_Pin
			| LD7_Pin | LD9_Pin | LD10_Pin | LD8_Pin | LD6_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : B2_Pin */
	GPIO_InitStruct.Pin = B2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(B2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : CS_Pin */
	GPIO_InitStruct.Pin = CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DC_Pin */
	GPIO_InitStruct.Pin = DC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DC_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : DM_Pin DP_Pin */
	GPIO_InitStruct.Pin = DM_Pin | DP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF14_USB;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PD6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : I2C1_SCL_Pin I2C1_SDA_Pin */
	GPIO_InitStruct.Pin = I2C1_SCL_Pin | I2C1_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
