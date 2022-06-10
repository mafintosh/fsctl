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
