#define _GNU_SOURCE

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __APPLE__
int setresgid(gid_t rgid, gid_t egid, gid_t sgid);
int setresuid(uid_t ruid, uid_t euid, uid_t suid);
#endif

int main(int argc, char **argv) {
  gid_t egid;
  uid_t euid;
  char *args[] = {"/bin/sh", NULL};

  (void)argc;
  if (atoi(argv[1]) == 423) {
    egid = getegid();
    euid = geteuid();
    setresgid(egid, egid, egid);
    setresuid(euid, euid, euid);
    execv("/bin/sh", args);
  } else
    write(1, "No !\n", 5);
  return (0);
}
