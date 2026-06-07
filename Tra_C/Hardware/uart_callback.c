#include "ti_msp_dl_config.h"
#include <string.h>
#include "board.h"
#include "uart_callback.h"
#include <stdlib.h>
//Bluetooth remote control associated flag bits
//蓝牙遥控相关的标志位
int Flag_Left, Flag_Right, Flag_Direction=0, Turn_Flag;

extern uint8_t PID_Send;

#define BT_PACKET_SIZE (200)
volatile uint8_t gBTPacket[BT_PACKET_SIZE];
volatile uint8_t gBTCounts = 0 ;
volatile uint8_t lastBTCounts = 0 ;
volatile bool gCheckBT;

uint8_t RecvOverFlag = 0;

void bt_control(uint8_t recv);

void BT_DAMConfig(void)
{
    DL_DMA_disableChannel(DMA, DMA_CH0_CHAN_ID);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&UART_BT_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &gBTPacket[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, BT_PACKET_SIZE);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
}

void BTBufferHandler(void)
{
    uint32_t tick = 0;
    static uint8_t handleflag = 0;
    static uint8_t handleSize = 0;
    static uint8_t lastSize = 0;

    //recvsize为已经接收的数据个数
    uint8_t recvsize = BT_PACKET_SIZE - DL_DMA_getTransferSize(DMA, DMA_CH0_CHAN_ID);

    uint32_t ticks = 0;
    if( recvsize != lastSize)
    {
        handleflag=1;
        tick = Systick_getTick();//正在接收数据,刷新时间
    }
    else
    {
        //数据已停止接收,超时后开始处理
        if( ((tick-Systick_getTick())&SysTickMAX_COUNT) >= SysTick_MS(1) && handleflag == 1)
        {
            handleflag = 0;

            //处理数据
            // for(uint8_t i=handleSize;i<recvsize;i++)
            //     printf("%c",gBTPacket[i]);
            // printf("\r\n");

            //处理串口数据
            for(uint8_t i=handleSize;i<recvsize;i++)
                bt_control(gBTPacket[i]);


            //记录本次数据处理的位???
            handleSize = recvsize;

            //处理到一定数据量时刷新一次dma搬运地址,不让dma完成搬运
            if( recvsize >= BT_PACKET_SIZE/2 )
            {
                recvsize=0;
                handleSize=0;
                lastSize = 0;
                BT_DAMConfig();
            }
        }
    }

    lastSize = recvsize;
}


void UART_BT_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_BT_INST)) {
        case DL_UART_IIDX_RX:
            gBTPacket[gBTCounts++] = DL_UART_Main_receiveData(UART_BT_INST);
            break;
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:
            BT_DAMConfig();
        default:
            break;
    }
}

void bt_control(uint8_t recv)
{
    static int Usart_Receive = 0;
    static uint8_t Flag_PID = 0;
    static uint8_t i = 0;
    static uint8_t j = 0;
    static uint8_t Receive[50];
    static float Data = 0;
    /* Save received byte */
    Usart_Receive = recv;
    /* Start frame */
    if(Usart_Receive == 0x7B)      // '{'
        Flag_PID = 1;
    /* End frame */
    if(Usart_Receive == 0x7D)      // '}'
        Flag_PID = 2;
    /* Collect frame data */
    if(Flag_PID == 1)
    {
        Receive[i] = Usart_Receive;
        i++;
    }
    /* Parse frame */
    if(Flag_PID == 2)
    {
        /* APP requests current PID parameters */
        if(Receive[3] == 0x50)     // 'P'
        {
            PID_Send = 1;
        }
        else if(Receive[1] != 0x23)    // '#'
        {
            /* Convert ASCII number to float */
            char num_str[20];
            memcpy(num_str, &Receive[3], i - 4);
            num_str[i - 4] = '\0';
            Data = atof(num_str);
            switch(Receive[1])
            {
                case 0x30:  Velocity_KPa=Data;break;
                case 0x31:  Velocity_KIa=Data;break;
                case 0x32:  MotorA.Target_Encoder=Data;break;//Velocity_Kp=Data;break;
                case 0x33:  Velocity_KPb=Data;break;//Velocity_Ki=Data;break;
                case 0x34:  Velocity_KIb=Data;break;//Turn_Kp=Data;break;
                case 0x35:  MotorB.Target_Encoder=Data;break;//Turn_Kd=Data;break;
                case 0x36:  break; //预留
                case 0x37:  break; //预留
                case 0x38:  break; //预留
            }
        }
        /* Reset parser state */
        Flag_PID = 0;
        i = 0;
        j = 0;
        Data = 0;
        memset(Receive, 0, sizeof(Receive));
    }
}