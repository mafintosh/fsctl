#include <io.h>
#include <uv.h>

#include "../include/fsctl.h"
#include "platform.h"

int
fsctl__lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  if (length == 0) length = SIZE_MAX;

  DWORD flags = 0;

  if (type == FSCTL_WRLOCK) flags |= LOCKFILE_EXCLUSIVE_LOCK;

  OVERLAPPED overlapped = {
    .hEvent = 0,
    .Offset = offset,
    .OffsetHigh = offset >> 32,
  };

  BOOL res = LockFileEx(
    fd,
    flags,
    0,
    length,
    length >> 32,
    &overlapped,
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fsctl__try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  if (length == 0) length = SIZE_MAX;

  DWORD flags = LOCKFILE_FAIL_IMMEDIATELY;

  if (type == FSCTL_WRLOCK) flags |= LOCKFILE_EXCLUSIVE_LOCK;

  OVERLAPPED overlapped = {
    .hEvent = 0,
    .Offset = offset,
    .OffsetHigh = offset >> 32,
  };

  BOOL res = LockFileEx(
    fd,
    flags,
    0,
    length,
    length >> 32,
    &overlapped,
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fsctl__unlock (uv_os_fd_t fd, uint64_t offset, size_t length) {
  if (length == 0) length = SIZE_MAX;

  OVERLAPPED overlapped = {
    .hEvent = 0,
    .Offset = offset,
    .OffsetHigh = offset >> 32,
  };

  BOOL res = UnlockFileEx(
    fd,
    flags,
    0,
    length,
    length >> 32,
    &overlapped,
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fsctl__set_sparse (uv_os_fd_t fd) {
  BOOL res = DeviceIoControl(fd, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, NULL, NULL);

  return res ? 0 : uv_translate_sys_error(GetLastError());
}
