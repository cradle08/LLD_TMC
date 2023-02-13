/*************************液面探测驱动板*********************************/
STM32F103C8T6, 20k_RAM, 64_Flash

CAN:
    RX: PA11
    TX: PA12

UART:(链接TMC)
    UART_RX：PA10
    UART_TX：PA9


IIC1:(气压传感器)
    CLK:  PB6
    SDA: PB7

IIC2（电容传感器）:
    CLK:  PB10
    SDA: PB11

SYS_LED:
    PB4

SW1-4:
    PC13\PC14\PC15, PA7

NTC：
    PA3（ADC）

FDC2212: 
    ADD: PA1
    INTB: PA4
    SD: PA5

光耦：
P_DP1: PA15（复位光耦）
P_DP1: PB0（限位位光耦）

LLD液面探测反馈信号: 
    PB1


SPI2: (链接FLASH)
    SPI_MOSI: PB15
    SPI_MISO: PB14
    SPI_CLK: PB13
    SPI_CS: PB12

TMC5130:
    SPI_MOSI: PB15
    SPI_MISO: PB14
    SPI_CLK: PB13
    SPI_CS: PB3


TMC2209:
    SPREAD: PB3
    DIR: PB4
    EN: PA6
    STEP:PB5
    CLK:PA8


