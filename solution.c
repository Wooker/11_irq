#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/irqflags.h>

#define IRQ_NO 8
static int irq = IRQ_NO, my_dev_id, counter = 0;
static struct kobject *my_kobj;

static irqreturn_t my_interrupt(int irq, void *dev_id) {
  counter++;
  printk(KERN_INFO "kernel_mooc %d", counter);
  return IRQ_NONE;
}

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr,
                           char *buf) {
  int len = 0;
  printk(KERN_INFO "kernel_mooc In show");
  len = sprintf(buf, "%d\n", counter);
  return len;
}

static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr,
                            const char *buf, size_t count) {
  return 0;
}

static struct kobj_attribute my_attribute =
    __ATTR(my_sys, 0644, my_sys_show, my_sys_store);

int __init mod_init(void) {
  int retval;
  printk(KERN_DEBUG "kernel_mooc kernel_mooc In init");
  pr_info("kernel_mooc kernel_mooc In init");

  my_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
  if (!my_kobj) {
    pr_err("kernel_mooc kobject_create_and_add failed");
    return -ENOMEM;
  }

  retval = sysfs_create_file(my_kobj, &my_attribute.attr);
  if (retval) {
    pr_err("kernel_mooc sysfs_create_file failed");
    kobject_put(my_kobj);
  }

  if (request_irq(irq, (irq_handler_t) my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id)) {
    pr_err("kernel_mooc request_irq failed");
    kobject_put(my_kobj);
    return -1;
  }

  return retval;
}

void __exit mod_exit(void) {
  kobject_put(my_kobj);
  free_irq(irq, &my_dev_id);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_DESCRIPTION("IRQ");
MODULE_AUTHOR("Zakhar Semenov");
MODULE_LICENSE("GPL");