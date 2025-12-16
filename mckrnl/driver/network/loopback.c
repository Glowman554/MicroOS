#include <driver/network/loopback.h>

bool loopback_is_device_present(driver_t* driver) {
    return true;
}

char* loopback_get_device_name(driver_t* driver) {
    return "loopback";
}

void loopback_init(driver_t* driver) {
    nic_driver_t* ndriver = (nic_driver_t*) driver;
    ndriver->mac.mac = 0;

    register_nic_driver(ndriver);
}

void loopback_send(nic_driver_t* driver, async_t* async, uint8_t* data, uint32_t size) {
    switch (async->state) {
        case STATE_INIT:
            driver->recv(driver, data, size);
            async->state = STATE_DONE;
            break;
    }
}

void loopback_stack(nic_driver_t* driver, void* stack) {}


nic_driver_t loopback_driver = {
    .driver = {
        .get_device_name = loopback_get_device_name,
        .is_device_present = loopback_is_device_present,
        .init = loopback_init
    },
    .send = loopback_send,
    .stack = loopback_stack
};
