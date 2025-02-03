#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define tempo 200
#define BOTAO_A 5
#define BOTAO_B 6
#define LED_R 11
#define LED_G 12
#define LED_B 13

// Variável global para armazenar a cor (Entre 0 e 255 para intensidade)
uint8_t led_r = 0; // Intensidade do vermelho
uint8_t led_g = 100; // Intensidade do verde
uint8_t led_b = 0; // Intensidade do azul

// Variável global para armazenar numeral
static uint8_t number = 1;

// Armazena o tempo do último evento (em microssegundos)
static volatile uint32_t last_time = 0; 

// Buffer para armazenar quais LEDs estão ligados matriz 5x5
bool led_buffer_1[NUM_PIXELS] = {
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0
};

bool led_buffer_2[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 0
};

bool led_buffer_3[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool led_buffer_4[NUM_PIXELS] = {
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0
};

bool led_buffer_5[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool led_buffer_6[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool led_buffer_7[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0
};

bool led_buffer_8[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool led_buffer_9[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool led_buffer_0[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
};

// Array de ponteiros para os buffers
bool* led_buffers[10] = {
    led_buffer_0, led_buffer_1, led_buffer_2, led_buffer_3, led_buffer_4,
    led_buffer_5, led_buffer_6, led_buffer_7, led_buffer_8, led_buffer_9
};

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}


void set_one_led()
{
    // Define a cor
    uint32_t color = urgb_u32(255, 0, 0);
    bool* buffer = led_buffers[number % 10];

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (buffer[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

//  Funções de Interrupção
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if(current_time - last_time > 200000) { // 200 ms de debouncing
        last_time = current_time;
        if(gpio == BOTAO_A){
            number++;
            set_one_led();
        } else {
            number--;
            set_one_led();
        }
    }
}

int main()
{
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    //inicializacao
    stdio_init_all();   // Inicializa o terminal serial
    gpio_init(BOTAO_A); // Inicializa Botao A 
    gpio_set_dir(BOTAO_A, GPIO_IN); // Configura Botao como entrada
    gpio_pull_up(BOTAO_A);  // Habilita Pull Up interno
    gpio_init(BOTAO_B); // Inicializa Botao B
    gpio_set_dir(BOTAO_B, GPIO_IN); // Configura Botao como entrada
    gpio_pull_up(BOTAO_B);  // Habilita Pull Up interno

    gpio_init(LED_R);   // Inicializa o LED
    gpio_set_dir(LED_R, GPIO_OUT);  // Configura o led como saida
    gpio_init(LED_G);   // Inicializa o LED
    gpio_set_dir(LED_G, GPIO_OUT);  // Configura o led como saida
    gpio_init(LED_B);   // Inicializa o LED
    gpio_set_dir(LED_B, GPIO_OUT);  // Configura o led como saida

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);


    while (1)
    {
        gpio_put(LED_R, true);
        sleep_ms(tempo);
        gpio_put(LED_R, false);
        sleep_ms(tempo);
    }

    return 0;
}