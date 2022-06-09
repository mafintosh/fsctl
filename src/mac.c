#include <errno.h>
#include <fcntl.h>
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

  int res = fcntl(fd, F_SETLKW, &data);

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

  int res = fcntl(fd, F_SETLK, &data);

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

  int res = fcntl(fd, F_SETLK, &data);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__punch_hole (uv_os_fd_t fd, uint64_t offset, size_t length) {
  struct fpunchhole data = {
    .fp_flags = 0,
    .reserved = 0,
    .fp_offset = offset,
    .fp_length = length,
  };

  int res = fcntl(fd, F_PUNCHHOLE, &data);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}
