#include <node_api.h>
#include <napi-macros.h>
#include <uv.h>

#ifndef WIN32
#include <sys/file.h>
#endif

static napi_value punchhole_name;

typedef struct fsctl_native_punchhole {
  napi_async_work worker;
  napi_ref ctx;
  napi_ref callback;
  uv_os_fd_t fd;
  int result;
  off_t start;
  off_t length;
} fsctl_native_punchhole_t;

NAPI_METHOD(fsctl_native_sparse) {
  NAPI_ARGV(1)
  NAPI_ARGV_UINT32(fd, 0)

  #ifdef WIN32
  uv_os_fd_t h = uv_get_osfhandle(fd);
  DWORD d;
  int result = DeviceIoControl(h, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &d, NULL) ? 1 : 0;
  #else
  int result = 1;
  #endif

  NAPI_RETURN_UINT32(result)
}

NAPI_METHOD(fsctl_native_lock) {
  NAPI_ARGV(1)
  NAPI_ARGV_UINT32(fd, 0)

  uv_os_fd_t h = uv_get_osfhandle(fd);

  #ifdef WIN32
  int locked = LockFile(h, 0, 0, 1, 0) ? 1 : 0;
  #else
  int locked = flock(h, LOCK_EX | LOCK_NB) == 0 ? 1 : 0;
  #endif

  NAPI_RETURN_UINT32(locked)
}

NAPI_METHOD(fsctl_native_unlock) {
  NAPI_ARGV(1)
  NAPI_ARGV_UINT32(fd, 0)

  uv_os_fd_t h = uv_get_osfhandle(fd);

  #ifdef WIN32
  int unlocked = UnlockFile(h, 0, 0, 1, 0) ? 1 : 0;
  #else
  int unlocked = flock(h, LOCK_UN | LOCK_NB) == 0 ? 1 : 0;
  #endif

  NAPI_RETURN_UINT32(unlocked)
}

static void on_punchhole (napi_env env, void* data) {
  fsctl_native_punchhole_t *f = (fsctl_native_punchhole_t *) data;

  #ifdef __linux__
  f->result = fallocate(f->fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, f->start, f->length);
  #else
  fpunchhole_t s = {
    .fp_offset = f->start,
    .fp_length = f->length
  };

  f->result = fcntl(f->fd, F_PUNCHHOLE, &s);
  #endif
}

static void on_punchhole_complete (napi_env env, napi_status status, void* data) {
  fsctl_native_punchhole_t *f = (fsctl_native_punchhole_t *) data;

  napi_value argv[1];

  napi_value ctx;
  napi_get_reference_value(env, f->ctx, &ctx);

  napi_value callback;
  napi_get_reference_value(env, f->callback, &callback);

  napi_create_int32(env, f->result, &argv[0]);
  NAPI_MAKE_CALLBACK(env, NULL, ctx, callback, 1, argv, NULL)

  napi_delete_async_work(env, f->worker);
  napi_delete_reference(env, f->callback);
  napi_delete_reference(env, f->ctx);
}

NAPI_METHOD(fsctl_native_punchhole) {
  NAPI_ARGV(6)

  NAPI_ARGV_BUFFER_CAST(fsctl_native_punchhole_t *, data, 0)

  napi_async_work worker = data->worker;
  napi_create_async_work(env, NULL, punchhole_name, on_punchhole, on_punchhole_complete, data, &worker);

  napi_value ctx = argv[1];
  napi_create_reference(env, ctx, 1, &(data->ctx));

  napi_value callback = argv[2];
  napi_create_reference(env, callback, 1, &(data->callback));

  NAPI_ARGV_UINT32(fd, 3)
  NAPI_ARGV_UINT32(start, 4)
  NAPI_ARGV_UINT32(len, 5)

  data->fd = uv_get_osfhandle(fd);
  data->start = start;
  data->length = len;

  napi_queue_async_work(env, worker);

  return NULL;
}

NAPI_INIT () {
  napi_create_string_utf8(env, "fsctl: punchhole", NAPI_AUTO_LENGTH, &punchhole_name);

  NAPI_EXPORT_SIZEOF(fsctl_native_punchhole_t)
  NAPI_EXPORT_FUNCTION(fsctl_native_sparse)
  NAPI_EXPORT_FUNCTION(fsctl_native_lock)
  NAPI_EXPORT_FUNCTION(fsctl_native_unlock)
  NAPI_EXPORT_FUNCTION(fsctl_native_punchhole)
}
