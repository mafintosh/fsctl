#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"
#include "platform.h"

int
fsctl__punch_hole (uv_os_fd_t fd, uint64_t offset, size_t length) {
  int res = fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, offset, length);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}

int
fsctl__swap_at (uv_os_fd_t from_fd, const char *from_path, uv_os_fd_t to_fd, const char *to_path) {
  int res = renameat2(from_fd, from_path, to_fd, to_path, RENAME_EXCHANGE);

  return res == -1 ? uv_translate_sys_error(errno) : res;
}
