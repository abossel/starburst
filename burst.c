#include <stdlib.h>
#include <math.h>
#include "burst.h"

// PI / 180
#define BURST_PI_180 0.01745329f

/*
 * reset a particle to starting value
 */
static void burst_reset_particle(t_emitter *e, int n)
{
    int degrees;
    float radians;

    if (n < 0 || n > e->count)
        return;
    e->particle[n].x = e->x;
    e->particle[n].y = e->y;
    degrees = rand() % 360;
    radians = (float)degrees * BURST_PI_180;
    e->particle[n].dx = e->speed * cos(radians);
    e->particle[n].dy = e->speed * sin(radians);
    e->particle[n].delay = 0.0f;
}

/*
 * allocate a burst data structure
 * speed is pixels per second
 */
t_emitter *burst_create(int count, int width, int height, int x, int y, int speed)
{
    t_emitter *e;
    int i;

    e = malloc(sizeof(t_emitter));
    if (e == NULL)
        return NULL;
    e->particle = malloc(sizeof(t_particle) * count);
    if (e->particle == NULL)
    {
        free(e);
        return NULL;
    }
    e->count = count;
    e->width = width;
    e->height = height;
    e->x = x;
    e->y = y;
    e->gravity = 0;
    e->speed = speed;
    for (i = 0; i < e->count; i++)
        burst_reset_particle(e, i);

    return e;
}

/*
 * free the burst data structure
 */
void burst_free(t_emitter *e)
{
    if (e != NULL)
    {
        free(e->particle);
        free(e);
    }
}

/*
 * set the gravity of the system
 */
void burst_set_gravity(t_emitter *e, int gravity)
{
    e->gravity = gravity;
}

/*
 * set a delay before each particles start.
 * this function must run after a reset
 */
void burst_delay_start(t_emitter *e, float delay)
{
    int i;

    for (i = 0; i < e->count; i++)
        e->particle[i].delay = i * delay;
}

/*
 * get the number of particles in the system
 */
int burst_get_count(t_emitter *e)
{
    return e->count;
}

/*
 * get the x coordinate of a particle as an int
 */
int burst_get_px(t_emitter *e, int n)
{
    if (n < 0 || n > e->count)
        return 0;
    return e->particle[n].x;
}

/*
 * get the y coordinate of a particle as an int
 */
int burst_get_py(t_emitter *e, int n)
{
    if (n < 0 || n > e->count)
        return 0;
    return e->particle[n].y;
}

/*
 * update the position of a particle over delta seconds
 */
void burst_update(t_emitter *e, float delta)
{
    int i;

    for (i = 0; i < e->count; i++)
    {
        if (e->particle[i].delay > 0.0f)
        {
            e->particle[i].delay -= delta;
        }
        else
        {
            e->particle[i].dy += delta * e->gravity;
            e->particle[i].x += delta * e->particle[i].dx;
            e->particle[i].y += delta * e->particle[i].dy;
            if (e->particle[i].y < 0.0f || e->particle[i].y > e->height ||
                e->particle[i].x < 0.0f || e->particle[i].x > e->width)
                burst_reset_particle(e, i);
        }
    }
}
