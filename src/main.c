#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/console/console.h>
#include <stdlib.h>
#include <pwm_z42.h>

/*
 * PWM configuration
 *
 * TPM clock: MCGFLLCLK (~48 MHz) selected via TPM_PLLFLL
 * Prescaler: PS_128  → TPM tick clock = 48 MHz / 128 ≈ 375 kHz
 * MOD = TPM_MODULE   → PWM frequency ≈ 375 kHz / 1000 = 375 Hz  (imperceptible flicker)
 *
 * Active-low LED: a higher CnV value means more time HIGH → less LED brightness.
 *   CnV = TPM_MODULE * (100 - brightness_pct) / 100
 */
#define TPM_MODULE      1000

/*
 * Orange = Red 100% + Green ~65% + Blue 0%
 * These ratios set the colour balance; overall brightness scales both uniformly.
 */
#define ORANGE_RED_PCT   100
#define ORANGE_GREEN_PCT  65

static uint16_t brightness_to_cnv(int brightness_pct, int colour_pct)
{
    int effective = brightness_pct * colour_pct / 100;
    return (uint16_t)(TPM_MODULE * (100 - effective) / 100);
}

static void set_orange(int brightness_pct)
{
    if (brightness_pct < 0)   brightness_pct = 0;
    if (brightness_pct > 100) brightness_pct = 100;

    pwm_tpm_CnV(TPM2, 0, brightness_to_cnv(brightness_pct, ORANGE_RED_PCT));
    pwm_tpm_CnV(TPM2, 1, brightness_to_cnv(brightness_pct, ORANGE_GREEN_PCT));
}

int main(void)
{
    pwm_tpm_Init(TPM2, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Ch_Init(TPM2, 0, TPM_PWM_H, GPIOB, 18);  /* Red   — PTB18 */
    pwm_tpm_Ch_Init(TPM2, 1, TPM_PWM_H, GPIOB, 19);  /* Green — PTB19 */

    console_init();

    printk("=== Controle de Cor Laranja via PWM ===\n");
    printk("Orange: Vermelho 100%% + Verde 65%%\n");
    printk("Frequencia PWM: ~375 Hz\n\n");

    set_orange(50);

    while (1) {
        printk("Digite a intensidade (0-100): ");
        char *line = console_getline();
        if (!line || line[0] == '\0') continue;

        int brightness = atoi(line);
        set_orange(brightness);

        printk("Intensidade: %d%% | CnV vermelho: %u | CnV verde: %u\n",
               brightness,
               brightness_to_cnv(brightness, ORANGE_RED_PCT),
               brightness_to_cnv(brightness, ORANGE_GREEN_PCT));
    }

    return 0;
}
