#include <errno.h>
#include <fcntl.h>
#include <linux/falloc.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"
#include "platform.h"

int
fsctl__lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  struct flock data = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = type == FSCTL_WRLOCK ? F_WRLCK : F_RDLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(fd, F_OFD_SETLKW, &data);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  struct flock data = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = type == FSCTL_WRLOCK ? F_WRLCK : F_RDLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(fd, F_OFD_SETLK, &data);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__unlock (uv_os_fd_t fd, uint64_t offset, size_t length) {
  struct flock data = {
    .l_start = offset,
    .l_len = length,
    .l_pid = 0,
    .l_type = F_UNLCK,
    .l_whence = SEEK_SET,
  };

  int res = fcntl(fd, F_OFD_SETLK, &data);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__punch_hole (uv_os_fd_t fd, uint64_t offset, size_t length) {
  int res = fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, offset, length);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}
