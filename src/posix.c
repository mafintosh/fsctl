#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"
#include "platform.h"

int
fsctl__lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  struct flock flock = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = type == FSCTL_WRLOCK ? F_WRLCK : F_RDLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(fd, F_SETLKW, &flock);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  struct flock flock = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = type == FSCTL_WRLOCK ? F_WRLCK : F_RDLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(fd, F_SETLK, &flock);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__unlock (uv_os_fd_t fd, uint64_t offset, size_t length) {
  struct flock flock = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = F_UNLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(fd, F_SETLK, &flock);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__set_sparse (uv_os_fd_t fd) {
  return 0;
}
