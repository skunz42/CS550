#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define BUFFER_SIZE 10000

static int open_dev(struct inode *inode, struct file *file)
{
    pr_info("Opening device...\n");
    return 0;
}

static int close_dev(struct inode *inodep, struct file *filp)
{
    pr_info("Closing device...\n");
    return 0;
}

char * get_state(long int state_num) {
   switch (state_num) {
       case 0x0:
            return "TASK_RUNNING";
       case 0x1:
            return "TASK_INTERRUPTIBLE";
       case 0x2:
            return "TASK_UNINTERRUPTIBLE";
       case 0x4:
            return "__TASK_STOPPED";
       case 0x8:
            return "__TASK_TRACED";
       case 0x10:
            return "EXIT_DEAD";
       case 0x20:
            return "EXIT_ZOMBIE";
       case 0x40:
            return "TASK_PARKED";
       case 0x80:
            return "TASK_DEAD";
       case 0x100:
            return "TASK_WAKEKILL";
       case 0x200:
            return "TASK_WAKING";
       case 0x400:
            return "TASK_NOLOAD";
       case 0x800:
            return "TASK_NEW";
       case 0x1000:
            return "TASK_STATE_MAX";
       case (0x20 | 0x10):
            return "EXIT_ZOMBIE, EXIT_DEAD";
       case (0x100 | 0x2):
            return "TASK_WAKEKILL, TASK_UNINTERRUPTIBLE";
       case (0x100 | 0x4):
            return "TASK_WAKEKILL, __TASK_STOPPED";
       case (0x100 | 0x8):
            return "TASK_WAKEKILL, __TASK_TRACED";
       case (0x2 | 0x400):
            return "TASK_UNINTERRUPTIBLE, TASK_NOLOAD";
       case (0x2 | 0x1):
            return "TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE";
       case (0x0 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40):
            return "TASK_RUNNING, TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE, TASK_STOPPED, __TASK_TRACED, EXIT_DEAD, EXIT_ZOMBIE, TASK_PARKED";
       default:
            printk(KERN_ALERT "%ld\n", state_num);
            return "UNKNOWN_STATE";
   }
}

static ssize_t read_dev(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    struct task_struct* task_list;
    char glob_buf[BUFFER_SIZE];
    int ret = 0;
    char * task_state;

    for_each_process(task_list) {
        task_state = get_state(task_list->state);
        printk(KERN_INFO "PID=%d, PPID=%d, CPU=%d, STATE=%s\n", task_list->pid, task_list->parent->pid, task_cpu(task_list), task_state);
        sprintf(glob_buf + strlen(glob_buf), "PID=%d PPID=%d CPU=%d STATE=%s\n", task_list->pid, task_list->parent->pid, task_cpu(task_list), task_state);
    }
    copy_to_user(buf, glob_buf, strlen(glob_buf)+1);
    return ret;
}

static const struct file_operations sample_fops = {
    .owner			= THIS_MODULE,
    .read           = read_dev,
    .open			= open_dev,
    .release		= close_dev,
    .llseek 		= no_llseek,
};

struct miscdevice sample_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "process_list",
    .fops = &sample_fops,
};

static int __init misc_init(void)
{
    int error;

    error = misc_register(&sample_device);
    if (error) {
        pr_err("can't misc_register :(\n");
        return error;
    }

    pr_info("Initializing process list\n");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&sample_device);
    pr_info("I'm out\n");
}

module_init(misc_init);
module_exit(misc_exit);
