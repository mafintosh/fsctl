#ifndef FSCTL_SHARED_H
#define FSCTL_SHARED_H

#include <stdbool.h>
#include <stdint.h>
#include <uv.h>

int
fsctl__lock (uv_os_fd_t fd, uint64_t offset, size_t length, fsctl_lock_type_t type, bool block);

int
fsctl__unlock (uv_os_fd_t fd, uint64_t offset, size_t length);

int
fsctl__punch_hole (uv_os_fd_t fd, uint64_t offset, size_t length);

int
fsctl__set_sparse (uv_os_fd_t fd);

#endif // FSCTL_SHARED_H
