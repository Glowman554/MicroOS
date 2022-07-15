#include <driver/clock_driver.h>

#include <stddef.h>
#include <utils/unix_time.h>

clock_driver_t* global_clock_driver = NULL;

long long time(clock_driver_t* driver) {
	clock_result_t result = driver->get_time(driver);
	return to_unix_time(result.year, result.month, result.day, result.hours, result.minutes, result.seconds);
}
