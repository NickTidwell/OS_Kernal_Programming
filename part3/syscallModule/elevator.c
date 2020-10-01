#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/sched.h>

MODULE_LICENSE("DUAL BSD/GPL");

void addToQueue(int, int, int);
void print_elevator(void);
void change_floor(int);

#define NUM_FLOORS 10

//Elevator States
#define OFFLINE 0
#define IDLE 1
#define LOADING 2
#define UP 3
#define DOWN 4

#define UNINFECTED 0;
#define INFECTED 1;

#define NUM_FLOOR 10;
#define BUF_LEN 100

static struct proc_dir_entry* proc_entry;
static char msg[BUF_LEN];
static int procfs_buf_len;

int curr_floor;
int next_floor;
int elevator_state;
// int elevator_direction;
int num_passenger;
int passenger_waiting;
int	infected_status;
int passenger_served;
int stop_signal = 0;
typedef struct queue_member {

	struct list_head list;
	int type;
	int start_floor;
	int dest_floot;
} queue_member;


struct task_struct *elevator_thread;

struct list_head passenger_queue[NUM_FLOORS]; //queue of passenger
struct list_head elevator_list; //queue of elevator


static int run_elevator(void *data){

	while(!kthread_should_stop()){
		ssleep(1);
		switch(elevator_state){
			case OFFLINE:
				printk(KERN_INFO "Elevator OFFLINE");
				break;
			case IDLE:
				printk(KERN_INFO "Elevator IDLE");
				//Check to Unload
				elevator_state = UP;
				next_floor = curr_floor + 1;
				break;
			case UP:
				change_floor(next_floor);
				printk(KERN_INFO "Elevator UP");
				if(curr_floor == 10){
					elevator_state = DOWN;
					next_floor = curr_floor - 1;
				}
				else next_floor = curr_floor + 1;
				break;
			case DOWN:
				change_floor(next_floor);
				printk(KERN_INFO "Elevator Down");
				if(curr_floor == 1){
					elevator_state = UP;
					next_floor = curr_floor + 1;
				}
				else next_floor = curr_floor - 1;

				if(stop_signal == 1 && curr_floor == 1) //TODO: Add Check to make sure no one is left to serve in the future
				{
					elevator_state = OFFLINE;
					stop_signal = 0;
				}
				break;
			case LOADING:
				printk(KERN_INFO "Elevator Loading");
				break;
		}
	}
	printk(KERN_INFO "Thread Stopping\n");
	return 0;
}

void change_floor(int floor){

	if(floor != curr_floor){
		ssleep(2);
		curr_floor = floor;
	}
}

static ssize_t elevator_read(struct file* file, char * ubuf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "Elevator State: %d\n Elevator Satus: %d\n Current Floor: %d\n  Number of Passengers: %d\n Number or Passengers Waiting: %d\n Number Passengers Serviced: %d\n", elevator_state, infected_status, curr_floor, num_passenger, passenger_waiting, passenger_served);
	print_elevator();
	procfs_buf_len = strlen(msg);

	if (*ppos > 0 || count < procfs_buf_len)
		return 0;
	if (copy_to_user(ubuf, msg, procfs_buf_len))
		return -EFAULT;
	
	*ppos = procfs_buf_len;
	return procfs_buf_len;

}
static ssize_t elevator_write(struct file* file, const char * ubuf, size_t count, loff_t* ppos)
{
	//Elevator Write is Not needed for this project...
	printk(KERN_INFO "elevator proc_write test\n");
	if (count > BUF_LEN)
		procfs_buf_len = BUF_LEN;
	else
		procfs_buf_len = count;
	
	copy_from_user(msg, ubuf, procfs_buf_len);
	printk(KERN_INFO "got from user: %s\n", msg);
	return procfs_buf_len;
}
static struct file_operations procfile_fops = {
	.owner = THIS_MODULE,
	.read = elevator_read,
	.write = elevator_write,
};
extern long (*STUB_start_elevator)(void);
long start_elevator(void) {
	if(elevator_state == OFFLINE){
		printk(KERN_INFO "Starting Elevator\n");
		elevator_state = IDLE;
		return 1;
	}
	return 0;
}

extern long (*STUB_issue_request)(int,int,int);
long issue_request(int start, int stop, int type) {
	
	printk("Issue Request: start = %d, stop = %d, type = %d\n", start, stop, type);

	if(start == stop){
		//We are already at the desination floor dummy
	}
	else{
		addToQueue(start, stop, type);
	}
	return 1;

}

extern long (*STUB_stop_elevator)(void);
long stop_elevator(void) {
	printk("Stop Request");
	if(stop_signal == 0){
		stop_signal = 1;
		return 0;
	}
	return 1;
}

static int elevator_init(void) {
	printk(KERN_ALERT "Elevator INIT\n");

	STUB_start_elevator = start_elevator;
	STUB_stop_elevator = stop_elevator;
	STUB_issue_request = issue_request;

	//Initialize queue on each floor
	int i = 0;
	while (i < NUM_FLOORS) 
	{
    	INIT_LIST_HEAD(&passenger_queue[i]);
    	i++;
  	}
  	INIT_LIST_HEAD(&elevator_list);

	int curr_floor = 1;
	int next_floor = 1;
	int elevator_state = OFFLINE;
	int elevator_next_dir = UP;
	int num_passenger = 0;
	int	infected_status = UNINFECTED;
	int passenger_served = 0;
	passenger_waiting = 0;

	printk(KERN_ALERT "Elevator Thread Cretae\n");

	//Initialize Elevator Thread
	 elevator_thread = kthread_run(run_elevator, NULL, "mythread");
    // if(IS_ERR(elevator_thread)) {
    //   printk("Error: ElevatorRun\n");
    //   return PTR_ERR(elevator_thread);
    // }
	// else
	// 	printk(KERN_INFO "Thread Created Successfully\n");c


	//Initialize Proc Entry
	proc_entry = proc_create("elevator", 0666, NULL, &procfile_fops);

	if(proc_entry == NULL)
	{	
		 return -ENOMEM;
	}

	return 0;
}

static void elevator_exit(void) {
	STUB_start_elevator = NULL;
	STUB_stop_elevator = NULL;
	STUB_issue_request = NULL;
	printk(KERN_ALERT "Elevator EXIT\n");
	if(elevator_thread)
		kthread_stop(elevator_thread);
	proc_remove(proc_entry);
	return;
}

void addToQueue(int start, int stop, int type){
	struct queue_member *new_member;
	new_member = kmalloc(sizeof(struct queue_member), __GFP_RECLAIM);
	new_member->start_floor = start;
	new_member->dest_floot = stop;
	new_member->type = type;
	list_add_tail(&new_member->list, &passenger_queue[start-1]);
}

void print_elevator()
{
	struct list_head *qPos;
	struct queue_member *qMember;
	int i = 0;
	int queuePos = 0;
	while(i < NUM_FLOORS){

		printk("Floor: %d\n", i+1);
		list_for_each(qPos, &passenger_queue[i]){
			qMember = list_entry(qPos, queue_member, list);
			printk("Queue pos: %d\nType: %d\nStart Floor: %d\nDest Floor: %d\n", queuePos, qMember->type, qMember->start_floor, qMember->dest_floot);
      		++queuePos;
		}
		i++;
	}
}


module_init(elevator_init);
module_exit(elevator_exit);



