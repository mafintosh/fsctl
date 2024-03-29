#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"
#include "platform.h"

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
