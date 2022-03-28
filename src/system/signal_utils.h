#ifndef SIGNAL_H
#define SIGNAL_H
#include "general/inc_exception.h"

// int sigaction(int __sig, const struct sigaction *__restrict __act, struct sigaction *__restrict __oact);
#include <signal.h>

void registerSigHandler() {
	struct sigaction tmp;
}



#endif
