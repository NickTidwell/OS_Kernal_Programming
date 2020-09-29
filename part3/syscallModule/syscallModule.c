#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/linkage.h>

MODULE_LICENSE("GPL");
extern long (*STUB_start_elevator)(void);
long start_elevator(void) {

	//Implement Start Elevator Below
	return 1;
}

extern long (*STUB_issue_request)(int,int,int);
long issue_request(int start, int stop, int type) {
	
	printk("Issue Request: start = %d, stop = %d, type = %d\n", start, stop, type);

	return 1;

}

extern long (*STUB_stop_elevator)(void);
long stop_elevator(void) {
	//Implement Stop Elevator Below
	return 1;
}

static int elevator_sys_init(void) {
	STUB_start_elevator = start_elevator;
	STUB_stop_elevator = stop_elevator;
	STUB_issue_request = issue_request;
	return 0;
}


static void elevator_exit(void) {
	STUB_start_elevator = NULL;
	STUB_stop_elevator = NULL;
	STUB_issue_request = NULL;
}

module_init(elevator_sys_init);
module_exit(elevator_exit);



