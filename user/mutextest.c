#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// без синхронизации
static void
print_args_nosync(int argc, char **argv)
{
  int i, j;
  for (i = 1; i < argc; i++) {
    for (j = 0; argv[i][j] != 0; j++) {
      printf("%d: arg %d, char '%c'\n", getpid(), i, argv[i][j]);
    }
  }
}

// строка целиком под мьютексом
static void
print_args_sync(int argc, char **argv, int fd)
{
  int i, j;
  for (i = 1; i < argc; i++) {
    for (j = 0; argv[i][j] != 0; j++) {
      if (mutex_lock(fd) < 0) {
        printf("mutex_lock failed\n");
        exit(1);
      }
      printf("%d: arg %d, char '%c'\n", getpid(), i, argv[i][j]);
      if (mutex_unlock(fd) < 0) {
        printf("mutex_unlock failed\n");
        exit(1);
      }
    }
  }
}

// Проверка, что read/write/fstat на fd мьютекса дают -1
static void
check_not_io(int fd)
{
  char buf[4] = "x";
  struct stat st;

  if (read(fd, buf, 1) != -1)
    printf("FAIL: read on mutex returned != -1\n");
  if (write(fd, buf, 1) != -1)
    printf("FAIL: write on mutex returned != -1\n");
  if (fstat(fd, &st) != -1)
    printf("FAIL: fstat on mutex returned != -1\n");
  printf("check_not_io: OK\n");
}

int
main(int argc, char *argv[])
{
  int fd, fd2, pid;

  if (argc < 2) {
    printf("usage: mutextest arg1 [arg2 ...]\n");
    exit(1);
  }

  // без синхронизации
  printf("\nNO SYNC\n");
  pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }
  if (pid == 0) {
    print_args_nosync(argc, argv);
    exit(0);
  }
  print_args_nosync(argc, argv);
  wait(0);

  // с мьютексом
  printf("\nWITH MUTEX\n");
  if (mutex(&fd) < 0) {
    printf("mutex failed\n");
    exit(1);
  }

  pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    close(fd);
    exit(1);
  }
  if (pid == 0) {
    print_args_sync(argc, argv, fd);
    close(fd);
    exit(0);
  }
  print_args_sync(argc, argv, fd);
  wait(0);
  close(fd);

  // Проверка read/write/fstat
  printf("\nVerify read/write/fstat\n");
  if (mutex(&fd2) < 0) {
    printf("mutex failed\n");
    exit(1);
  }
  check_not_io(fd2);
  close(fd2);

  exit(0);
}