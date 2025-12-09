#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum {
    vga_height = 25,
    vga_width = 80,
    vga_memory = 0xb8000,
    ps2_data = 0x60,
    ps2_ctrl = 0x64,
};

/* Hardware text mode color constants. */
enum vga_color {
    vga_color_black = 0,
    vga_color_light_grey = 7,
};

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                     : "=a"(ret)
                     : "Nd"(port)
                     : "memory");
    return ret;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val),
                       "Nd"(port) : "memory");
}

static inline void cpu_relax(void)
{
    asm volatile ( "rep ; nop" : : : "memory" );
}

static inline uint8_t vga_entry_color(enum vga_color fg,
                                      enum vga_color bg) 
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

static size_t strlen(const char* str) 
{
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

static size_t term_row;
static size_t term_column;
static uint8_t term_color;
static uint16_t* term_buffer = (uint16_t*)vga_memory;

static void term_initialize(void) 
{
    term_row = 0;
    term_column = 0;
    term_color = vga_entry_color(vga_color_light_grey,
                                     vga_color_black);
        
    for (size_t y = 0; y < vga_height; y++) {
        for (size_t x = 0; x < vga_width; x++) {
            const size_t index = y * vga_width + x;
            term_buffer[index] = vga_entry(' ', term_color);
        }
    }
}

static void term_putentryat(char c, uint8_t color,
                                size_t x, size_t y) 
{
    const size_t index = y * vga_width + x;
    term_buffer[index] = vga_entry(c, color);
}

static void update_cursor(int x, int y)
{
    uint16_t pos = y * vga_width + x;

    outb(0x3d4, 0x0f);
    outb(0x3d5, (uint8_t) (pos & 0xff));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (uint8_t) ((pos >> 8) & 0xff));
}

static void term_putchar(char c) 
{
    if (c == '\n') {
        term_column = vga_width - 1;
    } else {
        term_putentryat(c, term_color, term_column,
                            term_row);
    }
    if (++term_column == vga_width) {
        term_column = 0;
        if (++term_row == vga_height) {
            term_row = 0;
        }
    }
    update_cursor(term_column, term_row);
}

static void term_write(const char* data, size_t size) 
{
    for (size_t i = 0; i < size; i++) {
        term_putchar(data[i]);
    }
}

static void term_writestring(const char* data) 
{
    term_write(data, strlen(data));
}


static uint8_t ps2_read()
{
    int cnt = 1024;
    while(!(inb(ps2_ctrl) & 1) && --cnt) cpu_relax();
    return cnt ? inb(ps2_data) : 0xff;
}

#define SCANCODE(x, y) case (x): return (y);

static char getchar()
{
    for (;;) {
        switch (ps2_read()) {
            SCANCODE(0x02, '1');
            SCANCODE(0x03, '2');
            SCANCODE(0x04, '3');
            SCANCODE(0x05, '4');
            SCANCODE(0x06, '5');
            SCANCODE(0x07, '6');
            SCANCODE(0x08, '7');
            SCANCODE(0x09, '8');
            SCANCODE(0x0a, '9');
            SCANCODE(0x0b, '0');
            SCANCODE(0x0c, '-');
            SCANCODE(0x33, ',');
            SCANCODE(0x1c, '\n');
        }
    }
}

uint64_t solve(uint64_t lower, uint64_t upper);

static char output[] = "                    \n";

void kernel_main(void) 
{
    /* Initialize terminal interface */
    term_initialize();

    term_writestring("Welcome to Advent of Code "
                         "Day 2 Part 2 OS!\n\n");

    term_writestring("Please type the input data and hit ENTER:\n");

    uint64_t answer = 0;
    uint64_t current_number = 0;

    for (;;) {
        char c = getchar();
        term_putchar(c);

        uint64_t lower, upper;
        if ('0' <= c && c <= '9') {
            current_number *= 10;
            current_number += c - '0';
        } else if (c == '-') {
            lower = current_number;
            current_number = 0;
        } else {
            upper = current_number;
            current_number = 0;
            answer += solve(lower, upper);
            if (c == '\n') {
                break;
            }
        }
    }

    for (size_t i = sizeof(output) - 3;; --i) {
        output[i] = '0' + answer % 10;
        answer /= 10;
        if (!answer) break;
    }

    term_writestring("\nThe answer is:\n");
    term_writestring(output);
    term_writestring("\nThe kernel is going to stop now.\n"
                     "Thank you for using Advent of Code OS!");
}
