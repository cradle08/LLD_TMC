
STM32F103C8T6, 20k_RAM, 64_Flash

CAN:
    RX: PA11
    TX: PA12

UART:(链接TMC)
    UART_RX：PA10
    UART_TX：PA9

SPI2: (链接FLASH)
    SPI_MOSI: PB15
    SPI_MISO: PB14
    SPI_CLK: PB13
    SPI_CS: PB12

IIC1:(气压传感器)
    CLK:  PB6
    SDA: PB7

IIC2（电容传感器）:
    CLK:  PB10
    SDA: PB11

TMC2209:
    SPREAD: PB3
    DIR: PB4
    STEP:PB5
    EN: PA6
    CLK:PA8

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





/**********************************************************/
STM32F103ZET6, 64k_RAM, 512_Flash

UART1:(链接TMC)
UART_RX：PA10
UART_TX：PA9

UART2：（调试）
RX: PD6
TX: PD5

SPI2: (链接FLASH)
SPI_MOSI: PB15
SPI_MISO: PB14
SPI_CLK: PB13
SPI_CS: PB13 --->PB8

SPREAD: PB3

DIR: PB4
STEP:PB5 ---> PA7、PC7
EN: PA6

CLK:PA8 --->*




