#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "sparkle.h"

/*
 * convert unicode code point to utf-8 string
 */
static void code_point_to_utf8(char *dst, int code)
{
    if (code >= 0x0000 && code <= 0x007f)
    {
        dst[0] = code & 0x7f;
        dst[1] = '\0';
    }
    if (code >= 0x0080 && code <= 0x07ff)
    {
        dst[0] = (code >> 6) & 0x1f;
        dst[0] |= 0xc0;
        dst[1] = code & 0x3f;
        dst[1] |= 0x80;
        dst[2] = '\0';
    }
    if (code >= 0x0800 && code <= 0xffff)
    {
        dst[0] = (code >> 12) & 0x0f;
        dst[0] |= 0xe0;
        dst[1] = (code >> 6) & 0x3f;
        dst[1] |= 0x80;
        dst[2] = code & 0x3f;
        dst[2] |= 0x80;
        dst[3] = '\0';
    }
    if (code >= 0x010000 && code <= 0x10ffff)
    {
        dst[0] = (code >> 18) & 0x07;
        dst[0] |= 0xf0;
        dst[1] = (code >> 12) & 0x3f;
        dst[1] |= 0x80;
        dst[2] = (code >> 6) & 0x3f;
        dst[2] |= 0x80;
        dst[3] = code & 0x3f;
        dst[3] |= 0x80;
        dst[4] = '\0';
    }
}

/*
 * free a braille null-terminated array and return null
 */
static char **free_braille_nta(char **nta)
{
    int i;

    if (nta != NULL)
    {
        i = 0;
        while (nta[i] != NULL)
        {
            free(nta[i]);
            i++;
        }
        free(nta);
    }

    return NULL;
}

/*
 * generate a null-terminated array of the braille utf-8 characters
 */
static char **generate_braille_utf8()
{
    char **braille;
    int i;

    // allocate the nta
    braille = malloc(sizeof(char *) * 257);
    if (braille == NULL)
        return NULL;
    for (i = 0; i < 256; i++)
    {
        braille[i] = malloc(5);
        if (braille[i] == NULL)
            return free_braille_nta(braille);
        braille[i][0] = '\0';
    }
    braille[i] = NULL;

    // generate braille utf-8 characters U+2800 to U+28FF
    for (i = 0; i < 256; i++)
        code_point_to_utf8(braille[i], 0x2800 | i);

    return braille;
}

/*
 * free the sparkle data structure
 */
void sparkle_free(t_sparkle *s)
{
    if (s != NULL)
    {
        free_braille_nta(s->braille);
        free(s->display);
        free(s->buffer);
        free(s->text.display);
        free(s);
    }
}

/*
 * set every pixel in the display buffer to black
 */
void sparkle_clear(t_sparkle *s)
{
    memset(s->display, 0, s->width * s->height);
}

/*
 * clear all text in the display
 */
void sparkle_clear_text(t_sparkle *s)
{
    memset(s->text.display, 0, s->text.width * s->text.height);
}

/*
 * allocate and initialise the sparkle data structure
 */
t_sparkle *sparkle_create(int width, int height)
{
    t_sparkle *s;

    if (width <= 0 || height <= 0)
        return NULL;
    s = malloc(sizeof(t_sparkle));
    if (s == NULL)
        return NULL;
    s->width = width;
    s->height = height;
    s->braille = generate_braille_utf8();
    s->display = malloc(width * height);
    s->buffer = malloc(width * height / 2);
    s->text.width = width / 2 + (width % 2 != 0);
    s->text.height = height / 4 + (height % 4 != 0);
    s->text.display = malloc(s->text.width * s->text.height);
    if (s->braille == NULL || s->display == NULL || s->buffer == NULL || s->text.display == NULL)
    {
        sparkle_free(s);
        return NULL;
    }
    sparkle_clear(s);
    sparkle_clear_text(s);
    return s;
}

/*
 * get a pixel from the display buffer or 0 if out of range
 */
char sparkle_get_pixel(t_sparkle *s, int x, int y)
{
    if (x < s->width && y < s->height)
        return s->display[x + y * s->width];
    return 0;
}

/*
 * set a pixel int the display buffer
 */
void sparkle_set_pixel(t_sparkle *s, int x, int y, char c)
{
    if (x < s->width && y < s->height)
        s->display[x + y * s->width] = c;
}

/*
 * draw a line on the display buffer
 */
void sparkle_line(t_sparkle *s, int x0, int y0, int x1, int y1, int c)
{
    int dx, dy, sx, sy;
    int error, error2;

    dx = x1 - x0;
    if (dx < 0)
        dx = -dx;
    dy = y1 - y0;
    if (dy > 0)
        dy = -dy;
    sx = x0 < x1 ? 1 : -1;
    sy = y0 < y1 ? 1 : -1;
    error = dx + dy;

    while (1)
    {
        sparkle_set_pixel(s, x0, y0, c);
        if (x0 == x1 && y0 == y1)
            break;
        error2 = 2 * error;
        if (error2 >= dy)
        {
            if (x0 == x1)
                break;
            error = error + dy;
            x0 = x0 + sx;
        }
        if (error2 <= dx)
        {
            if (y0 == y1)
                break;
            error = error + dx;
            y0 = y0 + sy;
        }
    }
}

/*
 * write text to the text buffer at text coordinates
 */
void sparkle_write_text(t_sparkle *s, int x, int y, char *text)
{
    int i;

    if (x < 0 || x >= s->text.width || y < 0 || y >= s->text.height || text == NULL)
        return;
    i = y * s->text.width + x;
    while (x < s->text.width && *text != '\0')
    {
        s->text.display[i] = *text;
        text++;
        x++;
        i++;
    }
}

/*
 * write text to the text buffer at pixel coordinates
 */
void sparkle_write_text_at_pixel(t_sparkle *s, int x, int y, char *text)
{
    if (x < 0 || x >= s->width || y < 0 || y >= s->height || text == NULL)
        return;
    sparkle_write_text(s, x / 2, y / 4, text);
}

/*
 * draw the display buffer on the console
 */
void sparkle_draw(t_sparkle *s)
{
    char *back;
    int size;
    int code;
    int x, y, i;

    // back points to the end of the buffer
    // size is the number of bytes in the buffer
    back = s->buffer;
    size = 0;

    // clear the screen and move cursor to home
    memcpy(back, "\e[2J\e[H", 7);
    back += 7;
    size += 7;

    i = 0;
    for (y = 0; y < s->height; y += 4)
    {
        for (x = 0; x < s->width; x += 2)
        {
            if (s->text.display[i] == '\0')
            {
                // dots on braille chars map to bits in the unicode code point
                code = 0;
                if (sparkle_get_pixel(s, (x + 0), (y + 0))) code |= 0x01;
                if (sparkle_get_pixel(s, (x + 0), (y + 1))) code |= 0x02;
                if (sparkle_get_pixel(s, (x + 0), (y + 2))) code |= 0x04;
                if (sparkle_get_pixel(s, (x + 0), (y + 3))) code |= 0x40;
                if (sparkle_get_pixel(s, (x + 1), (y + 0))) code |= 0x08;
                if (sparkle_get_pixel(s, (x + 1), (y + 1))) code |= 0x10;
                if (sparkle_get_pixel(s, (x + 1), (y + 2))) code |= 0x20;
                if (sparkle_get_pixel(s, (x + 1), (y + 3))) code |= 0x80;
                // all braille utf-8 chars are 3 bytes
                memcpy(back, s->braille[code], 3);
                back += 3;
                size += 3;
            }
            else
            {
                *back = s->text.display[i];
                back += 1;
                size += 1;
            }
            i += 1;
        }
        *back = '\n';
        back += 1;
        size += 1;
    }

    // HACK prevent warning on gcc
    if (write(STDOUT_FILENO, s->buffer, size) == 0) { }
}
