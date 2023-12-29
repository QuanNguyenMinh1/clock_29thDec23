/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "ILI9225.h"
#include "stdio.h"
#include "stdint.h"
#include <string.h>
#include "RTC.h"
#include "button.h"
#include "DHT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */
uint8_t second_present, min_present, hour_present, date_present, month_present, year_present;
uint8_t second_temp, min_temp, hour_temp, date_temp, month_temp, year_temp;
uint8_t date_max;
char temperature_string[20];
char humidity_string[20];
float Temperature;
float Humidity;
char str1[20];
char str2[20];

DHT_DataTypedef DHT11_Data;
//Button_TypeDef;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
DateTime_t datetime;
void btn_adjust_up(void);
void btn_adjust_down(void);
void Read_DataDHT(void)
{
	DHT_GetData(&DHT11_Data);
	Temperature = DHT11_Data.Temperature;
	Humidity = DHT11_Data.Humidity;
}
Button_TypeDef buttonmode;
Button_TypeDef buttonup;
Button_TypeDef buttondown;
Button_TypeDef *ButtonX;

typedef enum
{
	BINHTHUONG = 0,
	CHINHPHUT = 1,
	CHINHGIO = 2,
	CHINHNGAY = 3,
}Mode;

Mode mode; // khai báo là biến "mode" có kiểu dữ liệu "Mode"

//////////////////////////////////////////////////////////////CHƯƠNG TRÌNH PHỤC VỤ NGẮT//////////////////////////////////////////////////////////////
void btn_pressing_callback()
{
	if (ButtonX->GPIO_Pin == GPIO_PIN_0)
	{
		switch (mode)
		{
		case BINHTHUONG: // mode = 0
	  datetime.date = date_temp; //gán th�?i gian vừa cài đặt xong cho biến official (biến mà RTC sẽ tự động cập nhật)
			mode = CHINHPHUT;  // gán mode 0 thành 1
			break; // đang ở case BINHTHUONG thì chương trình sẽ gán cho biến mode (dòng 84), sau đó break ra kh�?i switch, rồi khi này chương trình sẽ không thực hiện lệnh gì trong khi đợi đi�?u kiện của 1 trong 3 cái if ở ngắt là đúng;
		// Vậy lúc này, nếu bấm button_mode một lần nữa thì chương trình sẽ xét switch rồi nhảy vô case CHINHGIO vì biến "mode" đã được gán = 2 ở dòng 88; Else nhấn button_up một lần thì chương trình
		// sẽ nhảy vào chạy hàm btn_adjust(up) hoặc nếu nhấn button_down thì chương trình sẽ nhảy vào chạy hàm btn_adjust(down).
		case CHINHPHUT: // mode = 1
			mode = CHINHGIO; // gán mode = 2
			break;
		case CHINHGIO: // mode = 2
	  datetime.min = min_temp; //gán th�?i gian vừa cài đặt xong cho biến official (biến mà RTC sẽ tự động cập nhật)
			mode = CHINHNGAY; // gán mode = 3
			break;
		case CHINHNGAY:      // mode = 3
	  datetime.hour = hour_temp; //gán th�?i gian vừa cài đặt xong cho biến official (biến mà RTC sẽ tự động cập nhật)
			mode = BINHTHUONG; // gán mode = 0
			break;
		}
	}
  ////////////////BUTTON_UP///////////////
	if (ButtonX->GPIO_Pin == GPIO_PIN_1)
	{
		btn_adjust_up();
//		void btn_adjust(up)
//			{
//			switch(mode)
//				{
//				case CHINHPHUT:
//					// viet code de cong 1 phut va khi den 60 thi quay lai
//		            min_temp = datetime.min; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
//		        while(1){
//		            fill_rectangle(110, 200, 230, 210, COLOR_WHITE); // blink tại thông số "minute"
//		            if (ButtonX->GPIO_Pin == GPIO_PIN_1)
//		            {
//		            min_temp ++;
//		            if (min_temp == 60)
//		            {
//		                min_temp = 0;
//		            }
//				    draw_string(40, 200, COLOR_BLACK, 2, str1);
//		            }
//		        }
//					 break;
//				case CHINHGIO:
//					// viet code de cong 1 gio va khi den 60 thi quay lai
//		            hour_temp = datetime.hour; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
//		        while(1){
//		            fill_rectangle(110, 200, 230, 210, COLOR_WHITE); // blink tại thông số "hour"
//		            if (ButtonX->GPIO_Pin == GPIO_PIN_1)
//		            {
//		            hour_temp ++;
//		            if (hour_temp == 24)
//		            {
//		                hour_temp = 0;
//		            }
//				    draw_string(40, 200, COLOR_BLACK, 2, str1);
//		            }
//		        }
//					 break;
//				case CHINHNGAY:
//					// viet code de cong 1 ngay va biet duoc so ngay trong thang de quay lai
//		            date_temp = datetime.date; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
//		        while(1){
//		            fill_rectangle(85, 215, 205, 235, COLOR_WHITE); // Blink tại thông số "date"
//		            if (ButtonX->GPIO_Pin == GPIO_PIN_1)
//		            {
//		            date_temp ++;
//		            switch (datetime.month)   // xét xem tháng hiện tại có tổng bao nhiêu ngày
//		            {
//		            case 1: case 3: case 5:case 7: case 8: case 10: case 12:
//		                date_max = 31 + 1;
//		                break;
//		            case 4: case 6: case 9: case 11:
//		                date_max = 30 + 1;
//		            case 2:
//		                if ((datetime.year - 2014) % 4 == 0)
//		            {
//		                date_max = 29 + 1;
//		            }
//		            else
//		            {
//		                date_max = 28 + 1;
//		            }
//		                break;
//		            default:
//		                break;
//		            }
//		            if (date_temp == date_max)
//		            {
//		                date_temp = 1;
//		            }
//				    draw_string(40, 220, COLOR_BLACK, 2, str2); // in cả string 2 (nhưng chỉ có thông số date thay đổi).
//		            }
//		        }
//					 break;
//				}
	}

  ///////////////BUTTON_DOWN////////////
	if (ButtonX->GPIO_Pin == GPIO_PIN_2)
	{
	  btn_adjust_down();
//		void btn_adjust(down)
//			{
//		    	switch(mode)
//				{
//				case CHINHPHUT:
//					// viet code de tru 1 phut va khi den 0 thi quay lai
//		            min_temp = datetime.min; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
//		        while(1){
//		            fill_rectangle(110, 200, 230, 210, COLOR_WHITE); // blink tại thông số "minute"
//		            if (ButtonX->GPIO_Pin == GPIO_PIN_1)
//		            {
//		            min_temp --;
//		            if (min_temp == -1)
//		            {
//		                min_temp = 59;
//		            }
//				    draw_string(40, 200, COLOR_BLACK, 2, str1);
//		            }
//		        }
//					 break;
//				case CHINHGIO:
//					// viet code de tru 1 gio va khi den 0 thi quay lai
//		            hour_temp = datetime.hour; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
//		        while(1){
//		            fill_rectangle(110, 200, 230, 210, COLOR_WHITE); // blink tại thông số "hour"
//		            if (ButtonX->GPIO_Pin == GPIO_PIN_1)
//		            {
//		            hour_temp --;
//		            if (hour_temp == -1)
//		            {
//		                hour_temp = 23;
//		            }
//				    draw_string(40, 200, COLOR_BLACK, 2, str1);
//		            }
//		        }
//					 break;
//				case CHINHNGAY:
//					// viet code de tru 1 ngay va biet duoc so ngay trong thang de quay lai
//		            date_temp = datetime.date; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
//		        while(1){
//		            fill_rectangle(85, 215, 205, 235, COLOR_WHITE); // Blink tại thông số "date"
//		            if (ButtonX->GPIO_Pin == GPIO_PIN_1)
//		            {
//		            date_temp --;
//		            switch (datetime.month)   // xét xem tháng hiện tại có tổng bao nhiêu ngày
//		            {
//		            case 1: case 3: case 5:case 7: case 8: case 10: case 12:
//		                date_max = 31 + 1;
//		                break;
//		            case 4: case 6: case 9: case 11:
//		                date_max = 30 + 1;
//		            case 2:
//		                if ((datetime.year - 2014) % 4 == 0) // nếu năm hiện tại là năm nhuận thì số tổng số ngày của tháng 2 ở năm hiện tại là 29, còn không thì là 28.
//		            {
//		                date_max = 29 + 1;
//		            }
//		            else
//		            {
//		                date_max = 28 + 1;
//		            }
//		                break;
//		            default:
//		                break;
//		            }
//		            if (date_temp == 0)
//		            {
//		                date_temp = date_max - 1;
//		            }
//				    draw_string(40, 220, COLOR_BLACK, 2, str2); // in cả string 2 (nhưng chỉ có thông số date thay đổi).
//		            }
//		        }
//					 break;
//				}
//			}
//
//		}
	}
}

//                    Tạo hàm sẽ chạy khi nhấn button_up
void btn_adjust_up (void)
{
	switch(mode)
	{
		case CHINHPHUT:
			// viet code de cong 1 phut va khi den 60 thi quay lai
			min_temp = datetime.min; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
			while(1){
				fill_rectangle(110, 200, 230, 210, COLOR_WHITE); // blink tại thông số "minute"
				if (ButtonX->GPIO_Pin == GPIO_PIN_1)
				{
				min_temp ++;
				if (min_temp == 60)
				{
					min_temp = 0;
				}
				draw_string(40, 200, COLOR_BLACK, 2, str1);
				}
			}
			break;
		case CHINHGIO:
			// viet code de cong 1 gio va khi den 60 thi quay lai
			hour_temp = datetime.hour; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
			while(1){
				fill_rectangle(110, 200, 230, 210, COLOR_WHITE); // blink tại thông số "hour"
				if (ButtonX->GPIO_Pin == GPIO_PIN_1)
				{
				hour_temp ++;
				if (hour_temp == 24)
				{
					hour_temp = 0;
				}
				draw_string(40, 200, COLOR_BLACK, 2, str1);
				}
			}
			 break;
		case CHINHNGAY:
			// viet code de cong 1 ngay va biet duoc so ngay trong thang de quay lai
			date_temp = datetime.date; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
			while(1)
			{
				fill_rectangle(85, 215, 205, 235, COLOR_WHITE); // Blink tại thông số "date"
				if (ButtonX->GPIO_Pin == GPIO_PIN_1)
				{
					date_temp ++;
					switch (datetime.month)   // xét xem tháng hiện tại có tổng bao nhiêu ngày
					{
						case 1:
						case 3:
						case 5:
						case 7:
						case 8:
						case 10:
						case 12:
							date_max = 31 + 1;
							break;
						case 4:
						case 6:
						case 9:
						case 11:
							date_max = 30 + 1;
						case 2:
							if ((datetime.year - 2014) % 4 == 0)
							{
								date_max = 29 + 1;
							}
							else
							{
								date_max = 28 + 1;
							}
							break;
						default:
							break;
					}
					if (date_temp == date_max)
					{
						date_temp = 1;
					}
					draw_string(40, 220, COLOR_BLACK, 2, str2); // in cả string 2 (nhưng chỉ có thông số date thay đổi).
				}
			}
			break;
	}
}
//                       Tạo hàm sẽ chạy khi nhấn button_down
void btn_adjust_down(void)
{
	switch(mode)
	{
		case BINHTHUONG:
			break;
		case CHINHPHUT:
			// viet code de tru 1 phut va khi den 0 thi quay lai
			min_temp = datetime.min; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
			while(1){
				fill_rectangle(110, 200, 230, 210, COLOR_WHITE); // blink tại thông số "minute"
				if (ButtonX->GPIO_Pin == GPIO_PIN_1)
				{
					min_temp --;
					if (min_temp == -1)
					{
						min_temp = 59;
					}
					draw_string(40, 200, COLOR_BLACK, 2, str1);
				}
			}
			break;
		case CHINHGIO:
			// viet code de tru 1 gio va khi den 0 thi quay lai
			hour_temp = datetime.hour; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
			while(1){
				fill_rectangle(110, 200, 230, 210, COLOR_WHITE); // blink tại thông số "hour"
				if (ButtonX->GPIO_Pin == GPIO_PIN_1)
				{
					hour_temp --;
					if (hour_temp == -1)
					{
						hour_temp = 23;
					}
					draw_string(40, 200, COLOR_BLACK, 2, str1);
				}
			}
			break;
		case CHINHNGAY:
			// viet code de tru 1 ngay va biet duoc so ngay trong thang de quay lai
			date_temp = datetime.date; // gán giá trị của "biến official" (RTC đang dùng để cập nhật) cho biến temporary.
			while(1)
			{
				fill_rectangle(85, 215, 205, 235, COLOR_WHITE); // Blink tại thông số "date"
				if (ButtonX->GPIO_Pin == GPIO_PIN_1)
				{
					date_temp --;
					switch (datetime.month)   // xét xem tháng hiện tại có tổng bao nhiêu ngày
					{
						case 1:
						case 3:
						case 5:
						case 7:
						case 8:
						case 10:
						case 12:
							date_max = 31 + 1;
							break;
						case 4:
						case 6:
						case 9:
						case 11:
							date_max = 30 + 1;
						case 2:
							if ((datetime.year - 2014) % 4 == 0) // nếu năm hiện tại là năm nhuận thì số tổng số ngày của tháng 2 ở năm hiện tại là 29, còn không thì là 28.
							{
								date_max = 29 + 1;
							}
							else
							{
								date_max = 28 + 1;
							}
							break;
						default:
							break;
					}
					if (date_temp == 0)
					{
						date_temp = date_max - 1;
					}
					draw_string(40, 220, COLOR_BLACK, 2, str2); // in cả string 2 (nhưng chỉ có thông số date thay đổi).
				}
			}
			break;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

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
  MX_SPI2_Init();
  MX_I2C2_Init();
  DHT_Start();
  /* USER CODE BEGIN 2 */
	int mode = 0; // Cho chương trình chạy ở MODE 0 trước tiên khi mới bắt đầu.
	button_init(&buttonmode, GPIOA, GPIO_PIN_0);
	button_init(&buttonup, GPIOA, GPIO_PIN_1);
	button_init(&buttondown, GPIOA, GPIO_PIN_2);
	lcd_init();
	fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_WHITE);
	draw_string(0, 50, COLOR_GREENYELLOW, 5, "HELLO");
	HAL_Delay(2500);
	fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_WHITE);

    // Cài đặt th�?i gian ban đầu
	datetime.second = 15;
	datetime.min = 44;
	datetime.hour = 12;
	datetime.day = 8;
	datetime.date = 58;
	datetime.month = 12;
	datetime.year = 23;
	RTC_WriteTime(&datetime);

	draw_string(20, 50, COLOR_GOLD, 5, "QMDP");
	HAL_Delay(2500);
//  fill_rectangle(0, 0, 276, 220, COLOR_WHITE);
	fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_WHITE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	button_handle(&buttonmode);
	button_handle(&buttonup);
	button_handle(&buttondown);

	 RTC_ReadTime(&datetime);
	  second_temp = datetime.second;
	  min_temp = datetime.min;
	  hour_temp = datetime.hour;
	  date_temp = datetime.date;
	  month_temp = datetime.month;
	  year_temp = datetime.year;
	  sprintf(str1, "Time %.2d:%.2d:%.2d", datetime.hour, datetime.min, datetime.second);


	  if(second_temp != second_present)
	  {
		  fill_rectangle(140, 100, 160, 110, COLOR_WHITE);
		  second_present = second_temp;
	  }
	  draw_string(40, 100, COLOR_BLACK, 1, str1);

	  if(min_temp != min_present)
	  	  {
	  		  fill_rectangle(110, 100, 130, 110, COLOR_WHITE);
	  		  min_present = min_temp;
	  	  }
	  draw_string(40, 100, COLOR_BLACK, 1, str1);

	  if(hour_temp != hour_present)
	 	  	  {
	 	  		  fill_rectangle(85, 100, 105, 110, COLOR_WHITE);
	 	  		  hour_present = hour_temp;
	 	  	  }
	  draw_string(40, 100, COLOR_BLACK, 1, str1);

	  sprintf(str2,"Date %.2d/%.2d/%.2d",datetime.date,datetime.month,datetime.year );
// để sprintf ở đây cho nó chính xác, date, month, year lâu lắm
	  //để sprintf ở đây để convert, lệnh in lên LCD nằm trong thư viện của LCD
	  if(date_temp != date_present)
	  	 	  	  {
	  	 	  		  fill_rectangle(85, 115, 105, 135, COLOR_WHITE);
	  	 	  		  date_present = date_temp;
	  	 	  	  }
	  	  draw_string(40, 120, COLOR_BLACK, 1, str2);

	  	if(month_temp != month_present)
	  		 	  	  {
	  		 	  		  fill_rectangle(110, 115, 130, 135, COLOR_WHITE);
	  		 	  		  month_present = month_temp;
	  		 	  	  }
	  		  draw_string(40, 120, COLOR_BLACK, 1, str2);

	  		if(year_temp != year_present)
	  			 	  	  {
	  			 	  		  fill_rectangle(135, 115, 160, 135, COLOR_WHITE);
	  			 	  		  year_present = year_temp;
	  			 	  	  }
	  			  draw_string(40, 120, COLOR_BLACK, 1, str2);
//

//		  			          Read_DataDHT();
//		  				      snprintf(temperature_string, 7, "%.2f", Temperature);
//		  				      snprintf(humidity_string, 7, "%.2f", Humidity);
//		  				      fill_rectangle(0, 0, 220, 176, COLOR_WHITE);
//		  				      draw_string(20, 20, COLOR_BLUE, 4, temperature_string);
//		  					  draw_string(20, 90, COLOR_BLUE, 4, humidity_string);
//		  					  HAL_Delay(2000);
////
     snprintf(temperature_string, 7, "%.2f", Temperature);
     snprintf(humidity_string, 7, "%.2f", Humidity);
     fill_rectangle(150, 20, 220, 50, COLOR_WHITE);
//          Read_DataDHT();
//	      draw_string(20, 20, COLOR_GOLD, 1, "TEMP");
//	      draw_string(20, 40, COLOR_GOLD, 1, "HUM");
////
     draw_string(150, 20, COLOR_BLUE, 1, temperature_string);
	  draw_string(150, 40, COLOR_BLUE, 1, humidity_string);
////////	  	  DHT_GetData(&dht11);
 	  Read_DataDHT();
  /* USER CODE END 3 */
  }
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OUT_DHT11_Pin|CMD_Pin|CS_Pin|RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BUTTON_MODE_Pin BUTTON_UP_Pin BUTTON_DOWN_Pin */
  GPIO_InitStruct.Pin = BUTTON_MODE_Pin|BUTTON_UP_Pin|BUTTON_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT_DHT11_Pin CMD_Pin CS_Pin RST_Pin */
  GPIO_InitStruct.Pin = OUT_DHT11_Pin|CMD_Pin|CS_Pin|RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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


#ifdef  USE_FULL_ASSERT
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
