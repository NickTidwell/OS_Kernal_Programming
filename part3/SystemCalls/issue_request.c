#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

/* System call stub */
long (*STUB_issue_request)(int,int,int) = NULL;
EXPORT_SYMBOL(STUB_issue_request);

/* System call wrapper */


SYSCALL_DEFINE3(issue_request, int, start, int, stop, int, type) {
if (STUB_issue_request != NULL)
	return STUB_issue_request(start, stop, type);
else
	return -ENOSYS;}
