/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "stm32h7xx_hal.h"
#include "SSD1680.h"
#include "fonts.h"

#include "interfacev.h"
#include "fc_weights.h"
//#include "model_weights.c"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBUG_MODE 1

#define INPUT_DIM  9
#define HIDDEN_1   64
#define HIDDEN_2   32
#define OUTPUT_DIM 5
#define REFRESH_MODE 199
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart7;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_UART7_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void parse_gpgga(uint8_t *line);
void HM10_Process(uint8_t byte);
void Handle_BT_Command(char* cmd);
void Process_WiFi_Data(void);
void debug_disp(void);
int mlp_forward_pass(double current_lat, double current_lon, uint8_t hour_of_day, uint16_t min_of_day, uint8_t day_of_week,
					 int time_to_dept, uint8_t is_ticket_reg, uint8_t is_entering, uint8_t is_exiting);
void epd_ticket_handler(char *train_type, char *train_kind, char *train_num, char *date, char *dept_time,
						char *dept_sta, char *arr_time, char *arr_sta, char *car, char *seat, char *price);
char* sta_code_decoder(char *sta_code);
SPI_HandleTypeDef hspi1;
SSD1680_HandleTypeDef hepd;

static void MX_SSD1680_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t idx = 0;
uint8_t BTBuffer[1];

#define WIFI_BUF_SIZE 512
static uint8_t WFBuffer[WIFI_BUF_SIZE];
static uint16_t wf_idx = 0;

uint8_t GPSBuffer[128];

typedef struct {
    char ssid[32];
    int8_t rssi;
} WiFiInfo;

#define MAX_WIFI_COUNT 5
static WiFiInfo wifi_list[MAX_WIFI_COUNT];
static uint8_t wifi_count = 0;

#define BT_BUF_SIZE 16
static char btrecv[BT_BUF_SIZE];
static uint8_t bt_idx = 0;

char parts[15][16];

int result;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_UART7_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  MX_SSD1680_Init();

  SSD1680_Border(&hepd, ColorWhite);
  SSD1680_Clear(&hepd, ColorWhite);
  SSD1680_Refresh(&hepd, REFRESH_MODE);

  HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
  HAL_TIM_Base_Start_IT(&htim1);

  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  HAL_TIM_Base_Start_IT(&htim2);

  HAL_NVIC_SetPriority(UART5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(UART5_IRQn);

  typedef struct
  {
      GPIO_TypeDef* port;
      uint16_t pin;
      GPIO_PinState state;
  }
  GpioSet;

  GpioSet list[] =
  {
      {BT_BLED_GPIO_Port,  BT_BLED_Pin,  GPIO_PIN_SET},
      {BT_RLED_GPIO_Port,  BT_RLED_Pin,  GPIO_PIN_RESET},
      {GPS_RLED_GPIO_Port, GPS_RLED_Pin, GPIO_PIN_RESET},
      {GPS_GLED_GPIO_Port, GPS_GLED_Pin, GPIO_PIN_SET},
      {WF_LED_GPIO_Port,   WF_LED_Pin,   GPIO_PIN_SET},
      {STA_LED_GPIO_Port,  STA_LED_Pin,  GPIO_PIN_SET},
      {EC_LED_GPIO_Port,   EC_LED_Pin,   GPIO_PIN_SET},
      {EC_RELAY_GPIO_Port, EC_RELAY_Pin, GPIO_PIN_RESET},
	  {EPD_EN_GPIO_Port, EPD_EN_Pin, GPIO_PIN_RESET}
  };

  for (int i = 0; i < sizeof(list)/sizeof(list[0]); i++)
  {
      HAL_GPIO_WritePin(list[i].port, list[i].pin, list[i].state);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Receive_IT(&huart4, BTBuffer, 1);
  HAL_UART_Receive_IT(&huart5, WFBuffer, 1);
  HAL_UART_Receive_IT(&huart7, (uint8_t*)GPSBuffer, 1);

  // Start Define Variable
  uint8_t msg[] = "STM32 is now on\n";
  char input_train_type[10] = {0};
  char input_train_kind[20] = {0};
  char input_train_num[10] = {0};
  char input_date[16] = {0};
  char input_dept_time[10] = {0};
  char input_arr_time[10] = {0};
  char input_dept_sta[20] = {0};
  char input_arr_sta[20] = {0};
  char input_car[10] = {0};
  char input_seat[10] = {0};
  char input_price[10] = {0};
  //(char *train_type, char *train_kind, char *train_num, char *date, char *dept_time, char *dept_sta, char *arr_time, char *arr_sta, char *car, char *seat, char *price)

  double debug_array[9] = {24.167680, 120.653612, 17, 1072, 5, 1, 0, 0, 1};
  char *debug_ticket[4] = {"A50111562576561ZZZZ33003230ZZZZZ6ZP7UNxP7UNx15VNA000V7UF0F07", //台中->豐原
	  "N50049351999418ZZZZ10803300ZZZZZ3ZP1VLUP1VNI161NA004y1VHEA49", //桃園->台中
	  "0720412420244007936860000849004202508301834470072025083019154701000000300000030030030000000540006001INTIRS000020250830109ABE", //桃園->台中
	  "0421511450742087503470000333004202505252215520072025052522475201000000100000080020010000000540006000INTETS000020250525101909"  //台中->板橋
  };

  // End Define Variable

  if (DEBUG_MODE)
  {
	  	HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 1000);

	    result = mlp_forward_pass(debug_array[0], debug_array[1], debug_array[2], debug_array[3], debug_array[4], debug_array[5], debug_array[6], debug_array[7],debug_array[8]);

	    for (int i = 0; i < (sizeof(debug_ticket) / sizeof(debug_ticket[0])); i++)
	    {
	    	if (isalpha((unsigned)debug_ticket[i][0]))
	    	{
	    		strcpy(input_train_type, "TRA");
	    		if (debug_ticket[i][0] == 'A')  strcpy(input_train_kind, "Local");
	    		if (debug_ticket[i][0] == 'N')  strcpy(input_train_kind, "T.C.Exp.");
	    	}

	    	else
	    	{
	    		strcpy(input_train_type, "THSR");
	    		strcpy(input_train_kind, "Standard");
	    		strncpy(input_train_num, debug_ticket[i] + 24 , 4); // Train number
	    		strncpy(input_dept_sta, debug_ticket[i] + 30, 2);   // Departure station code
	    		//input_dept_sta = sta_code_decoder(dept_sta_code);
	    		char raw_date[9] = {0};
	    		strncpy(raw_date, debug_ticket[i] + 31, 8);      // Date
	    		input_date[0] = raw_date[0];
	    		input_date[1] = raw_date[1];
	    		input_date[2] = raw_date[2];
	    		input_date[3] = raw_date[3];
	    		input_date[4] = '-';
	    		input_date[5] = raw_date[4];
	    		input_date[6] = raw_date[5];
	    		input_date[7] = '-';
	    		input_date[8] = raw_date[6];
	    		input_date[9] = raw_date[7];
	    		input_date[10] = '\0';

	    		strncpy(input_dept_time, debug_ticket[i] + 39, 4); // Departure time
	    		strncpy(input_arr_sta, debug_ticket[i] + 46, 2);    // Arrival station code
	    		strncpy(input_arr_time, debug_ticket[i] + 56, 4);  // Arrival time
	    		strncpy(input_car, debug_ticket[i] + 76, 2);       // Car
	    		strncpy(input_seat, debug_ticket[i] + 79, 5);      // Seat
	    		strncpy(input_arr_sta, debug_ticket[i] + 90, 1);    // Price
	    	}

	    	//(char *train_type, char *train_kind, char *train_num, char *date, char *dept_time, char *dept_sta, char *arr_time, char *arr_sta, char *car, char *seat, char *price)
	    	epd_ticket_handler(input_train_type, input_train_kind, input_train_num, input_date, input_dept_time, input_dept_sta,
	    			input_arr_time, input_arr_sta, input_car, input_seat, input_price);
	    	//debug_disp();
	    	HAL_Delay(3000);
	    }
  }

  else if (!DEBUG_MODE)
  {
	  HAL_GPIO_WritePin(EPD_EN_GPIO_Port, EPD_EN_Pin, GPIO_PIN_SET);
	  HAL_Delay(100);
	  SSD1680_Clear(&hepd, ColorWhite);
	  SSD1680_DrawBitmap(&hepd, 0, 0, startupv, 152, 296);
	  SSD1680_Refresh(&hepd, REFRESH_MODE);
	  HAL_Delay(2000);
	  SSD1680_DrawBitmap(&hepd, 0, 0, einvoicev, 152, 296);
	  SSD1680_Refresh(&hepd, REFRESH_MODE);
	  HAL_Delay(1000);
	  SSD1680_DrawBitmap(&hepd, 0, 0, easycardv, 152, 296);
	  SSD1680_Refresh(&hepd, REFRESH_MODE);
	  HAL_Delay(1000);
	  SSD1680_DrawBitmap(&hepd, 0, 0, exhomescreen, 152, 296);
	  SSD1680_Refresh(&hepd, REFRESH_MODE);
	  HAL_Delay(1000);
	  SSD1680_DrawBitmap(&hepd, 0, 0, extra, 152, 296);
	  SSD1680_Refresh(&hepd, REFRESH_MODE);
	  HAL_Delay(1000);
	  SSD1680_DrawBitmap(&hepd, 0, 0, exthsr, 152, 296);
	  SSD1680_Refresh(&hepd, REFRESH_MODE);
	  HAL_Delay(3000);
	  HAL_GPIO_WritePin(EPD_EN_GPIO_Port, EPD_EN_Pin, GPIO_PIN_RESET);
  }

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 47999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 9999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 47999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 49999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief UART7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART7_Init(void)
{

  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 9600;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart7.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart7, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart7, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */

  /* USER CODE END UART7_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, STA_LED_Pin|WF_LED_Pin|BT_RLED_Pin|BT_BLED_Pin
                          |EC_RELAY_Pin|EC_LED_Pin|EPD_RST_Pin|EPD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EPD_CS_GPIO_Port, EPD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPS_RLED_Pin|GPS_GLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EPD_EN_GPIO_Port, EPD_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : STA_LED_Pin WF_LED_Pin BT_RLED_Pin BT_BLED_Pin
                           EC_RELAY_Pin EC_LED_Pin EPD_RST_Pin EPD_DC_Pin */
  GPIO_InitStruct.Pin = STA_LED_Pin|WF_LED_Pin|BT_RLED_Pin|BT_BLED_Pin
                          |EC_RELAY_Pin|EC_LED_Pin|EPD_RST_Pin|EPD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : EPD_CS_Pin */
  GPIO_InitStruct.Pin = EPD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EPD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EPD_BUSY_Pin */
  GPIO_InitStruct.Pin = EPD_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EPD_BUSY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : GPS_RLED_Pin GPS_GLED_Pin */
  GPIO_InitStruct.Pin = GPS_RLED_Pin|GPS_GLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : EPD_EN_Pin */
  GPIO_InitStruct.Pin = EPD_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EPD_EN_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void MX_SSD1680_Init(void)
{
    hepd.SPI_Handle = &hspi1;
    hepd.CS_Port = EPD_CS_GPIO_Port;
    hepd.CS_Pin  = EPD_CS_Pin;
    hepd.DC_Port = EPD_DC_GPIO_Port;
    hepd.DC_Pin  = EPD_DC_Pin;
    hepd.RESET_Port = EPD_RST_GPIO_Port;
    hepd.RESET_Pin  = EPD_RST_Pin;
    hepd.BUSY_Port  = EPD_BUSY_GPIO_Port;
    hepd.BUSY_Pin   = EPD_BUSY_Pin;
    hepd.Color_Depth = 1;
    hepd.SPI_Timeout = 100;
    hepd.Scan_Mode  = WideScan;
    hepd.Resolution_X = 152;
    hepd.Resolution_Y = 296;

    SSD1680_Init(&hepd);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART4) // BT
    {
		HM10_Process(BTBuffer[0]);
		HAL_UART_Receive_IT(&huart4, BTBuffer, 1);
    }

    else if (huart->Instance == UART5) // WiFi
    {

    	HAL_UART_Transmit(&huart2, WFBuffer, 1, 100);
		HAL_UART_Receive_IT(&huart5, WFBuffer, 1);

		uint8_t byte = WFBuffer[wf_idx];

		// Debug: 顯示原始數據
		#if DEBUG_MODE
			if (byte >= ' ' && byte <= '~')
			{
				HAL_UART_Transmit(&huart2, &byte, 1, 10);
			}
			else if (byte == '\r')
			{
				uint8_t cr[] = "<CR>";
				HAL_UART_Transmit(&huart2, cr, 4, 10);
			}
			else if (byte == '\n')
			{
				uint8_t lf[] = "<LF>\r\n";
				HAL_UART_Transmit(&huart2, lf, 6, 10);
			}
		#endif

		if (wf_idx < WIFI_BUF_SIZE - 1)
		{
			wf_idx++;
		}
		else
		{
			Process_WiFi_Data();
		}

		// 檢查是否收到完整回應 (以 "OK" 結尾)
		if (wf_idx >= 2 &&
			WFBuffer[wf_idx-2] == 'O' &&
			WFBuffer[wf_idx-1] == 'K')
		{
			Process_WiFi_Data();
		}

		HAL_UART_Receive_IT(&huart5, &WFBuffer[wf_idx], 1);

    }

    else if (huart->Instance == UART7) // GPS
    {
    	//HAL_UART_Transmit(&huart2, GPSBuffer, 1, 100);
    	HAL_UART_Receive_IT(&huart7, GPSBuffer, 1);
        static uint8_t rx_byte;
        rx_byte = GPSBuffer[0];

        if (rx_byte == '\n' || rx_byte == '\r')
        {
            if (idx > 0)
            {
                GPSBuffer[idx] = '\0';
                parse_gpgga(GPSBuffer);
                idx = 0;
            }
        }

        else if (rx_byte >= ' ' && rx_byte <= '~')
        {
            if (idx < sizeof(GPSBuffer) - 1)
            {
                GPSBuffer[idx++] = rx_byte;
            }

            else
            {
                idx = 0;
            }
        }
        HAL_UART_Receive_IT(&huart7, (uint8_t*)&GPSBuffer[0], 1);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{
		HAL_GPIO_TogglePin(STA_LED_GPIO_Port, STA_LED_Pin);
	}
	else if(htim->Instance == TIM2)
	{
		// WiFi 掃描
		#if DEBUG_MODE
			uint8_t text[] = "\r\n>>> Starting WiFi Scan <<<\r\n";
			HAL_UART_Transmit(&huart2, text, sizeof(text)-1, 100);
		#endif

		// 清空舊數據
		wf_idx = 0;
		wifi_count = 0;
		memset(WFBuffer, 0, WIFI_BUF_SIZE);
		memset(wifi_list, 0, sizeof(wifi_list));

		// 發送掃描指令
		uint8_t cmd[] = "AT+CWLAP\r\n";
		HAL_UART_Transmit(&huart5, cmd, sizeof(cmd)-1, 100);

		HAL_GPIO_TogglePin(WF_LED_GPIO_Port, WF_LED_Pin);
	}
}

void parse_gpgga(uint8_t *line)
{
	if (strncmp((const char *)line, "$GPGGA", 6) != 0 && strncmp((const char *)line, "$GNGGA", 6) != 0)
    {
        return;
    }

    int i = 0;
    char *token = strtok((char *)line, ",");

    while (token != NULL && i < 15) {
        size_t len = strlen(token);
        if (len > 15) len = 15;
        memcpy(parts[i], token, len);
        parts[i][len] = '\0';
        i++;
        token = strtok(NULL, ",");
    }
    if (i < 10)
    {
        return;
    }

    if (strlen(parts[6]) == 0 || parts[6][0] == '0')
    {
        return;
    }

    if (strlen(parts[2]) == 0) return;
    float rawLat = atof(parts[2]);
    if (rawLat == 0.0) return;

    int latDeg = (int)(rawLat / 100);
    float latMin = rawLat - latDeg * 100.0;
    float lat = latDeg + latMin / 60.0;

    if (strlen(parts[3]) > 0 && parts[3][0] == 'S')
    {
        lat = -lat;
    }

    if (strlen(parts[4]) == 0) return;
    float rawLon = atof(parts[4]);
    if (rawLon == 0.0) return;

    int lonDeg = (int)(rawLon / 100);
    float lonMin = rawLon - lonDeg * 100.0;
    float lon = lonDeg + lonMin / 60.0;

    if (strlen(parts[5]) > 0 && parts[5][0] == 'W') {
        lon = -lon;
    }

    float alt = (strlen(parts[9]) > 0) ? atof(parts[9]) : 0.0;

    int satellites = (strlen(parts[7]) > 0) ? atoi(parts[7]) : 0;

    float rawTime = (strlen(parts[1]) > 0) ? atof(parts[1]) : 0.0;
    uint8_t hour = ((int)rawTime / 10000) % 24;
    uint8_t minute = ((int)rawTime / 100) % 100;
    uint8_t second = (int)rawTime % 100;

    #if DEBUG_MODE
		char debug_msg[64];
		int len = snprintf(debug_msg, sizeof(debug_msg), "Lat:%.6f Lon:%.6f Alt:%.1fm Sat:%d\n%02d:%02d:%02d UTC\n", lat, lon, alt, satellites, hour, minute, second);

		if (len > 0 && len < sizeof(debug_msg))
		{
			HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, len, 1000);
		}
    #endif
}

void Handle_BT_Command(char* cmd)
{
    #if DEBUG_MODE
        char msg[64];
        int len = snprintf(msg, sizeof(msg), "\r\n[CMD] %s\r\n", cmd);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 100);
    #endif
}

void HM10_Process(uint8_t byte)
{
    // Debug: 顯示接收到的字符
    #if DEBUG_MODE
        HAL_UART_Transmit(&huart2, &byte, 1, 10);
    #endif

    // 加入緩衝區
    if (bt_idx < BT_BUF_SIZE - 1)
    {
        btrecv[bt_idx++] = byte;
        btrecv[bt_idx] = '\0';
    }
    else
    {
        // 滿了就重置
        bt_idx = 0;
        btrecv[0] = byte;
        btrecv[1] = '\0';
        bt_idx = 1;
    }

    // 檢查是否包含關鍵字（滑動窗口檢查）
    // 只檢查最後收到的部分

    // 方法：檢查緩衝區中是否出現 "CONN" 或 "LOST"
    if (strstr(btrecv, "CONN") != NULL)
    {
        // 連接成功
        HAL_GPIO_WritePin(BT_BLED_GPIO_Port, BT_BLED_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BT_RLED_GPIO_Port, BT_RLED_Pin, GPIO_PIN_SET);

        #if DEBUG_MODE
            uint8_t msg[] = "\r\n[BT] Connected\r\n";
            HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
        #endif

        // 清空
        bt_idx = 0;
        memset(btrecv, 0, BT_BUF_SIZE);
    }
    else if (strstr(btrecv, "LOST") != NULL)
    {
        // 連接斷開
        HAL_GPIO_WritePin(BT_BLED_GPIO_Port, BT_BLED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BT_RLED_GPIO_Port, BT_RLED_Pin, GPIO_PIN_RESET);

        #if DEBUG_MODE
            uint8_t msg[] = "\r\n[BT] Disconnected\r\n";
            HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
        #endif

        // 清空
        bt_idx = 0;
        memset(btrecv, 0, BT_BUF_SIZE);
    }

    // 處理用戶指令（換行結束）
    if (byte == '\n' || byte == '\r')
    {
        if (bt_idx > 1 && strstr(btrecv, "OK") == NULL)
        {
            // 不是 HM-10 狀態訊息，是用戶指令
            // 移除換行符
            while (bt_idx > 0 && (btrecv[bt_idx-1] == '\r' || btrecv[bt_idx-1] == '\n'))
            {
                btrecv[--bt_idx] = '\0';
            }

            if (bt_idx > 0)
            {
                Handle_BT_Command(btrecv);
            }
        }

        // 清空緩衝區
        bt_idx = 0;
        memset(btrecv, 0, BT_BUF_SIZE);
    }
}

void Parse_WiFi_Line(char* line)
{
    // 格式: +CWLAP:(3,"2F_Office_2.4G",-57,"58:11:22:61:d6:80",1,-29,0,4,4,7,1)
    //              ↑   ↑               ↑
    //              加密 SSID            RSSI

    char* ssid_start = strchr(line, '"');
    if (!ssid_start) return;
    ssid_start++; // 跳過第一個 "

    char* ssid_end = strchr(ssid_start, '"');
    if (!ssid_end) return;

    // 提取 SSID
    size_t ssid_len = ssid_end - ssid_start;
    if (ssid_len > 31) ssid_len = 31;

    char ssid[32];
    strncpy(ssid, ssid_start, ssid_len);
    ssid[ssid_len] = '\0';

    // 找到 RSSI (在第一個逗號之後)
    char* rssi_start = strchr(ssid_end, ',');
    if (!rssi_start) return;
    rssi_start++; // 跳過逗號

    int8_t rssi = atoi(rssi_start);

    // 檢查是否已存在
    for (int i = 0; i < wifi_count; i++)
    {
        if (strcmp(wifi_list[i].ssid, ssid) == 0)
        {
            // 更新 RSSI（取較強的）
            if (rssi > wifi_list[i].rssi)
            {
                wifi_list[i].rssi = rssi;
            }
            return;
        }
    }

    // 加入新的 WiFi
    if (wifi_count < MAX_WIFI_COUNT)
    {
        strncpy(wifi_list[wifi_count].ssid, ssid, 31);
        wifi_list[wifi_count].ssid[31] = '\0';
        wifi_list[wifi_count].rssi = rssi;
        wifi_count++;
    }
}

// 顯示 WiFi 列表
void Display_WiFi_List(void)
{
    if (wifi_count == 0)
    {
        #if DEBUG_MODE
            uint8_t msg[] = "No WiFi found\r\n";
            HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
        #endif
        return;
    }

    #if DEBUG_MODE
        char msg[128];
        int len = snprintf(msg, sizeof(msg), "\r\n=== WiFi Scan (%d networks) ===\r\n", wifi_count);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 100);

        for (int i = 0; i < wifi_count; i++)
        {
            len = snprintf(msg, sizeof(msg), "%d. %s: %d dBm\r\n",
                          i+1, wifi_list[i].ssid, wifi_list[i].rssi);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 100);
        }

        uint8_t end[] = "============================\r\n\r\n";
        HAL_UART_Transmit(&huart2, end, sizeof(end)-1, 100);
    #endif
}

// 處理 WiFi 數據
void Process_WiFi_Data(void)
{
    // 將緩衝區轉為字串
    WFBuffer[wf_idx] = '\0';

    // 解析每一行
    char* line_start = (char*)WFBuffer;
    char* line_end;

    while ((line_end = strchr(line_start, '\n')) != NULL)
    {
        *line_end = '\0'; // 結束當前行

        // 檢查是否為 CWLAP 回應
        if (strstr(line_start, "+CWLAP:") != NULL)
        {
            #if DEBUG_MODE
                char msg[128];
                int len = snprintf(msg, sizeof(msg), "WiFi: %s\r\n", line_start);
                HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 100);
            #endif

            Parse_WiFi_Line(line_start);
        }
        else if (strstr(line_start, "OK") != NULL)
        {
            // 掃描完成，顯示結果
            Display_WiFi_List();

            // 清空列表準備下次掃描
            wifi_count = 0;
            memset(wifi_list, 0, sizeof(wifi_list));
        }

        line_start = line_end + 1;
    }

    wf_idx = 0;
    memset(WFBuffer, 0, WIFI_BUF_SIZE);
}

void epd_ticket_handler(char *train_type, char *train_kind, char *train_num, char *date, char *dept_time,
						char *dept_sta, char *arr_time, char *arr_sta, char *car, char *seat, char *price)
{   //0 16 32 48 64 80 96 112 128 144 160 176 192 208 224 240 256 272 288 304
	//0 8 16 32 40 48 56 64 72 80 88 96 104 112 120 128 136 144 152 160
	HAL_GPIO_WritePin(EPD_EN_GPIO_Port, EPD_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	SSD1680_Clear(&hepd, ColorWhite);

	if (strcmp(train_type, "THSR") == 0)  SSD1680_Text(&hepd, 64, 0, "THSR", &cp866_8x16);
	else if (strcmp(train_type, "TRA") == 0)  SSD1680_Text(&hepd, 64, 0, "TRA ", &cp866_8x16);

	if (date != NULL)  SSD1680_Text(&hepd, 0, 16, date, &cp866_8x16);
	if (train_kind != NULL && strcmp(train_type, "TRA") == 0)  SSD1680_Text(&hepd, 88, 16, train_kind, &cp866_8x16);
	if (train_kind != NULL && (strcmp(train_type, "THSR") == 0)) SSD1680_Text(&hepd, 88, 16, train_kind, &cp866_8x8);
	if (train_num != NULL && (strcmp(train_type, "THSR") == 0))
	{
		if (train_num[0] == '0')  SSD1680_Text(&hepd, 88, 24, train_num + 1, &cp866_8x8);
		else  SSD1680_Text(&hepd, 88, 24, train_num, &cp866_8x8);
	}

	SSD1680_Text(&hepd, 88, 32, "Depart", &cp866_8x14);

	SSD1680_Text(&hepd, 88, 46, sta_code_decoder(dept_sta), &cp866_8x14);
	SSD1680_Text(&hepd, 88, 60, "Arrive", &cp866_8x14);
	SSD1680_Text(&hepd, 88, 72, sta_code_decoder(arr_sta), &cp866_8x14);

	SSD1680_Refresh(&hepd, REFRESH_MODE);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(EPD_EN_GPIO_Port, EPD_EN_Pin, GPIO_PIN_RESET);
}

void debug_disp(void)
{
	HAL_GPIO_WritePin(EPD_EN_GPIO_Port, EPD_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	SSD1680_Text(&hepd,  0, 280, "Predict:", &cp866_8x16);
	char predict_str[12];
	sprintf(predict_str, "%d", result);
	SSD1680_Text(&hepd,  72, 280, predict_str, &cp866_8x16);
	SSD1680_Refresh(&hepd, REFRESH_MODE);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(EPD_EN_GPIO_Port, EPD_EN_Pin, GPIO_PIN_RESET);
}

char* sta_code_decoder(char *sta_code)
{
	int sta_code_int = atoi(sta_code);
	switch (sta_code_int)
	{
	    case 1:
	        return "Nangang";
	        break;
	    case 2:
	        return "Taipei";
	        break;
	    case 3:
	        return "Banqiao";
	        break;
	    case 4:
	        return "Taoyuan";
	        break;
	    case 5:
	        return "Hsinchu";
	        break;
	    case 6:
	        return "Miaoli";
	        break;
	    case 7:
	        return "Taichung";
	        break;
	    case 8:
	        return "Changhua";
	        break;
	    case 9:
	        return "Yunlin";
	        break;
	    case 10:
	        return "Chiayi";
	        break;
	    case 11:
	        return "Tainan";
	        break;
	    case 12:
	        return "Zuoying";
	        break;
	    default:
	        return sta_code;
	        break;
	}
}

void relu(float *data, int size) {
    for (int i = 0; i < size; i++) {
        if (data[i] < 0.0f) data[i] = 0.0f;
    }
}

void softmax(float *data, int size) {
    float max_val = data[0];
    for (int i = 1; i < size; i++) if (data[i] > max_val) max_val = data[i];
    float sum = 0.0f;
    for (int i = 0; i < size; i++) {
        data[i] = expf(data[i] - max_val);
        sum += data[i];
    }
    for (int i = 0; i < size; i++) data[i] /= sum;
}


int mlp_forward_pass(double current_lat, double current_lon, uint8_t hour_of_day, uint16_t min_of_day, uint8_t day_of_week, int time_to_dept, uint8_t is_ticket_reg, uint8_t is_entering, uint8_t is_exiting)
{
    float x[INPUT_DIM];
    x[0] = current_lat;
    x[1] = current_lon;
    x[2] = (float)hour_of_day;
    x[3] = (float)min_of_day;
    x[4] = (float)day_of_week;
    x[5] = (float)time_to_dept;
    x[6] = (float)is_ticket_reg;
    x[7] = (float)is_entering;
    x[8] = (float)is_exiting;

    float h1[HIDDEN_1];
    float h2[HIDDEN_2];
    float out[OUTPUT_DIM];

    // 2. 第一層前向傳播 (9 -> 64)
    for (int i = 0; i < HIDDEN_1; i++) {
        float sum = fc1_bias[i];
        for (int j = 0; j < INPUT_DIM; j++) {
            sum += x[j] * fc1_weight[i][j];
        }
        h1[i] = sum;
    }
    relu(h1, HIDDEN_1);

    // 3. 第二層前向傳播 (64 -> 32)
    for (int i = 0; i < HIDDEN_2; i++) {
        float sum = fc2_bias[i];
        for (int j = 0; j < HIDDEN_1; j++) {
            sum += h1[j] * fc2_weight[i][j];
        }
        h2[i] = sum;
    }
    relu(h2, HIDDEN_2);

    // 4. 第三層前向傳播 (32 -> 5)
    for (int i = 0; i < OUTPUT_DIM; i++) {
        float sum = fc3_bias[i];
        for (int j = 0; j < HIDDEN_2; j++) {
            sum += h2[j] * fc3_weight[i][j];
        }
        out[i] = sum;
    }

    // 5. 使用 Softmax 轉換為機率並挑選最大值
    softmax(out, OUTPUT_DIM);

    int best_class = 0;
    float max_p = out[0];
    for (int i = 1; i < OUTPUT_DIM; i++) {
        if (out[i] > max_p) {
            max_p = out[i];
            best_class = i;
        }
    }

    return best_class; // 回傳預測類別 (0~4)
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {

  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
