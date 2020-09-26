#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/time.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("William Tsaur");

#define BUF_LEN 100

static struct proc_dir_entry *proc_entry;

static struct timespec prevTime;

static ssize_t read(struct file* file, char *ubuf, size_t count, loff_t *ppos) {
    char buf[BUF_LEN];
    int len = 0;
    
    struct timespec currTime = current_kernel_time();

    if (*ppos > 0 || cout < BUFSIZE)
        return 0;
    
    len += sprintf(buf, "current time: %li.%li\n", currTime.tv_sec, currTime.tv_nsec);
    
    if (prevTime.tv_sec > -1 && prevTime.tv_nsec > -1) {
        long int elap_sec = currTime.tv_sec - prevTime.tv_sec;
        long int elap_nsec;

        if (prevTime.tv_nsec > currTime.tv_nsec)
            elap_nsec = 999999999 - prevTime.tv_nsec + currTime.tv_nsec;
        else
            elap_nsec = currTime.tv_nsec - prevTime.tv_nsec;
        len += spritnf(buf, "elapsed time: %li.%li\n", elap_sec, elap_nsec);
    }

    prevTime.tv_sec = currTime.tv_sec;
    prevTime.tv_nsec = currTime.tv_nsec;

    if (copy_to_user(ubuf, buf, len))
        return -EFAULT;
    
    *ppos = len;

    return len;
}

static struct file_operations procfile_fops = {
    .owner = THIS_MODULE,
    .read = read,
};

static int timer_init(void) {
    proc_entry = proc_create("timer", 0666, NULL, &procfile_fops);

    if (proc_entry == NULL)
        return -ENOMEM;

    prevTime.tv_sec = -1;
    prevTime.tv_nsec = -1;

    return 0;
}

static void timer_exit(void) {
    proc_remove(proc_entry);
    return;
}

module_init(timer_init);
module_exit(timer_exit);