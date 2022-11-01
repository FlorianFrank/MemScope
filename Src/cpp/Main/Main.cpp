/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */

#include <cpp/Devices/STM32F429Wrapper.h>
#include <cpp/MemoryModules/FRAM_Rohm_MR48V256CTAZAARL.h>
#include <cpp/MemoryControllers/MemoryControllerParallel.h>
#include "cpp/Main/Main.hpp"
#include <SystemFiles/gpio.h>


bool running = true;

/**
  * @brief  The application entry point.
  * @retval int
  */
#define RxBuffer_SIZE   10
#define TxBuffer_SIZE   68

// Buffers:
uint8_t RxBuffer[RxBuffer_SIZE];  // RxBuffer is where the DMA is going to copy the data
uint8_t TxBuffer[68] = {85, 61, 123, 34, 107, 101, 121, 34, 32, 58, 32, 34, 49, 50, 51, 34, 44, 32, 34, 110, 97, 109, 101, 34, 58, 34, 83, 84, 77, 51, 50, 70, 52, 50, 57, 34, 44, 32, 34, 101, 120, 116, 101, 114, 110, 97, 108, 77, 101, 109, 111, 114, 121, 34, 58, 32, 34, 70, 82, 65, 77, 34, 125, 67, 189, 7, 19, 170}; //stm32

HAL_StatusTypeDef hal_status;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* Private function prototypes -----------------------------------------------*/
static void MX_DMA_Init(void);
void MX_USART1_UART_Init(void);

int main()
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();/*
    #if STM32
        STM32F429Wrapper device;
        device.Initialize();
    #endif
        FRAM_Rohm_MR48V256CTAZAARL fram;

    MemoryControllerParallel memoryController(nullptr, fram, device);
    memoryController.Initialize();*/

    hal_status = HAL_UART_Transmit(&huart1, TxBuffer, TxBuffer_SIZE, 100);
    printf("hal_status %d\n", hal_status);

    //call uart receive function then the data will be stored in the RxBuffer
    hal_status = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxBuffer, RxBuffer_SIZE);
    printf("hal_status %d\n", hal_status);
    // Disable Half of Data Callback
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);

    printf("START\n");
    /*for (uint32_t i = 0; i < fram.GetMemorySize(); i++)
    {
        memoryController.Write8BitWord(i, 0xFF);
        HAL_Delay(1);

    }

    printf("START READING\n");

    uint8_t retValue = 0x00;
    for (uint32_t i = 0; i < fram.GetMemorySize(); i++)
    {
        memoryController.Read8BitWord(i, &retValue);
    }

    int ctr = 0;
    if(retValue != 0xFF)
    {
        ctr++;
    }

    printf("CTR %d\n", ctr);
*/


    while(1){}
}// End main

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
    if(huart == &huart1) {
        uint8_t retValue = 0x00;

        for (uint32_t i = 0; i < 5; i++) {
            HAL_UART_Transmit(&huart1, &retValue, sizeof(retValue),100);
            retValue++;
            retValue++;
        }
        // After the callback the DMA will stop and we must start it again (keep listening to any command)
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxBuffer, RxBuffer_SIZE);
        // Disable Half of Data Callback
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    }
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    /* DMA2_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

void MX_USART1_UART_Init(void)
{

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        printf("Error in \n");
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */
}