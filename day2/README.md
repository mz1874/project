## Task creation and using
* In Day two, I will show how to use different way to create tasks and , how to use some of functions.

---
### 1- thread_creation.c

**Short description**

This task create two task by different way , static and dynamic. by using the functions of **rt_thread_init**
and **rt_thread_create**

<span style="color: red;">PS: Do not leave a while() to the end of main function scope. 
Otherwise, the task will never be scheduled</span>

```c
int main(void) {
    err = rt_thread_init(&thread1,
                         "staticTask",
                         staticTask,
                         RT_NULL,
                         static_stack,
                         sizeof(static_stack),
                         25, 5);
    if (err == RT_EOK) {
        rt_thread_startup(&thread1);
    }

    thread2 = rt_thread_create("dynamicTask", dynamicTask, RT_NULL, 512, 30, 20);
    rt_thread_startup(thread2);
    while (1) {

    }
}

```

The correct way is below:

```c
int main(void) {
    err = rt_thread_init(&thread1,
                         "staticTask",
                         staticTask,
                         RT_NULL,
                         static_stack,
                         sizeof(static_stack),
                         25, 5);
    if (err == RT_EOK) {
        rt_thread_startup(&thread1);
    }

    thread2 = rt_thread_create("dynamicTask", dynamicTask, RT_NULL, 512, 30, 20);
    rt_thread_startup(thread2);
    return 0;
}
```

Effect like below (Correct Way ):

![Effect.jpg](..%2Fass%2Fday2%2FEffect.jpg)

InCorrect Way:

![incorrect.jpg](..%2Fass%2Fday2%2Fincorrect.jpg)


### 2- main.c at /application-day2

This C code demonstrate how to delete a task by using functions **rt_thread_detach** and **rt_thread_delete**
**rt_thread_detach** is used to delete static task. **rt_thread_delete** can not delete static task


**Delete dynamic task by using rt_thread_delete()**
```c
/**
 * This task will be crated dynamically and deleted by rt_thread_delete once i == 10
 * @param parameter
 */
void dynamicTask(void * parameter)
{
    int i =0;
    while(1){
        if(i==10){
            rt_kprintf("Deleted dynamicTask Task\r\n");
            rt_thread_t t = rt_thread_self();
            rt_thread_delete(t);
        }
        rt_kprintf("This is a dynamicTask Task\r\n");
        i++;
        rt_thread_delay(500);
    }
}
```

**Delete dynamic task by using rt_thread_detach()**

```c
/**
 * This task will  deleted by rt_thread_detach once i == 10
 * @param parameter
 */
static void staticTask(void * parameter)
{
    int i =0;
    while(1){
        if(i==10){
            rt_kprintf("Deleted staticTask Task\r\n");
            rt_thread_t t = rt_thread_self();
            rt_thread_detach(t);
        }
        rt_kprintf("This is a staticTask Task\r\n");
        i++;
        rt_thread_delay(500);
    }
}

```

Effect:

![Effect.jpg](..%2Fass%2Fday2%2FEffect.jpg)