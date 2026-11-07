#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h" 
#include "memlayout.h"
#include "stdio.h"


int
mutexalloc(struct file **pf)
{
    struct file *f;
    struct sleeplock *m;
    f = filealloc();
    if (f == 0) {
        return -1;
    }
    m = (struct sleeplock *)kalloc();
    if (m == 0) {
        fileclose(f);
        return -1;
    }
    initsleeplock(m, "mutex");
    f->type = FD_MUTEX;
    f->readable = 0;
    f->writable = 0;
    f->mutex = m;
    *pf = f;
    printk("[mutexalloc] pid=%d f=%p m=%p\n", myproc()->pid, f, m);
    return 0;
}

void
mutex_close(struct sleeplock *m) {
    if (m == 0) {
        return;
    }
    printk("[mutexclose] m=%p\n", m);
    kfree(m);
}

int
mutex_lock(struct sleeplock *m) {
    struct proc *p = myproc();
    acquire(&m->lk);
    if (m->locked && m->pid == p->pid) {
        release(&m->lk);
        return -1;
    }
    release(&m->lk);

    acquiresleep(m);
    printk("[mutex_lock] pid=%d m=%p\n", p->pid, m);
    return 0;
}

int mutex_unlock(struct sleeplock *m) {
    struct proc *p = myproc();
    acquire(&m->lk);
    if (!(m->locked && m->pid == p->pid)) {
        release(&m->lk);
        return -1;
    }

    release(&m->lk);
    releasesleep(m);
    printk("[mutex_unlock] pid=%d m=%p\n", p->pid, m);
    return 0;
}
