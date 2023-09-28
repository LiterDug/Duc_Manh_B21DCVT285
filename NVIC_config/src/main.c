/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f10x.h"
#include "Delay.h"
void LEDconfig(void);
void GPIO_EXTI(void);
void configEXTI(void);
void NVIC_EXTI(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);






// cau hinh cho led chan B2
void LEDconfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); //cai dat cac cau hinh trencho GPIOB
}



//cau hinh ngat ngoai cho 2 chan A8 va C13
// B1:cap xung va cau hinh chan A8 va C13 nhan tin hieu ngat ngoai lango vao keo len
// (cau hinh GPIO)
void GPIO_EXTI(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
//do ngat ngoai la chuc nang thay the nen phai bat AIFO

//cau hinh chan A4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

//cau hinh chan A8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

//cau hinh chan C13
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

//chon chan A8 va C13 la chan nhan tin hieu ngat ngoai
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
}

// B2:Cau hinh va cho phep ngat ngoai o EXTI
void configEXTI(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line8 | EXTI_Line13 | EXTI_Line4; //chon kenh 8 va kenh 13 ung voi A8 va C13
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //chon chedo ngat ngoai
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //choncanh tich cuc la canh xuong
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; //cho phep kenhngat ngoai duoc cau hinh
    EXTI_Init(&EXTI_InitStructure); //lenh cau hinh cac thong so duocluu trong EXTI_InitStructure
}

// B3: cau hinh cap do uu tien va cho phep ngat ngoai o NVIC
void NVIC_EXTI()
{


    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //0 capPreemptionPriority va 16 cap SubPriority

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //chon muc uu tien
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;// chon cac kenh tu 5-9
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //chon thu tuuu tien
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // cho phepngoai vi dang dc cau hinh o NVIC
    NVIC_Init(&NVIC_InitStructure); // lenh cau hinh cac thong so duocluu trong NVIC_InitStructure cho NVIC

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//chon kenh tu 10 den 15
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //chon mucuu tien
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


// B4: Viet chuong trinh con phuc vu ngat ngoai
// chuong trinh con phuc vi ngat ngoai cho chan A8
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_IMR_MR8) != RESET)
//kiem tra co phai la kenh 8 ngat khong?
    {
        EXTI_ClearITPendingBit(EXTI_IMR_MR8); //xoa congat kenh 8
        GPIO_SetBits(GPIOB, GPIO_Pin_2); //mo Led
    }
}
//chuong trinh con phuc vu ngat ngoai cho chan C13
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_IMR_MR13) != RESET)
//kiem tra co phai la kenh 13 ngat khong
    {
        EXTI_ClearITPendingBit(EXTI_IMR_MR13); //xoa congat kenh 13
        GPIO_ResetBits(GPIOB, GPIO_Pin_2); //tat Led
    }
}

void EXTI4_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_IMR_MR4) != RESET)
//kiem tra co phai la kenh 4 ngat khong
    {
        EXTI_ClearITPendingBit(EXTI_IMR_MR4); //xoa co ngat kenh 4
        GPIO_ResetBits(GPIOB, GPIO_Pin_2); //tat Led
    }
}


int main(void)
{
    SystemInit();
    LEDconfig();
    GPIO_EXTI();
    configEXTI();
    NVIC_EXTI();

    while(1)
    {

    }



}



















