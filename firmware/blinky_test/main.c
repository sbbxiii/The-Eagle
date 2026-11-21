typedef unsigned int uint32_t;

#define RCC_AHB1ENR   (*(volatile uint32_t *)(0x40023800 + 0x30))
#define GPIOC_MODER   (*(volatile uint32_t *)(0x40020800 + 0x00))
#define GPIOC_OTYPER  (*(volatile uint32_t *)(0x40020800 + 0x04))
#define GPIOC_OSPEEDR (*(volatile uint32_t *)(0x40020800 + 0x08))
#define GPIOC_ODR     (*(volatile uint32_t *)(0x40020800 + 0x14))

extern uint32_t _estack;

void Reset_Handler(void);
void Default_Handler(void) { while (1); }

__attribute__((section(".isr_vector")))
const void *vector_table[] = {
    &_estack,
    Reset_Handler,
    Default_Handler, /* NMI */
    Default_Handler, /* HardFault */
    Default_Handler, /* MemManage */
    Default_Handler, /* BusFault */
    Default_Handler, /* UsageFault */
};

static void delay(volatile uint32_t count) {
    while (count--) {
        __asm__("nop");
    }
}

void Reset_Handler(void) {
    /* 1. Enable GPIOC Clock in RCC_AHB1ENR (bit 2 = GPIOCEN) */
    RCC_AHB1ENR |= (1 << 2);

    /* 2. Configure PC8 as General Purpose Output (MODER8 = 01) */
    GPIOC_MODER &= ~(3U << (8 * 2));
    GPIOC_MODER |= (1U << (8 * 2));

    /* 3. Push-Pull (OTYPER8 = 0) */
    GPIOC_OTYPER &= ~(1U << 8);

    /* 4. High speed (OSPEEDR8 = 10) */
    GPIOC_OSPEEDR |= (2U << (8 * 2));

    /* 5. Infinite toggle loop (~2 Hz heartbeat) */
    while (1) {
        GPIOC_ODR ^= (1U << 8);
        delay(800000);
    }
}
