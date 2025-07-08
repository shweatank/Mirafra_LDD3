#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kthread.h>
#include<linux/delay.h>
#include<linux/init.h>


#define NUM_THREAD 3

MODULE_LICENSE("GPL");
MODULE_AUTHOR("REV");
MODULE_DESCRIPTION("Multiple kernel threads examples");

static struct task_struct *threads[NUM_THREAD];

struct my_thread_info{
	int id;
};

static int thread_fn(void *data){
	struct my_thread_info *info =(struct my_thread_info*)data;
	int count=0;
	pr_info("kthread :Thread %d started (PID=%d)\n",info->id,current->pid);
	while(!kthread_should_stop()){
		pr_info("kthread:thread %d running...count=%d\n",info->id,count++);
		ssleep(1);
	}
	pr_info("kthread :thread %d exiting\n",info->id);
	kfree(info);
	return 0;
}

static int __init multi_kthread_init(void){
	int i;
	pr_info("kthread:module loading with %d threads\n",NUM_THREAD);
	for(i=0;i<NUM_THREAD;i++){
		struct my_thread_info*info;
		info=kmalloc(sizeof(*info),GFP_KERNEL);
		if(!info)
			return -ENOMEM;
		info->id=i;
		threads[i]=kthread_run(thread_fn,info,"kthread__%d",i);
		if(IS_ERR(threads[i])){
			pr_err("Kthread:failed to create thread %d\n",i);
			kfree(info);
			threads[i]=NULL;
		}
	}
	return 0;
}

static void __exit multi_kthread_exit(void){
	int i;
	pr_info("kthread module unloading\n");
	for(i=0;i<NUM_THREAD;i++){
		if(threads[i]){
			kthread_stop(threads[i]);
			pr_info("kthread:thread %d stopped \n",i);
		}
	}
	pr_info("kthread:all threads exited\n");
}

module_init(multi_kthread_init);
module_exit(multi_kthread_exit);
