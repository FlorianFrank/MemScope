/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */

#include <cpp/Devices/STM32F429Wrapper.h>
#include <cpp/MemoryModules/FRAM_Rohm_MR48V256CTAZAARL.h>
#include <cpp/MemoryControllers/MemoryControllerParallel.h>
#include "cpp/Main/Main.hpp"
#include <SystemFiles/gpio.h>
#include <SystemFiles/fmc.h>
#include "cpp/uart_frame_protocol/Frame.h"
#include "cpp/uart_frame_protocol/JsonParser.h"

bool running = true;

using namespace std;

FMC_NORSRAM_TimingTypeDef T;
extern FMC_NORSRAM_TimingTypeDef Timing;

uint32_t globalDataSetupTime ;
uint32_t globalAddressSetupTime ;

/**
  * @brief  The application entry point.
  * @retval int
  */
#define RxBuffer_SIZE   400

// Buffers:
uint8_t RxBuffer[RxBuffer_SIZE];  // RxBuffer is where the DMA is going to copy the data
uint8_t TxBuffer[68] = {85, 61, 123, 34, 107, 101, 121, 34, 32, 58, 32, 34, 49, 50, 51, 34, 44, 32, 34, 110, 97, 109, 101, 34, 58, 34, 83, 84, 77, 51, 50, 70, 52, 50, 57, 34, 44, 32, 34, 101, 120, 116, 101, 114, 110, 97, 108, 77, 101, 109, 111, 114, 121, 34, 58, 32, 34, 70, 82, 65, 77, 34, 125, 67, 189, 7, 19, 170}; //stm32

HAL_StatusTypeDef hal_status;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;


/* Private function prototypes -----------------------------------------------*/
static void MX_DMA_Init(void);
void MX_USART1_UART_Init(void);

STM32F429Wrapper device;
FRAM_Rohm_MR48V256CTAZAARL fram;

int main(){

#if STM32
    //STM32F429Wrapper device;
    device.Initialize();
#endif
    //FRAM_Rohm_MR48V256CTAZAARL fram;

    //MemoryControllerParallel memoryController(nullptr, fram, device);
    //memoryController.Initialize();


    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    //MX_FMC_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();

    //call uart receive function then the data will be stored in the RxBuffer
    hal_status = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxBuffer, RxBuffer_SIZE);

    // Reading STM32F429 unique device ID, The UUID is stored in memory at address 0x1FFF7A10
    uint32_t idPart1 = STM32_UUID[0];
    uint32_t idPart2 = STM32_UUID[1];
    uint32_t idPart3 = STM32_UUID[2];

    // do something with the overall 96 bits
    printf("idPart1: 0x%08X\n", idPart1);
    printf("idPart2: 0x%08X\n", idPart2);
    printf("idPart3: 0x%08X\n", idPart3);

    while(1){

    }
}// End ma


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
    // WRITING Frame
    Frame writing_frame(WRITING_STATE);
    vector<uint8_t> writing_frame_bytes = writing_frame.getBytes();
    uint8_t *writing_frame_buffer = &writing_frame_bytes[0];

    // END WRITING Frame
    Frame end_writing_frame(END_WRITING_STATE);
    vector<uint8_t> end_writing_frame_bytes = end_writing_frame.getBytes();
    uint8_t *end_writing_frame_buffer = &end_writing_frame_bytes[0];

    // READING Frame
    Frame reading_frame(READING_STATE);
    vector<uint8_t> reading_frame_bytes = reading_frame.getBytes();
    uint8_t *reading_frame_buffer = &reading_frame_bytes[0];

    // END READING Frame
    Frame end_reading_frame(END_READING_STATE);
    vector<uint8_t> end_reading_frame_bytes = end_reading_frame.getBytes();
    uint8_t *end_reading_frame_buffer = &end_reading_frame_bytes[0];

    // FINISH Frame
    Frame finish_frame(FINISH);
    vector<uint8_t> finish_frame_bytes = finish_frame.getBytes();
    uint8_t *finish_frame_buffer = &finish_frame_bytes[0];

    uint32_t startAddress ;
    uint32_t stopAddress ;
    uint8_t initialValue;

    if(huart == &huart1) {
        cout << "RECEIVE " << endl;
        char received_payload[Size + 1];
        memcpy(received_payload, RxBuffer, Size);
        received_payload[Size] = '\0';

        cout << "> Size: " << Size<< endl;
        cout << "> Received message: " << received_payload << endl;

        std::string jsonString = received_payload;
        auto keyValues = JsonParser::parse(jsonString);





        //device.Initialize();
        MemoryControllerParallel memoryController(nullptr, fram, device);
        memoryController.Initialize();

        if (keyValues["HEADER"] == "*IDN?") {
            Frame frame(idn_payload);
            vector<uint8_t> TxIDN = frame.getBytes();
            uint8_t *raw_frame_buffer = &TxIDN[0];
            hal_status = HAL_UART_Transmit(&huart1, raw_frame_buffer, TxIDN.size(), 100);
        }else if (keyValues["HEADER"] == "TEST"){

            stringstream ss_start(keyValues["startAddress"]);
            ss_start >> startAddress;

            stringstream ss_stop(keyValues["stopAddress"]);
            ss_stop >> stopAddress;
            cout << "> start stop address: " << startAddress << " -> " << stopAddress << endl;

            initialValue = stoi(keyValues["initialValue"], nullptr, 16);
            printf("> Initial Value = %x\n", initialValue);

            uint8_t retValue = 0x00;

            if(keyValues["testType"] == "reliabilityTest"){
                //globalDataSetupTime = 15;
                globalAddressSetupTime = 10;
                device.Initialize();
                MemoryControllerParallel memoryController(nullptr, fram, device);
                memoryController.Initialize();

                stringstream ss(keyValues["dataSetupTime"]);
                ss >> globalDataSetupTime;

                MX_FMC_Init();

                printf("START WRITING\n");
                HAL_UART_Transmit(&huart1, writing_frame_buffer, writing_frame_bytes.size(),100);

                for (uint32_t i = startAddress; i < stopAddress; i++)
                {
                    printf("inside for loop  %u \n", i);
                    memoryController.Write8BitWord(i, initialValue);
                }
                HAL_UART_Transmit(&huart1, end_writing_frame_buffer, end_writing_frame_bytes.size(),100);

                //device.Initialize();
                printf("START READING\n");

                //fram.GetMemorySize()
                HAL_UART_Transmit(&huart1, reading_frame_buffer, reading_frame_bytes.size(),100);

                for (uint32_t i = startAddress ; i < stopAddress; i++)
                {
                    memoryController.Read8BitWord(i, &retValue);
                    printf("%x;%x\n", i , retValue);

                    vector<uint32_t> data;
                    if(retValue != initialValue){
                        data = {0x00};
                    }else{
                        data = {0x01};
                    }

                    vector<uint8_t> raw_frame;
                    raw_frame.push_back(0xab);
                    raw_frame.push_back(0xef);
                    raw_frame.push_back(data.size());
                    raw_frame.push_back(retValue);
                    raw_frame.push_back(0xe1);
                    raw_frame.push_back(0xe2);
                    uint8_t *raw_frame_buffer = &raw_frame[0];

                    HAL_UART_Transmit(&huart1, raw_frame_buffer, raw_frame.size(), 100);
                }
                // Send end reading from the fram
                HAL_UART_Transmit(&huart1, end_reading_frame_buffer,end_reading_frame_bytes.size() , 100);
            }

            else if(keyValues["testType"] == "readLatencyTest"){
                cout << "\n*** START Read Latency Test ***\n" << endl;
                cout << ">>> WRITING with default dataSetupTime \n" << endl;

                HAL_UART_Transmit(&huart1, writing_frame_buffer, writing_frame_bytes.size(),100);

                globalAddressSetupTime = 10;
                globalDataSetupTime = 15;

                /* FMC initialization */
                MX_FMC_Init();

                for (uint32_t i = startAddress ; i < stopAddress ; i++)
                {
                    memoryController.Write8BitWord(i, initialValue);
                }

                HAL_UART_Transmit(&huart1, end_writing_frame_buffer, end_writing_frame_bytes.size(),100);

                //globalDataSetupTime = stoi(keyValues["dataSetupTime"]);
                stringstream ss(keyValues["dataSetupTime"]);
                ss >> globalDataSetupTime;
                cout << ">>> READING with reduced DataSetupTime: " << globalDataSetupTime << endl;
                MX_FMC_Init();

                HAL_UART_Transmit(&huart1, reading_frame_buffer, reading_frame_bytes.size(),100);
                // fram.GetMemorySize()

                for (uint32_t i = startAddress; i < stopAddress; i++){
                    memoryController.Read8BitWord(i, &retValue);
                    //printf("%x;%x\n", i , retValue);
                    vector<uint32_t> data;
                    if(retValue != 0x55){
                        data = {0x00};
                    }else{
                        data = {0x01};
                    }
                    // build a frame
                    vector<uint8_t> raw_frame;
                    raw_frame.push_back(0xab);
                    raw_frame.push_back(0xef);
                    raw_frame.push_back(data.size());
                    raw_frame.push_back(retValue);
                    raw_frame.push_back(0xe1);
                    raw_frame.push_back(0xe2);
                    uint8_t *raw_frame_buffer = &raw_frame[0];

                    HAL_UART_Transmit(&huart1, raw_frame_buffer, raw_frame.size(), 100);
                }
                // Send end reading from the fram
                HAL_UART_Transmit(&huart1, end_reading_frame_buffer,end_reading_frame_bytes.size() , 100);
            }

            else if(keyValues["testType"] == "writeLatencyTest"){
                // TODO
                printf("*** START Write Latency Test ***\n\n");
            }else if(keyValues["testType"] == "rowHammingTest"){
                // TODO
                printf("*** START Row Hamming Test ***\n\n");
            }else{
                printf("*** Test Not Defined ***\n\n");
            }

            HAL_UART_Transmit(&huart1, finish_frame_buffer, finish_frame_bytes.size(),100);
        }

        else {
            printf("WARNING: HEADER NOT DEFINED ***\n\n");
            HAL_UART_Transmit(&huart1, finish_frame_buffer, finish_frame_bytes.size(),100);
        }

        printf("*** END IF ***\n\n");
        // set all elements of the buffer to 0
        memset(TxBuffer, 0, sizeof TxBuffer);

        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxBuffer, RxBuffer_SIZE);

        // Disable Half of Data Callback
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
        // After the callback the DMA will stop && we must start it again (keep listening to any command)
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