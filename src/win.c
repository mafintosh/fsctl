#include <io.h>
#include <uv.h>

#include "../include/fsctl.h"

int
fsctl__set_sparse (uv_os_fd_t fd) {
  BOOL res = DeviceIoControl(fd, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, NULL, NULL);
  return res ? 0 : uv_translate_sys_error(GetLastError());
}
