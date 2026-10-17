#include "types.h"
#include "riscv.h"
#include "defs.h"c
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "file.h"
#include "memlayout.h"


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
    *pf = f;
    return 0;
}

void
mutex_close(struct sleeplock *m) {
    if (m == 0) {
        return;
    }
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
    return 0;
}

int mutex_unlock(struct sleeplock *m) {
    struct proc *p = myproc();
    acquire(&m->lk);
    if (m->locked && m->pid == p->pid) {
        release(&m->lk);
        return -1;
    }

    release(&m->lk);
    releasesleep(m);
    return 0;
}
