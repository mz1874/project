# 线程间同步与线程间通信

* 本章节中我将会阐述如何实现在线程间同步和线程间如何通信

## 线程间同步

线程间同步在RT-Thread中存在三种方式，分别是

### 1- 信号量 (semaphore)
信号量是一种轻型的用于解决线程间同步问题的内核对象，线程可以获取或释放它，从而达到同步或互斥的目的。
信号量工作示意图如下图所示，每个信号量对象都有一个信号量值和一个线程等待队列，
信号量的值对应了信号量对象的实例数目、资源数目，假如信号量值为 5，
则表示共有 5 个信号量实例（资源）可以被使用，当信号量实例数目为零时，
再申请该信号量的线程就会被挂起在该信号量的等待队列上，等待可用的信号量实例（资源）。

![06sem_ops.png](..%2Fass%2Fday3%2F06sem_ops.png)

信号量的创建一共有两种方式，分别是静态创建以及动态分配通过调用下面的Functions

**静态创建**

```c
 rt_sem_t rt_sem_create(const char *name,
                        rt_uint32_t value,
                        rt_uint8_t flag);
```
**动态创建**

```c
rt_err_t rt_sem_init(rt_sem_t       sem,
                    const char     *name,
                    rt_uint32_t    value,
                    rt_uint8_t     flag)
```

<p style="color: red">需要注意的是，静态创建的任何的对象只可以调用，detach 方法进行脱离，脱离之后，内存并没有被释放。而动态创建的则会被自动
回收</p>

同时我们注意到，在上述创建信号量的时候有一个Flag参数，这个flag决定了Task获取信号量的方式。 这个参数接收两个参数

```c
RT_IPC_FLAG_FIFO 先进先出， 谁先第一个尝试获取信号量，则优先分配给谁
RT_IPC_FLAG_PRIO 基于优先级， 基于线程优先级的抢占式调度
```

如果这个信号量的值设置为1，那么其实等同于互斥锁。用户可以通过调用下面的Function来获取和释放信号量

**尝试获取信号量，如果当前无法获取到信号量则等待固定事件**

```c
rt_err_t rt_sem_take (rt_sem_t sem, rt_int32_t time);
```

**尝试立即获取信号量**
```c
rt_err_t rt_sem_trytake(rt_sem_t sem);
```

那么如果信号量设置的值大于等于1的话，那么实际上表明的则为当前资源的可用数量, 让我们举一个例子，比如说现在A 主要负责生产巧克力， B则是负责吃巧克力
我们用下面的代码来来表示(生产者消费者模型)

```c
var 巧克力数量;

function A()
{
    while(1){
        GenerateChocolateAndUpdate(); // 生产并且更新巧克力数量
        WiatTime(); //休息
    }
}

function B()
{
    while(1)
    {
       尝试获取巧克力，如果巧克力数量！=0，那么获取到巧克力
       {     
             吃巧克力，巧克力数量-1
       }else{
             获取失败，什么都不做
            }
    }
}
```
![sem.jpg](..%2Fass%2Fday3%2Fsem.jpg)

Code at day3/[semaphore_test.c](semaphore_test.c)

### 2- 互斥锁，互斥集(Mutex)
互斥锁只有两种状态，它可以被用来描述一个资源，可用或者不可用。 如果一个task获取到了这个锁，那么其他的tasks无法再同时获取到这个锁。除非获取到锁的
线程主动释放这个锁。

当一个线程持有互斥量时，其他线程将不能够对它进行开锁或持有它，持有该互斥量的线程也能够再次获得这个锁而不被挂起，如下图时所示
这个特性与一般的二值信号量有很大的不同：在信号量中，因为已经不存在实例，线程递归持有会发生主动挂起（最终形成死锁）。

**演示**
```c
void some_function() {
    
    rt_mutex_take(); // 第一次获取锁


    rt_mutex_take(); // 第二次尝试获取锁，这将导致死锁，因为线程A已经持有这个锁，但由于它再次尝试获取锁而被挂起，无法继续执行

    rt_mutex_release(); // 释放锁

    rt_mutex_release(); // 释放锁
}

```

同样互斥锁，存在两种创建的方法，类似其他的RT-object

1- 静态创建
```c
rt_err_t rt_mutex_init (rt_mutex_t mutex, const char* name, rt_uint8_t flag);
```

2- 动态创建
```c
rt_mutex_t rt_mutex_create (const char* name, rt_uint8_t flag);
```

**删除**
```c
rt_err_t rt_mutex_delete (rt_mutex_t mutex);
```

**脱离**
```c
rt_err_t rt_mutex_detach (rt_mutex_t mutex);
```


**获取互斥量**
```c
rt_err_t rt_mutex_take (rt_mutex_t mutex, rt_int32_t time);  //等待指定时间，若获取不到
rt_err_t rt_mutex_trytake(rt_mutex_t mutex); //不等待
```

**释放互斥量**
```c
rt_err_t rt_mutex_release(rt_mutex_t mutex);
```

可以使用互斥量来实现两个Task对同一块资源的访问，比如说使同一个变量 ++
![mutex.jpg](..%2Fass%2Fday3%2Fmutex.jpg)

Code at day3/[mutex_test.c](mutex_test.c)


### 3- 事件集 (Event group)
事件集主要用于线程间的同步，与信号量不同，它的特点是可以实现一对多，多对多的同步。
即一个线程与多个事件的关系可设置为：其中任意一个事件唤醒线程，或几个事件都到达后才唤醒线程进行后续的处理；
同样，事件也可以是多个线程同步多个事件。这种多个事件的集合可以用一个 32 位无符号整型变量来表示，
变量的每一位代表一个事件，线程通过 “逻辑与” 或“逻辑或”将一个或多个事件关联起来，形成事件组合。
事件的 “逻辑或” 也称为是独立型同步，指的是线程与任何事件之一发生同步；事件 “逻辑与” 也称为是关联型同步，
指的是线程与若干事件都发生同步。

![06event_work.png](..%2Fass%2Fday3%2F06event_work.png)

**创建事件Group**

```c
rt_event_t rt_event_create(const char* name, rt_uint8_t flag);  //动态
rt_err_t rt_event_init(rt_event_t event, const char* name, rt_uint8_t flag); //静态
```

上述需要注意的是，事件集合也是可以FIFO 和基于Priority的调度

**删除和脱离**

```c
rt_err_t rt_event_delete(rt_event_t event); // 删除
rt_err_t rt_event_detach(rt_event_t event); //脱离
```

重要的方法在Send 和 receive事件结合上, 由于事件集合是一个32Bit的数，使用**1 << 位数** 或者使**用其他逻辑运算符**来确定，哪个事件产生了

```c
rt_err_t rt_event_send(rt_event_t event, rt_uint32_t set);
```

**接收事件**


```c
rt_err_t rt_event_recv(rt_event_t event,
                           rt_uint32_t set,
                           rt_uint8_t option,
                           rt_int32_t timeout,
                           rt_uint32_t* recved);
```

这里需要注意的是这几个参数，分别是SET，即32bits中的哪一个bit对应的事件是你感兴趣的，这里也可以使用逻辑与指定多个

**option 可选参数如下**
```c
/* 选择 逻辑与 或 逻辑或 的方式接收事件 */
RT_EVENT_FLAG_OR
RT_EVENT_FLAG_AND

/* 选择清除重置事件标志位 */
RT_EVENT_FLAG_CLEAR
```

![event-group.jpg](..%2Fass%2Fday3%2Fevent-group.jpg)

Code at day3/[event_group.c](event_group.c)


# 线程之间通信
这里我就不详细的写官方的function，其实这些functions RT-Thread官方命名结构， 文档等都维护的非常好。很多方法的名称的命名规则基本上都是一样的。
基本上看明白了一个，就可以看懂其他相关的Function了。我这里将会对线程之间通信的demo 进行展示。 分别使用邮箱和 消息队列。

TODO