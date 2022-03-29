#ifndef SINO_THREAD_H
#define SINO_THREAD_H

// pthread_equal() pthread_self() pthread_create() pthread_exit()
// pthread_join() pthread_cancel()
// pthread_cleanup_push() pthread_cleanup_pop() pthread_detach()
//
// pthread_mutex_init() pthread_mutex_destroy()
// pthread_mutex_lock() pthread_mutex_trylock()
// pthread_mutex_unlock() pthread_mutex_timedlock()
//
// pthread_rwlock_init() pthread_rwlock_destroy()
// pthread_rwlock_unlock()
// pthread_rwlock_rdlock() pthread_rwlock_wrlock()
// pthread_rwlock_tryrdlock() pthread_trywrlock()
// pthread_rwlock_timedrdlock() pthread_rwlock_timedwrlock()
//
// pthread_cond_init() pthread_cond_destroy()
// pthread_cond_wait() pthread_cond_timedwait()
// pthread_cond_signal() pthread_cond_broadcast()
//
// pthread_spin_init() pthread_spin_destroy()
// pthread_spin_lock() pthread_spin_trylock()
// pthread_spin_unlock()
//
// pthread_barrier_init() pthread_barrier_destroy()
// pthread_barrier_wait()
//
#include <pthread.h>

namespace THREAD {}

#endif
