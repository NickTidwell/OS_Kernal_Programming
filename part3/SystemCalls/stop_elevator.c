#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

/* System call stub */
long (*STUB_stop_elevator)(void) = NULL;
EXPORT_SYMBOL(STUB_stop_elevator);

/* System call wrapper */


SYSCALL_DEFINE0(stop_elevator) {
if (STUB_stop_elevator != NULL)
	return STUB_stop_elevator();
else
	return -ENOSYS;}
