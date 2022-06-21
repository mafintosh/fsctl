#ifndef FSCTL_PLATFORM_H
#define FSCTL_PLATFORM_H

#include <stdint.h>
#include <uv.h>

#include "../include/fsctl.h"

int
fsctl__try_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type);

int
fsctl__wait_for_lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type);

int
fsctl__unlock (uv_os_fd_t fd, uint64_t offset, size_t length);

int
fsctl__punch_hole (uv_os_fd_t fd, uint64_t offset, size_t length);

int
fsctl__sparse (uv_os_fd_t fd);

int
fsctl__swap (const char *from_path, const char *to_path);

int
fsctl__swap_at (uv_os_fd_t from_fd, const char *from_path, uv_os_fd_t to_fd, const char *to_path);

#endif // FSCTL_PLATFORM_H
