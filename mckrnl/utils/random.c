#include <utils/random.h>

#include <driver/timer_driver.h>

uint32_t rng_state = 0;

uint32_t rng_lazy_seed(void) {
    uint32_t t = (uint32_t)global_timer_driver->time_ms(global_timer_driver) + 123456789;
    t ^= 0xA5A5A5A5u;

    return t;
}

uint32_t rng(void) {
    if (rng_state == 0) {
        rng_state = rng_lazy_seed();
    }

    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

uint32_t rng_uniform(uint32_t bound) {
    if (bound == 0) {
        return 0;
    }
    return rng() % bound;
}

bool rng_chance(uint32_t percent) {
    return rng_uniform(100) < percent;
}
