#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("student");
MODULE_DESCRIPTION("Hello, world kernel module with parameters and lists");
MODULE_LICENSE("Dual BSD/GPL");

static uint hello_count = 1;
module_param(hello_count, uint, 0444);
MODULE_PARM_DESC(hello_count, "How many times to print 'Hello, world!'");

struct event_list {
	struct list_head list;
	ktime_t event_time;
};

static LIST_HEAD(event_list_head);

static int __init hello_init(void)
{
	uint i;
	struct event_list *event;

	if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10)) {
		pr_warn("Warning: hello_count is 0 or between 5 and 10\n");
	} else if (hello_count > 10) {
		pr_err("Error: hello_count is greater than 10. Module will not load.\n");
		return -EINVAL;
	}

	for (i = 0; i < hello_count; i++) {
		event = kmalloc(sizeof(*event), GFP_KERNEL);
		if (!event) {
			pr_err("Memory allocation failed!\n");
			return -ENOMEM;
		}
		
		event->event_time = ktime_get();
		list_add_tail(&event->list, &event_list_head);
		
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct event_list *event, *tmp;

	list_for_each_entry_safe(event, tmp, &event_list_head, list) {
		pr_info("Event time: %lld ns\n", ktime_to_ns(event->event_time));
		list_del(&event->list);
		kfree(event);
	}
	pr_info("Module unloaded successfully.\n");
}

module_init(hello_init);
module_exit(hello_exit);
