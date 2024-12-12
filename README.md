#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");

#define PROC_HIDDEN "hidden"
#define PROC_HIDDEN_PROCESS "hidden_process"

static int hidden_flag = 0; // 全局隐藏标志

// 定义隐藏进程的结构
struct hidden_process {
    pid_t pid;
    struct list_head list;
};
static LIST_HEAD(hidden_processes);

// 读取 /proc/hidden 的函数
static ssize_t hidden_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos) {
    char flag_str[2];
    snprintf(flag_str, sizeof(flag_str), "%d\n", hidden_flag);
    return simple_read_from_buffer(buffer, count, ppos, flag_str, strlen(flag_str));
}

// 写入 /proc/hidden 的函数
static ssize_t hidden_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos) {
    char flag_str[2];
    if (copy_from_user(flag_str, buffer, min(count, sizeof(flag_str) - 1)))
        return -EFAULT;

    if (flag_str[0] == '1') {
        hidden_flag = 1;
    } else if (flag_str[0] == '0') {
        hidden_flag = 0;
    } else {
        return -EINVAL;
    }

    return count;
}

// 文件操作结构体 for /proc/hidden
static const struct proc_ops hidden_ops = {
    .proc_read = hidden_read,
    .proc_write = hidden_write,
};

// 读取 /proc/hidden_process 的函数
static ssize_t hidden_process_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos) {
    char *buf;
    size_t len = 0;
    struct hidden_process *hp;

    // 分配临时缓冲区
    buf = kzalloc(1024, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;

    // 遍历隐藏进程链表
    list_for_each_entry(hp, &hidden_processes, list) {
        len += snprintf(buf + len, 1024 - len, "%d ", hp->pid);
        if (len >= 1024) break; // 防止缓冲区溢出
    }
    len += snprintf(buf + len, 1024 - len, "\n");

    // 将数据复制到用户空间
    len = simple_read_from_buffer(buffer, count, ppos, buf, len);
    kfree(buf);
    return len;
}

// 文件操作结构体 for /proc/hidden_process
static const struct proc_ops hidden_process_ops = {
    .proc_read = hidden_process_read,
};

// 模块初始化函数
static int __init hidden_init(void) {
    struct proc_dir_entry *entry;

    // 创建 /proc/hidden 文件
    entry = proc_create(PROC_HIDDEN, 0666, NULL, &hidden_ops);
    if (!entry)
        return -ENOMEM;

    // 创建 /proc/hidden_process 文件
    entry = proc_create(PROC_HIDDEN_PROCESS, 0444, NULL, &hidden_process_ops);
    if (!entry) {
        remove_proc_entry(PROC_HIDDEN, NULL);
        return -ENOMEM;
    }

    printk(KERN_INFO "Hidden module loaded.\n");
    return 0;
}

// 模块卸载函数
static void __exit hidden_exit(void) {
    struct hidden_process *hp, *tmp;

    // 删除隐藏进程链表
    list_for_each_entry_safe(hp, tmp, &hidden_processes, list) {
        list_del(&hp->list);
        kfree(hp);
    }

    // 删除 /proc 文件
    remove_proc_entry(PROC_HIDDEN, NULL);
    remove_proc_entry(PROC_HIDDEN_PROCESS, NULL);

    printk(KERN_INFO "Hidden module unloaded.\n");
}

// 内核模块入口和出口
module_init(hidden_init);
module_exit(hidden_exit);
