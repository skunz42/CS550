#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "proc_struct.h"

MODULE_LICENSE("Dual BSD/GPL");

proc_node *head = NULL;
proc_node *read_runner = NULL;

void insert_proc(proc_node *entry);
void print_list(void);
void free_list(void);

void insert_proc(proc_node *entry) {
    if (head == NULL) {
        head = entry;
        read_runner = head;
    } else {
        proc_node *runner = head;
        while (runner != NULL) {
            if (runner->next == NULL) {
                runner->next = entry;
                break;
            }
            runner = runner->next;
        }
    }
}

void print_list(void) {
    if (head != NULL) {
        proc_node *runner = head;
        while (runner != NULL) {
            pr_info("%d\n", runner->pid);
            runner = runner->next;
        }
    }
}

void free_list(void) {
    proc_node *runner = head;
    proc_node *next;
    while (runner != NULL) {
        next = runner->next;
        kfree(runner);
        runner = next;
    }
    head = NULL;
}

static int open_dev(struct inode *inode, struct file *file)
{
    struct task_struct* task_list;
    pr_info("Opening device...\n");

    for_each_process(task_list) {
        proc_node *entry = (proc_node*)kmalloc(BUFFER_SIZE, GFP_KERNEL);
        entry->pid = task_list->pid;
        entry->ppid = task_list->parent->pid;
        entry->cpu = task_cpu(task_list);
        entry->state = task_list->state;
        entry->next = NULL;
        printk(KERN_INFO "PID=%d, PPID=%d, CPU=%d, STATE=%ld\n", entry->pid, entry->ppid, entry->cpu, entry->state);
        insert_proc(entry);
    }
    return 0;
}

static int close_dev(struct inode *inodep, struct file *filp)
{
    pr_info("Closing device...\n");
    free_list();
    return 0;
}

static ssize_t read_dev(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    char *glob_buf;
    int ec;
    if (len < BUFFER_SIZE) {
        pr_info("need to request more bytes\n");
        return -1;
    }
    if (read_runner == NULL) {
        pr_info("got to end of list\n");
        read_runner = head;
        return 0;
    }
    glob_buf = (char *)(read_runner);
    ec = copy_to_user(buf, glob_buf, BUFFER_SIZE);
    if (ec != 0) {
        pr_info("errors on copy_to_user\n");
        return -1;
    }
    read_runner = read_runner->next;
    return BUFFER_SIZE;
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
