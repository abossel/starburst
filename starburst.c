#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "sparkle.h"
#include "burst.h"

#define WIDTH 158
#define HEIGHT 96

int main()
{
    t_emitter *st;
    t_sparkle *sp;
    int x, y, i;
    int count;

    srand(time(NULL));

    sp = sparkle_create(WIDTH, HEIGHT);
    st = burst_create(60, WIDTH, HEIGHT, WIDTH / 2, HEIGHT / 2, 40);
    if (sp == NULL || st == NULL)
    {
        sparkle_free(sp);
        burst_free(st);
        return 1;
    }

    count = burst_get_count(st);
    // burst_set_gravity(st, 30);
    // burst_delay_start(st, 0.1f);

    sparkle_write_text(sp, 5, 3, "Starburst");
    sparkle_write_text(sp, WIDTH / 2 - 1, HEIGHT / 4, "");

    while(1)
    {
        sparkle_clear(sp);

        for (i = 0; i < count; i++)
        {
            x = burst_get_px(st, i);
            y = burst_get_py(st, i);
            sparkle_set_pixel(sp, x, y, 1);
        }
        sparkle_draw(sp);
        // run animation at 10 fps
        usleep(100000);
        burst_update(st, 0.1f);
    }

    sparkle_free(sp);
    burst_free(st);

    return 0;
}
