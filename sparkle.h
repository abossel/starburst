#ifndef SPARKLE_H
#define SPARKLE_H

typedef struct s_sparkle
{
    char **braille;
    char *display;
    char *buffer;
    int width;
    int height;
} t_sparkle;

t_sparkle *sparkle_create(int width, int height);
void sparkle_free(t_sparkle *s);
void sparkle_clear(t_sparkle *s);
char sparkle_get_pixel(t_sparkle *s, int x, int y);
void sparkle_set_pixel(t_sparkle *s, int x, int y, char c);
void sparkle_line(t_sparkle *s, int x0, int y0, int x1, int y1, int c);
void sparkle_draw(t_sparkle *s);

#endif
