#ifndef TOOLS_H_
    #define TOOLS_H_

    #include <stdint.h>

typedef uint8_t byte;
typedef uint8_t reg8;
typedef uint16_t reg16;
typedef uint8_t n8_t;
typedef uint16_t n16_t;
typedef uint8_t a8_t;
typedef uint16_t a16_t;
typedef int8_t e8_t;
typedef uint16_t color_pixel;

typedef enum {
    READ = 0,
    WRITE
} io_operation;

#endif
