#ifndef BURST_H
#define BURST_H

typedef struct s_particle
{
    float x;
    float y;
    float dx;
    float dy;
    float delay;
} t_particle;

typedef struct s_emitter
{
    t_particle *particle;
    int count;
    int width;
    int height;
    int gravity;
    int speed;
    int x;
    int y;
} t_emitter;

t_emitter *burst_create(int count, int width, int height, int x, int y, int speed);
void burst_free(t_emitter *s);

void burst_delay_start(t_emitter *s, float delay);
void burst_set_gravity(t_emitter *s, int gravity);

int burst_get_count(t_emitter *s);
int burst_get_px(t_emitter *s, int n);
int burst_get_py(t_emitter *s, int n);

void burst_update(t_emitter *s, float delta);

#endif
