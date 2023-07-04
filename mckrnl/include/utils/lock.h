#pragma once

#define define_spinlock(name) static int  name = 0

#define atomic_acquire_spinlock(name) while (__sync_lock_test_and_set(&name, 1))
#define atomic_release_spinlock(name) __sync_lock_release(&name)