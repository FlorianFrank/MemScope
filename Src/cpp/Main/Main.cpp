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
#include "cpp/MemoryControllers/MemoryController.h"

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
#define RxBuffer_SIZE   600

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
    STM32F429Wrapper device;
    device.Initialize();
#endif
    FRAM_Rohm_MR48V256CTAZAARL fram;

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
    uint8_t initialValue_1;

    uint32_t rowOffset ;
    uint32_t hammeringIterations ;

    if(huart == &huart1) {
        cout << "RECEIVE " << endl;
        char received_payload[Size + 1];
        memcpy(received_payload, RxBuffer, Size);
        received_payload[Size] = '\0';

        cout << "> Size: " << Size<< endl;
        cout << "> Received message: " << received_payload << endl;

        std::string jsonString = received_payload;
        auto keyValues = JsonParser::parse(jsonString);


        if (keyValues["HEADER"] == "*IDN?") {
            Frame frame(idn_payload);
            cout <<  idn_payload << endl;
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
                globalDataSetupTime = 15;
                globalAddressSetupTime = 10;
                device.Initialize();
                MemoryControllerParallel memoryController(nullptr, fram, device);
                memoryController.Initialize();

                stringstream ss(keyValues["dataSetupTime"]);
                ss >> globalDataSetupTime;
                cout << ">>> WRITE/READ with DataSetupTime: " << globalDataSetupTime << endl;

                MX_FMC_Init();

                printf("START WRITING\n");
                HAL_UART_Transmit(&huart1, writing_frame_buffer, writing_frame_bytes.size(),100);

                for (uint32_t i = startAddress; i < stopAddress; i++)
                {
                    //printf("inside for loop  %u \n", i);
                    memoryController.Write8BitWord(i, initialValue);
                }
                HAL_UART_Transmit(&huart1, end_writing_frame_buffer, end_writing_frame_bytes.size(),100);

                //device.Initialize();
                printf("START READING\n");

                //fram.GetMemorySize()
                //printf("%u", fram.GetMemorySize());
                HAL_UART_Transmit(&huart1, reading_frame_buffer, reading_frame_bytes.size(),100);

                for (uint32_t i = startAddress ; i < stopAddress; i++)
                {
                    memoryController.Read8BitWord(i, &retValue);
                    //printf("%x;%x\n", i , retValue);

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

                globalDataSetupTime = 15;
                device.Initialize();
                MemoryControllerParallel memoryController(nullptr, fram, device);
                memoryController.Initialize();

                /* FMC initialization */
                MX_FMC_Init();

                for (uint32_t i = startAddress ; i < stopAddress ; i++)
                {
                    memoryController.Write8BitWord(i, initialValue);
                }

                HAL_UART_Transmit(&huart1, end_writing_frame_buffer, end_writing_frame_bytes.size(),100);

                stringstream ss(keyValues["dataSetupTime"]);
                ss >> globalDataSetupTime;
                cout << ">>> READING with reduced DataSetupTime: " << globalDataSetupTime << endl;

                MX_FMC_Init();

                HAL_UART_Transmit(&huart1, reading_frame_buffer, reading_frame_bytes.size(),100);

                for (uint32_t i = startAddress; i < stopAddress; i++){
                    memoryController.Read8BitWord(i, &retValue);
                    printf("Address:: %x, Value: %x\n", i, retValue);

                    // build a frame
                    vector<uint8_t> raw_frame;
                    raw_frame.push_back(0xab);
                    raw_frame.push_back(0xef);
                    raw_frame.push_back(sizeof(retValue));
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

                initialValue_1 = stoi(keyValues["initialValue_1"], nullptr, 16);
                printf("> Initial Value 1 = %x\n", initialValue_1);

                // Default Parameters
                globalAddressSetupTime = 10;
                globalDataSetupTime = 100;

                device.Initialize();
                MemoryControllerParallel memoryController(nullptr, fram, device);
                memoryController.Initialize();

                /* FMC initialization with default params*/
                MX_FMC_Init();

                /* Fill under normal conditions */
                for (uint32_t i = startAddress ; i < stopAddress ; i++)
                {
                    memoryController.Write8BitWord(i, initialValue_1);
                }

                /* Read memory and verify write operations */
                for (uint32_t i = startAddress; i < stopAddress; i++){
                    memoryController.Read8BitWord(i, &retValue);
                    vector<uint32_t> data;
                    if(retValue != initialValue_1){
                        data = {0x00};
                    }else{
                        data = {0x01};
                    }
                }

                //HAL_UART_Transmit(&huart1, end_writing_frame_buffer, end_writing_frame_bytes.size(),100);

                stringstream ss(keyValues["dataSetupTime"]);
                ss >> globalDataSetupTime;
                cout << ">>> READING with reduced DataSetupTime: " << globalDataSetupTime << endl;
                MX_FMC_Init();

                // Fill the memory under reduced data setup time
                HAL_UART_Transmit(&huart1, end_writing_frame_buffer, end_writing_frame_bytes.size(),100);

                for (uint32_t i = startAddress ; i < stopAddress ; i++)
                {
                    memoryController.Write8BitWord(i, initialValue);
                }

                /* read under normal data setup time */
                // Default Parameters
                globalAddressSetupTime = 10;
                globalDataSetupTime = 100;

                /* FMC initialization with default params*/
                MX_FMC_Init();

                HAL_UART_Transmit(&huart1, reading_frame_buffer, reading_frame_bytes.size(),100);

                for (uint32_t i = startAddress; i < stopAddress; i++){
                    memoryController.Read8BitWord(i, &retValue);

                    // build a frame
                    vector<uint8_t> raw_frame;
                    raw_frame.push_back(0xab);
                    raw_frame.push_back(0xef);
                    raw_frame.push_back(sizeof(retValue));
                    raw_frame.push_back(retValue);
                    raw_frame.push_back(0xe1);
                    raw_frame.push_back(0xe2);
                    uint8_t *raw_frame_buffer = &raw_frame[0];

                    HAL_UART_Transmit(&huart1, raw_frame_buffer, raw_frame.size(), 100);
                }


            }else if(keyValues["testType"] == "rowHammingTest"){
                // TODO
                printf("*** START Row Hamming Test ***\n\n");
                initialValue_1 = stoi(keyValues["initialValue_1"], nullptr, 16);
                stringstream ss_rowOffset(keyValues["rowOffset"]);
                ss_rowOffset >> rowOffset;

                stringstream ss_hamIterations(keyValues["HammeringIterations"]);
                ss_hamIterations >> hammeringIterations;

                printf("initial value_1 %x \n", initialValue_1);
                printf("initial value %x \n", initialValue);

                cout << ">>> rowOffset: " << rowOffset << endl;
                cout << ">>> hammeringIterations: " << hammeringIterations << endl;

                // fill the memory with initial_value_1 and reduced data setup time
                stringstream ss_dst(keyValues["dataSetupTime"]);
                ss_dst >> globalDataSetupTime;

                device.Initialize();
                MemoryControllerParallel memoryController(nullptr, fram, device);
                memoryController.Initialize();

                /* FMC initialization with default params*/
                MX_FMC_Init();

                /* Fill under normal conditions */
                for (uint32_t i = startAddress ; i < stopAddress ; i++)
                {
                    memoryController.Write8BitWord(i, initialValue_1);
                }

                /* Read memory and verify write operations */
                for (uint32_t i = startAddress; i < stopAddress; i++){
                    memoryController.Read8BitWord(i, &retValue);
                    vector<uint32_t> data;
                    if(retValue != initialValue_1){
                        data = {0x00};
                    }else{
                        data = {0x01};
                    }
                }

                // hammer on the rows
                for (int iteration = 0; iteration < hammeringIterations; ++iteration) {
                    for (uint32_t i = startAddress; i < stopAddress; i += 2 * rowOffset) {
                        int endAddress = i + rowOffset;
                        if (endAddress > stopAddress) {
                            endAddress = stopAddress;
                        }
                        for (uint32_t writeAddress = i; writeAddress < endAddress; ++writeAddress) {
                            memoryController.Write8BitWord(writeAddress, initialValue);
                        }
                    }
                }

                // Default Parameters
                globalDataSetupTime = 100;
                cout << ">>> READING with reduced DataSetupTime: " << globalDataSetupTime << endl;
                MX_FMC_Init();

                for (uint32_t i = startAddress; i < stopAddress; i++) {
                        memoryController.Read8BitWord(i, &retValue);
                    // build a frame
                        vector<uint8_t> raw_frame;
                        raw_frame.push_back(0xab);
                        raw_frame.push_back(0xef);
                        raw_frame.push_back(sizeof(retValue));
                        raw_frame.push_back(retValue);
                        raw_frame.push_back(0xe1);
                        raw_frame.push_back(0xe2);
                        uint8_t *raw_frame_buffer = &raw_frame[0];
                        HAL_UART_Transmit(&huart1, raw_frame_buffer, raw_frame.size(), 100);
                }

                HAL_UART_Transmit(&huart1, end_reading_frame_buffer,end_reading_frame_bytes.size() , 100);

                // if no exit

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