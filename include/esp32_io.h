#ifndef esp32_io_h
#define esp32_io_h

#include <stdint.h>

static inline __attribute__((always_inline))
uint32_t directRead(uint32_t pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    return (GPIO.in.val >> pin) & 0x1;
#else // plain ESP32
    if ( pin < 32 )
        return (GPIO.in >> pin) & 0x1;
    else if ( pin < 46 )
        return (GPIO.in1.val >> (pin - 32)) & 0x1;
#endif

    return 0;
}

static inline __attribute__((always_inline))
void directWriteLow(uint32_t pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    GPIO.out_w1tc.val = ((uint32_t)1 << pin);
#else // plain ESP32
    if ( pin < 32 )
        GPIO.out_w1tc = ((uint32_t)1 << pin);
    else if ( pin < 46 )
        GPIO.out1_w1tc.val = ((uint32_t)1 << (pin - 32));
#endif
}

static inline __attribute__((always_inline))
void directWriteHigh(uint32_t pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    GPIO.out_w1ts.val = ((uint32_t)1 << pin);
#else // plain ESP32
    if ( pin < 32 )
        GPIO.out_w1ts = ((uint32_t)1 << pin);
    else if ( pin < 46 )
        GPIO.out1_w1ts.val = ((uint32_t)1 << (pin - 32));
#endif
}

static inline __attribute__((always_inline))
void directModeInput(uint32_t pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    GPIO.enable_w1tc.val = ((uint32_t)1 << (pin));
#else
    if ( digitalPinIsValid(pin) )
    {
#if ESP_IDF_VERSION_MAJOR < 4      // IDF 3.x ESP32/PICO-D4
        uint32_t rtc_reg(rtc_gpio_desc[pin].reg);

        if ( rtc_reg ) // RTC pins PULL settings
        {
            ESP_REG(rtc_reg) = ESP_REG(rtc_reg) & ~(rtc_gpio_desc[pin].mux);
            ESP_REG(rtc_reg) = ESP_REG(rtc_reg) & ~(rtc_gpio_desc[pin].pullup | rtc_gpio_desc[pin].pulldown);
        }
#endif
	// Input
        if ( pin < 32 )
            GPIO.enable_w1tc = ((uint32_t)1 << pin);
        else
            GPIO.enable1_w1tc.val = ((uint32_t)1 << (pin - 32));
    }
#endif
}

static inline __attribute__((always_inline))
void directModeOutput(uint32_t pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    GPIO.enable_w1ts.val = ((uint32_t)1 << (pin));
#else
    if ( digitalPinIsValid(pin) && pin <= 33 ) // pins above 33 can be only inputs
    {
#if ESP_IDF_VERSION_MAJOR < 4      // IDF 3.x ESP32/PICO-D4
        uint32_t rtc_reg(rtc_gpio_desc[pin].reg);

        if ( rtc_reg ) // RTC pins PULL settings
        {
            ESP_REG(rtc_reg) = ESP_REG(rtc_reg) & ~(rtc_gpio_desc[pin].mux);
            ESP_REG(rtc_reg) = ESP_REG(rtc_reg) & ~(rtc_gpio_desc[pin].pullup | rtc_gpio_desc[pin].pulldown);
        }
#endif
        // Output
        if ( pin < 32 )
            GPIO.enable_w1ts = ((uint32_t)1 << pin);
        else // already validated to pins <= 33
            GPIO.enable1_w1ts.val = ((uint32_t)1 << (pin - 32));
    }
#endif
}

#ifdef  interrupts
#undef  interrupts
#endif
#ifdef  noInterrupts
#undef  noInterrupts
#endif
#define noInterrupts() {portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;portENTER_CRITICAL(&mux)
#define interrupts() portEXIT_CRITICAL(&mux);}

#endif
