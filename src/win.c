#include <io.h>
#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"
#include "platform.h"

int
fsctl__lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  if (length == 0) length = SIZE_MAX;

  DWORD flags = 0;

  if (type == FSCTL_WRLOCK) flags |= LOCKFILE_EXCLUSIVE_LOCK;

  OVERLAPPED data = {
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
    &data
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fsctl__try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type) {
  if (length == 0) length = SIZE_MAX;

  DWORD flags = LOCKFILE_FAIL_IMMEDIATELY;

  if (type == FSCTL_WRLOCK) flags |= LOCKFILE_EXCLUSIVE_LOCK;

  OVERLAPPED data = {
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
    &data
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fsctl__unlock (uv_os_fd_t fd, uint64_t offset, size_t length) {
  if (length == 0) length = SIZE_MAX;

  OVERLAPPED data = {
    .hEvent = 0,
    .Offset = offset,
    .OffsetHigh = offset >> 32,
  };

  BOOL res = UnlockFileEx(
    fd,
    0,
    length,
    length >> 32,
    &data
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fsctl__punch_hole (uv_os_fd_t fd, uint64_t offset, size_t length) {
  FILE_ZERO_DATA_INFORMATION data = {
    .FileOffset = {
      .QuadPart = offset,
    },
    .BeyondFinalZero = {
      .QuadPart = offset + length,
    },
  };

  DWORD bytes;

  BOOL res = DeviceIoControl(
    fd,
    FSCTL_SET_ZERO_DATA,
    &data,
    sizeof(data),
    NULL,
    0,
    &bytes, // Must be passed when lpOverlapped is NULL
    NULL
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}

int
fsctl__set_sparse (uv_os_fd_t fd) {
  DWORD bytes;

  BOOL res = DeviceIoControl(
    fd,
    FSCTL_SET_SPARSE,
    NULL,
    0,
    NULL,
    0,
    &bytes, // Must be passed when lpOverlapped is NULL
    NULL
  );

  return res ? 0 : uv_translate_sys_error(GetLastError());
}
