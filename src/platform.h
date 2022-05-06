#ifndef FSCTL_PLATFORM_H
#define FSCTL_PLATFORM_H

#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"

int
fsctl__lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type);

int
fsctl__try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type);

int
fsctl__unlock (uv_os_fd_t fd, uint64_t offset, size_t length);

int
fsctl__punch_hole (uv_os_fd_t fd, uint64_t offset, size_t length);

int
fsctl__set_sparse (uv_os_fd_t fd);

#endif // FSCTL_PLATFORM_H
