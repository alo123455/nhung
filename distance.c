
#include "distance.h"
// Bộ module HC_SR04 hoạt động dựa trên nguyên lý phát tín hiệu ở chân Trig và nhận tín hiệu ở chân Echo

void GPIO_Init(void) {
    // Enable clock cho GPIOA
    RCC -> AHB1ENR |= 0x1;
    
    // Cấu hình PA6 (Trig) là output
    GPIOA -> MODER &= ~(0x3 << 6 * 2);  // Clear bits for PA6
    GPIOA -> MODER |= (0x1 << 6 * 2);   // Set PA6 as output
    
    // Cấu hình PA7 (Echo) là input
    GPIOA -> MODER &= ~(0x3 << 7 * 2);  // PA7 là input

    // Enable Pull-down resistor cho PA7
    GPIOA -> PUPDR |= (0x2 << 7 * 2);  // Pull-down cho PA7
}

void Timer2_Init(void) {
    // Enable clock cho Timer2
    RCC -> APB1ENR |= 0x1;
    
    // Cấu hình prescaler để timer chạy với tần số 1MHz (16 MHz / 16 = 1 MHz)
    TIM2 -> PSC = 16 - 1;  // Prescaler = 15
    
    // Cấu hình Auto-reload register (ARR) để timer đếm tới giá trị 65535
    TIM2 -> ARR = 0xFFFF;
    
    // Bật bộ đếm Timer2
    TIM2 -> CR1 |= 0x1;
}

void HC_SR04_Init(void) {
    GPIO_Init();
    Timer2_Init();
}

uint32_t HC_SR04_Distance(void) {
    HC_SR04_Init();  // Khởi tạo HC-SR04 (nếu cần thiết)
    
    // Gửi tín hiệu kích hoạt
    GPIOA->ODR |= TRIG_PIN; // Chân Trig lên mức HIGH
    for (int i = 0; i < 10; i++);  // Giữ trong 10 micro giây (hoặc dùng delay chính xác 10us)
    GPIOA->ODR &= ~TRIG_PIN; // Chân Trig xuống mức LOW
    
    uint32_t time = 0;

    // Chờ Echo pin lên mức HIGH với timeout
    uint32_t timeout = 100000; // Timeout nếu không có vật cản
    while ((GPIOA->IDR & ECHO_PIN) == 0 && timeout-- > 0);  // Chờ Echo pin lên mức HIGH
    if (timeout == 0) {
        return 0;  // Không có vật cản hoặc lỗi
    }
    
    // Bắt đầu đo thời gian khi Echo pin lên mức HIGH
    TIM2->CNT = 0;  // Đặt lại bộ đếm Timer (Tính từ lúc Echo pin lên mức HIGH)
    while ((GPIOA->IDR & ECHO_PIN) != 0);  // Chờ Echo pin xuống mức LOW
    
    time = TIM2->CNT;  // Lấy giá trị đếm của Timer
    
    // Khoảng cách = (thời gian * tốc độ âm thanh) / 2
    // Tốc độ âm thanh = 34000 cm/s, vì tín hiệu đi và về nên chia cho 2
    uint32_t distance = (time * 34000) / 2000;  // Tính khoảng cách (cm)
    
    return distance;  // Trả về khoảng cách tính được
}
