#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

static char glob_buf[10000] = {0};

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

static ssize_t read_dev(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    struct task_struct* task_list;
    int ec = 0;

    for_each_process(task_list) {
        printk(KERN_INFO "PID: %d, PPID: %d, CPU: %d, STATE: %ld\n", task_list->pid, task_list->parent->pid, task_cpu(task_list), task_list->state);
        sprintf(glob_buf + strlen(glob_buf), "PID %d, PPID %d CPU: %d STATE %ld\n", task_list->pid, task_list->parent->pid, task_cpu(task_list), task_list->state);
    }

    ec = copy_to_user(buf, glob_buf, strlen(glob_buf));
    return ec;
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

    pr_info("I'm in\n");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&sample_device);
    pr_info("I'm out\n");
}
