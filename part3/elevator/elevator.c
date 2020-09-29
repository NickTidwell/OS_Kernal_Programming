#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/mutex.h>

MODULE_LICENSE("DUAL BSD/GPL");

#define BUF_LEN 100


//Elevator States
#define OFFLINE 0
#define IDLE 1
#define LOADING 2
#define UP 3
#define DOWN 4

#define UNINFECTED 0;
#define INFECTED 1;

#define NUM_FLOOR 10;
static struct proc_dir_entry* proc_entry;
static char msg[BUF_LEN];
static int procfs_buf_len;

int curr_floor;
int next_floor;
int elevator_state;
int elevator_next_dir;
int num_passenger;
int passenger_waiting;
int	infected_status;
int passenger_served;

static ssize_t elevator_read(struct file* file, char * ubuf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "Elevator State: %d\n Elevator Satus: %d\n Current Floor: %d\n  Number of Passengers: %d\n Number or Passengers Waiting: %d\n Number Passengers Serviced: %d\n", elevator_state, elevator_state, curr_floor, num_passenger, passenger_waiting, passenger_served);
	
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

static int elevator_init(void)
{
	int curr_floor = 1;
	int next_floor = 1;
	int elevator_state = OFFLINE;
	int elevator_next_dir = UP;
	int num_passenger = 0;
	int	infected_status = UNINFECTED;
	int passenger_served = 0;
	passenger_waiting = 0;
	printk(KERN_ALERT "Elevator INIT\n");
	proc_entry = proc_create("elevator", 0666, NULL, &procfile_fops);

	if(proc_entry == NULL)
	{	
		 return -ENOMEM;
	}

	//elevator_sys_init();
	return 0;
}

static void elevator_exit(void)
{
	printk(KERN_ALERT "Elevator EXIT\n");
	proc_remove(proc_entry);
	return;

}

module_init(elevator_init);
module_exit(elevator_exit);
