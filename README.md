# thread_pool
一个简单的Linux环境下C语言的线程池的实现，可用来学习基本线程。<br>
The simple implement of thread poll.<br> 

## 1. 基本原理
在传统服务器结构中, 常是有一个总的监听线程监听有没有新的用户连接服务器, 每当有一个新的用户进入,<br>
服务器就开启一个新的线程用户处理这个用户的数据包。这个线程只服务于这个用户,当用户与服务器端关闭<br>
连接以后, 服务器端销毁这个线程。然而频繁地开辟与销毁线程极大地占用了系统的资源。而且在大量用户的<br>
情况下, 系统为了开辟和销毁线程将浪费大量的时间和资源。线程池提供了一个解决外部大量用户与服务器有<br>
限资源的矛盾, 线程池和传统的一个用户对应一个线程的处理方法不同, 它的基本思想就是在程序开始时就在<br>
内存中开辟一些线程,线程的数目是固定的,他们独自形成一个类, 屏蔽了对外的操作, 而服务器只需要将数<br>
据包交给线程池就可以了。当有新的客户请求到达时 ,不是新创建一个线程为其服务 , 而是从“池子”中选择<br>
一个空闲的线程为新的客户请求服务 ,服务完毕后 , 线程进入空闲线程池中。如果没有线程空闲 的 话, 就<br>
将数据包暂时积累,等待线程池内有线程空闲以后再进行处理。通过对多个任务重用已经存在的线程对象,降低<br>
了对线程对象创建和销毁的开销。当客户请求时 ,线程对象已经存在,可以提高请求的响应时间,从而整体地提<br>
高了系统服务的表现。<br>
## 2. 线程池的特点： <br>
* 线程池拥有若干个线程；<br>
    a. 线程数量具有限制，通常小于并发的任务量；<br>
* 它主要用于执行大量的相对短暂的任务；<br>
    a. 计算密集型任务： 线程个数=CPU个数<br>
    b. I/O密集型任务： 线程个数>CPU个数<br>
* 如果任务执行时间很长，那么就不适合放到线程中进行处理；<br>
## 3. 该线程池的实现特点：<br>
* 用于执行大量相对短暂的任务；<br>
* 当任务增加的时候能够动态的增加线程池中线程的数量知道达到一个阈值；<br>
* 当任务执行完毕时， 能够动态销毁线程池中的线程。<br>

`Note`: 本质是生产者-消费者模型<br>
生产者线程向任务队列中添加任务，一旦队列有任务到来，如果有等待线程就唤<br>
醒来执行任务，如果没有等待线程并且线程数没有达到阈值，就会创建新的线程来执行任务。<br>

## 4.线程池框架
一般来说实现一个线程池主要包括以下几个部分组成：<br>
