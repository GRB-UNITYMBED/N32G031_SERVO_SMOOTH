#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"

/* ====================================================================
 * ⚙️ [โซนจูนฮาร์ดแวร์] 
 * ค่าจุดองศาต่างๆ ที่เราหามาอย่างยากลำบาก!
 * ==================================================================== */
#define PULSE_0_DEG     2250   // 0 องศา (หันซ้ายสุด)
#define PULSE_90_DEG    4400   // 90 องศา (ชี้ขึ้นฟ้าตั้งฉาก)
#define PULSE_180_DEG   5500   // 180 องศา (หันขวาสุด)

/* ฟังก์ชันหน่วงเวลา */
void Delay_Loop(volatile uint32_t count) {
    while(count--) { __NOP(); }
}

/* ====================================================================
 * 🧩 ฟังก์ชันย่อย: "ขยับทีละก้าว" (ส่งสัญญาณแค่ 1 รอบคลื่น)
 * ==================================================================== */
void Servo_Step(uint32_t pulse_width) {
    GPIO_SetBits(GPIOA, GPIO_PIN_1);     
    Delay_Loop(pulse_width);             
    GPIO_ResetBits(GPIOA, GPIO_PIN_1);   
    Delay_Loop(100000);                  
}

/* ====================================================================
 * 🪄 ฟังก์ชันเวทมนตร์: "ขยับแบบสมูทจากมุมหนึ่งไปอีกมุมหนึ่ง"
 * ==================================================================== */
void Smooth_Move(uint32_t start_pulse, uint32_t end_pulse) {
    // ถ้าจุดหมายอยู่ไกลกว่า (ต้องหมุนไปทางขวา) ให้ค่อยๆ บวกเลขขึ้น
    if (start_pulse < end_pulse) {
        for (uint32_t p = start_pulse; p <= end_pulse; p += 20) {
            Servo_Step(p);
        }
    } 
    // ถ้าจุดหมายอยู่น้อยกว่า (ต้องหมุนไปทางซ้าย) ให้ค่อยๆ ลบเล็กลง
    else {
        for (uint32_t p = start_pulse; p >= end_pulse; p -= 20) {
            Servo_Step(p);
        }
    }
}

/* ====================================================================
 * 🚀 [โซนฝึกเขียนโค้ด] ภารกิจ: "หันกวาด ซ้าย-กลาง-ขวา แบบนุ่มนวล"
 * ==================================================================== */
int main(void) {
    /* เตรียมความพร้อมฮาร์ดแวร์ */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    while(1) {
        
        /* 1. ค่อย ๆ กวาดแขนจาก 0 ไป 90 องศา */
        Smooth_Move(PULSE_0_DEG, PULSE_90_DEG);
        Delay_Loop(8000000); // พักโชว์ท่า 2 วินาที
        
        /* 2. ค่อย ๆ กวาดแขนจาก 90 ไป 180 องศา */
        Smooth_Move(PULSE_90_DEG, PULSE_180_DEG);
        Delay_Loop(8000000); // พักโชว์ท่า 2 วินาที

        /* 3. ค่อย ๆ กวาดแขนกลับจาก 180 มาที่ 90 องศา */
        Smooth_Move(PULSE_180_DEG, PULSE_90_DEG);
        Delay_Loop(8000000); // พักโชว์ท่า 2 วินาที

        /* 4. ค่อย ๆ กวาดแขนกลับจาก 90 มาที่ 0 องศา เพื่อเตรียมเริ่มรอบใหม่ */
        Smooth_Move(PULSE_90_DEG, PULSE_0_DEG);
        Delay_Loop(8000000); // พักโชว์ท่า 2 วินาที

    }
}