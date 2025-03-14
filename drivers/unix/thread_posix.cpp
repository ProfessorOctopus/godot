//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#if defined(UNIX_ENABLED)
#include "thread_posix.h"
#include "core/os/thread.h"
#include "core/string/ustring.h"
#ifdef PTHREAD_BSD_SET_NAME
#include <pthread_np.h>
#endif

static Error set_name(const String &p_name) {
#ifdef PTHREAD_NO_RENAME
	return ERR_UNAVAILABLE;
#else
#ifdef PTHREAD_RENAME_SELF
	// check if thread is the same as caller
	int err = pthread_setname_np(p_name.utf8().get_data());
#else
	pthread_t running_thread = pthread_self();
#ifdef PTHREAD_BSD_SET_NAME
	pthread_set_name_np(running_thread, p_name.utf8().get_data());
	int err = 0; // Open/FreeBSD ignore errors in this function
#elif defined(PTHREAD_NETBSD_SET_NAME)
	int err = pthread_setname_np(running_thread, "%s", const_cast<char *>(p_name.utf8().get_data()));
#else
	int err = pthread_setname_np(running_thread, p_name.utf8().get_data());
#endif // PTHREAD_BSD_SET_NAME
#endif // PTHREAD_RENAME_SELF
	return err == 0 ? OK : ERR_INVALID_PARAMETER;
#endif // PTHREAD_NO_RENAME
}

void init_thread_posix() {
	Thread::_set_platform_functions({ .set_name = set_name });
}

#endif // UNIX_ENABLED
