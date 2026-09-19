#include "types.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

uint64
sys_sum(void)
{
  int a, b;

  argint(0, &a);
  argint(1, &b);

  printk("debug: sys_sum was called with arguments %d and %d\n", a, b);

  return a + b;
}