/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_DEVICEDEFINES_H
#define MEMORY_TESTING_FW_DEVICEDEFINES_H
#include "cpp/MemoryErrorHandling.h"

#include <vector>
#include <list>

#if STM32
#include <stm32f4xx_hal_gpio.h>

#if STM32F429xx
#include <stm32f429xx.h>
#endif // STM32F429xx

typedef GPIO_TypeDef* GPIOBank;

#define  IO_BANK_A          ((GPIO_TypeDef *) GPIOA_BASE)
#define  IO_BANK_B          ((GPIO_TypeDef *) GPIOB_BASE)
#define  IO_BANK_C          ((GPIO_TypeDef *) GPIOC_BASE)
#define  IO_BANK_D          ((GPIO_TypeDef *) GPIOD_BASE)
#define  IO_BANK_E          ((GPIO_TypeDef *) GPIOE_BASE)
#define  IO_BANK_F          ((GPIO_TypeDef *) GPIOF_BASE)
#define  IO_BANK_G          ((GPIO_TypeDef *) GPIOG_BASE)
#define  IO_BANK_H          ((GPIO_TypeDef *) GPIOH_BASE)
#define  IO_BANK_UNDEFINED  ((GPIO_TypeDef *) nullptr)


#define  IO_PIN_1       GPIO_PIN_1
#define  IO_PIN_2       GPIO_PIN_2
#define  IO_PIN_3       GPIO_PIN_3
#define  IO_PIN_4       GPIO_PIN_4
#define  IO_PIN_5       GPIO_PIN_5
#define  IO_PIN_6       GPIO_PIN_6
#define  IO_PIN_7       GPIO_PIN_7
#define  IO_PIN_8       GPIO_PIN_8
#define  IO_PIN_9       GPIO_PIN_9
#define  IO_PIN_10      GPIO_PIN_10
#define  IO_PIN_11      GPIO_PIN_11
#define  IO_PIN_12      GPIO_PIN_12
#define  IO_PIN_13      GPIO_PIN_13
#define  IO_PIN_14      GPIO_PIN_14
#define  IO_PIN_15      GPIO_PIN_15
#define  IO_PIN_ALL         GPIO_PIN_ALL
#define  IO_PIN_UNDEFINED   0x00
#else
typedef int GPIOBank;
#endif // STM32

class GPIOPin {
    /**
     * @brief This class represents a GPIO pin consisting of a GPIO bank and a Pin.
     */
public:
    GPIOPin(GPIOBank bank, uint32_t pin): m_GPIOBank(bank), m_GPIOPin(pin){ }

    /**
     * @brief Overloaded operator allows the comparison between two different GPIOPins.
     * @param gpioPin gpio pin to compare.
     * @return true if the bank and pin is equal otherwise return false.
     */
    bool operator == (const GPIOPin& gpioPin) const
    {
        return m_GPIOPin == gpioPin.m_GPIOPin && m_GPIOBank == gpioPin.m_GPIOBank;
    }

    GPIOBank m_GPIOBank;
    uint32_t m_GPIOPin;
};

#endif // MEMORY_TESTING_FW_DEVICEDEFINES_H