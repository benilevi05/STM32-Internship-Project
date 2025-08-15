/*
 * lcd_pcf8574.c — HD44780 LCD (4-bit) via PCF8574 I2C backpack
 *
 * Mapping (typical backpacks):
 *   PCF8574 P0: RS, P1: RW, P2: EN, P3: BL, P4..P7: D4..D7
 *
 * NOT MY CODE - Beni
 */

#include "main.h"
#include <stdbool.h>
#include <string.h>

/* ========= USER CONFIG ========= */
#ifndef LCD_I2C_HANDLE
#define LCD_I2C_HANDLE   hi2c1          // change if you use hi2c2/hi2c3
#endif

#ifndef LCD_I2C_ADDR_7BIT
#define LCD_I2C_ADDR_7BIT  0x27         // typical PCF8574 backpack
#endif

// If your backlight logic is inverted (rare), set to 1
#ifndef LCD_BACKLIGHT_INVERT
#define LCD_BACKLIGHT_INVERT  0
#endif
/* ======== END USER CONFIG ====== */

// Control bit positions in the PCF8574 output byte
#define LCD_BIT_RS   (1U << 0)
#define LCD_BIT_RW   (1U << 1)
#define LCD_BIT_EN   (1U << 2)
#define LCD_BIT_BL   (1U << 3)

// Commands
#define LCD_CMD_CLEAR         0x01
#define LCD_CMD_HOME          0x02
#define LCD_CMD_ENTRYMODE     0x04
#define LCD_CMD_DISPLAYCTL    0x08
#define LCD_CMD_SHIFT         0x10
#define LCD_CMD_FUNCTIONSET   0x20
#define LCD_CMD_SETCGRAMADDR  0x40
#define LCD_CMD_SETDDRAMADDR  0x80

// Flags for display entry mode
#define LCD_ENTRY_RIGHT          0x00
#define LCD_ENTRY_LEFT           0x02
#define LCD_ENTRY_SHIFT_INC      0x01
#define LCD_ENTRY_SHIFT_DEC      0x00

// Flags for display on/off control
#define LCD_DISPLAY_ON           0x04
#define LCD_DISPLAY_OFF          0x00
#define LCD_CURSOR_ON            0x02
#define LCD_CURSOR_OFF           0x00
#define LCD_BLINK_ON             0x01
#define LCD_BLINK_OFF            0x00

// Flags for function set
#define LCD_8BITMODE             0x10
#define LCD_4BITMODE             0x00
#define LCD_2LINE                0x08
#define LCD_1LINE                0x00
#define LCD_5x10DOTS             0x04
#define LCD_5x8DOTS              0x00

// Basic timing helpers
static inline void lcd_udelay(volatile uint32_t us)
{
    // crude busy-wait: adjust multiplier if needed for your clock
    // For 216 MHz F7, ~10..15 NOPs per microsecond; we keep it simple & conservative
    for (volatile uint32_t i = 0; i < us * 40; ++i) { __NOP(); }
}

static inline void lcd_mdelay(uint32_t ms) { HAL_Delay(ms); }

// Driver state
static I2C_HandleTypeDef *lcd_hi2c = &LCD_I2C_HANDLE;
static uint8_t lcd_addr = (LCD_I2C_ADDR_7BIT << 1); // HAL expects 8-bit address
static uint8_t lcd_backlight = 1; // default ON

// Low-level I2C write of one byte to PCF8574
static HAL_StatusTypeDef lcd_i2c_write(uint8_t data)
{
    return HAL_I2C_Master_Transmit(lcd_hi2c, lcd_addr, &data, 1, 10);
}

// Build control byte (upper nibble carries D7..D4)
static uint8_t lcd_ctrl_build(uint8_t rs, uint8_t rw, uint8_t en, uint8_t nibble /* low 4 bits */)
{
    uint8_t out = 0;

    // Put data nibble on P7..P4 (D7..D4)
    out |= (uint8_t)((nibble & 0x0F) << 4);

    if (rs) out |= LCD_BIT_RS;
    if (rw) out |= LCD_BIT_RW;
    if (en) out |= LCD_BIT_EN;

    // Backlight bit (may be inverted)
    if (lcd_backlight ^ LCD_BACKLIGHT_INVERT)
        out |= LCD_BIT_BL;

    return out;
}

// Pulse EN high->low with the nibble present
static void lcd_pulse(uint8_t rs, uint8_t rw, uint8_t nibble)
{
    uint8_t hi = lcd_ctrl_build(rs, rw, 1, nibble);
    uint8_t lo = lcd_ctrl_build(rs, rw, 0, nibble);

    // EN high
    lcd_i2c_write(hi);
    lcd_udelay(1);  // >450 ns

    // EN low (latch)
    lcd_i2c_write(lo);
    lcd_udelay(50); // hold time for commands (min ~37us), we use ~50us
}

// Send one 8-bit value (command or data) using 4-bit transfers
static void lcd_send(uint8_t value, uint8_t rs)
{
    // Upper nibble first, then lower nibble
    lcd_pulse(rs, 0, (value >> 4) & 0x0F);
    lcd_pulse(rs, 0, (value & 0x0F));
}

// Public: backlight control
void lcd_backlight_set(bool on)
{
    lcd_backlight = on ? 1 : 0;
    // Write a no-op (EN low) to update BL pin immediately
    uint8_t b = lcd_ctrl_build(0, 0, 0, 0);
    lcd_i2c_write(b);
}

// Public: write a command
void lcd_cmd(uint8_t cmd)
{
    lcd_send(cmd, 0);
    // Certain commands need longer delays
    if (cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME) {
        lcd_mdelay(2); // 1.52 ms typical; we wait ~2 ms
    }
}

// Public: write a single character
void lcd_putc(char c)
{
    lcd_send((uint8_t)c, 1);
}

// Public: write a string
void lcd_puts(const char *s)
{
    while (*s) {
        lcd_putc(*s++);
    }
}

// Set cursor to (col, row). Supports 20x4/16x2 defaults.
void lcd_set_cursor(uint8_t col, uint8_t row)
{
    static const uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row > 3) row = 3;
    lcd_cmd(LCD_CMD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Clear & home
void lcd_clear(void) { lcd_cmd(LCD_CMD_CLEAR); }
void lcd_home(void)  { lcd_cmd(LCD_CMD_HOME);  }

// Create a custom 5x8 char at CGRAM location (0..7). pattern: 8 bytes, lower 5 bits used.
void lcd_create_char(uint8_t location, const uint8_t pattern[8])
{
    location &= 0x07;
    lcd_cmd(LCD_CMD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; ++i) lcd_putc((char)pattern[i]);
}

// Initialize LCD (must be called after power-up and I2C init)
void lcd_init(I2C_HandleTypeDef *hi2c, uint8_t addr7, bool backlight_on,
              bool two_lines /*true*/, bool font_5x10 /*false for most*/)
{
    lcd_hi2c = hi2c ? hi2c : &LCD_I2C_HANDLE;
    lcd_addr = (addr7 << 1);
    lcd_backlight = backlight_on ? 1 : 0;

    // Wait >40 ms after VDD rises
    lcd_mdelay(50);

    // The LCD is in 8-bit mode on power-up; send 0x03 three times
    // but we only have 4-bit bus via backpack, so we send high nibble pulses.
    for (int i = 0; i < 3; ++i) {
        lcd_pulse(0, 0, 0x03);
        lcd_mdelay(5);
    }

    // Set to 4-bit mode
    lcd_pulse(0, 0, 0x02);
    lcd_mdelay(1);

    // Function set: 4-bit, 2-line (or 1-line), 5x8 (or 5x10)
    uint8_t function = LCD_CMD_FUNCTIONSET | LCD_4BITMODE |
                       (two_lines ? LCD_2LINE : LCD_1LINE) |
                       (font_5x10 ? LCD_5x10DOTS : LCD_5x8DOTS);
    lcd_cmd(function);

    // Display OFF
    lcd_cmd(LCD_CMD_DISPLAYCTL | LCD_DISPLAY_OFF | LCD_CURSOR_OFF | LCD_BLINK_OFF);

    // Clear
    lcd_clear();

    // Entry mode: left-to-right, no shift
    lcd_cmd(LCD_CMD_ENTRYMODE | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DEC);

    // Display ON (no cursor, no blink by default)
    lcd_cmd(LCD_CMD_DISPLAYCTL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);

    // Ensure BL state is pushed
    lcd_backlight_set(backlight_on);
}

/*

extern I2C_HandleTypeDef hi2c1;

void lcd_demo(void)
{
    lcd_init(&hi2c1, 0x27, true, true, false); // addr=0x27, BL on, 2-line, 5x8
    lcd_set_cursor(0,0);
    lcd_puts("Hello, STM32!");
    lcd_set_cursor(0,1);
    lcd_puts("PCF8574 LCD OK");
}
 */
