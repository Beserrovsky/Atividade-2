#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // API para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)
#include <pwm_z42.h>                // Biblioteca personalizada com funções de controle do TPM (Timer/PWM Module)

// Define o valor do registrador MOD do TPM para configurar o período do PWM
#define TPM_MODULE 1000         // Define a frequência do PWM fpwm = (TPM_CLK / (TPM_MODULE * PS))
// Valores de duty cycle correspondentes a diferentes larguras de pulso
uint16_t duty_50  = TPM_MODULE/2;       // 50% de duty cycle (meio brilho)

int main(void)
{
    // Inicializa o módulo TPM2 com:
    // - base do TPMx
    // - fonte de clock PLL/FLL (TPM_CLK)
    // - valor do registrador MOD
    // - tipo de clock (TPM_CLK)
    // - prescaler de 1 a 128 (PS)
    // - modo de operação EDGE_PWM
    pwm_tpm_Init(TPM2, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);

    // Inicializa o canal 0 do TPM2 para gerar sinal PWM na porta GPIOB_18
    // - modo TPM_PWM_H (nível alto durante o pulso)
    pwm_tpm_Ch_Init(TPM2, 0, TPM_PWM_H, GPIOB, 18);

    // Define o valor do duty cycle: nesse caso, duty_100 (LED quase desligado)
    pwm_tpm_CnV(TPM2, 0, duty_50);

    // uint16_t var_duty = TPM_MODULE/2;       // 50% de duty cycle (meio brilho)

    // uint8_t state = 0; // varies from 0 to 100, only integers - resolution of 101 states

    // Loop infinito
    for (;;)
    {
        // // Runs at ?? Hz

        // if (state < 100) state++;

        // // Maybe it is too fast, increasing state and adjusting on the var_duty might do the trick.


        // var_duty = TPM_MODULE * (state / 100);
        // pwm_tpm_CnV(TPM2, 0, var_duty);
    }

    return 0;
}