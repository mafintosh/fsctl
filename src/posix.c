#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"
#include "platform.h"

int
fsctl__lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  if (offset != 0 || length != 0) return UV_EINVAL;

  int res = flock(fd, type == FSCTL_WRLOCK ? LOCK_EX : LOCK_SH);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  if (offset != 0 || length != 0) return UV_EINVAL;

  int res = flock(fd, (type == FSCTL_WRLOCK ? LOCK_EX : LOCK_SH) | LOCK_NB);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__unlock (uv_os_fd_t fd, uint64_t offset, size_t length) {
  if (offset != 0 || length != 0) return UV_EINVAL;

  int res = flock(fd, LOCK_UN);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__sparse (uv_os_fd_t fd) {
  return 0;
}
