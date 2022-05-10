const util = require('util')
const binding = require('node-gyp-build')(__dirname)

function onwork (errno) {
  if (errno < 0) this.reject(toError(errno))
  else this.resolve()
}

exports.lock = function lock (fd, offset = 0, length = 0, opts = {}) {
  if (typeof offset === 'object') {
    opts = offset
    offset = 0
  }

  if (typeof length === 'object') {
    opts = length
    length = 0
  }

  if (typeof opts !== 'object') {
    opts = {}
  }

  const req = Buffer.alloc(binding.sizeof_fsctl_napi_lock_t)
  const ctx = {
    req,
    resolve: null,
    reject: null
  }

  const promise = new Promise((resolve, reject) => {
    ctx.resolve = resolve
    ctx.reject = reject
  })

  const errno = binding.fsctl_napi_lock(req, fd, offset, length, opts.exclusive ? 1 : 0, ctx, onwork)

  if (errno < 0) throw toError(errno)

  return promise
}

exports.tryLock = function tryLock (fd, offset = 0, length = 0, opts = {}) {
  if (typeof offset === 'object') {
    opts = offset
    offset = 0
  }

  if (typeof length === 'object') {
    opts = length
    length = 0
  }

  if (typeof opts !== 'object') {
    opts = {}
  }

  const errno = binding.fsctl_napi_try_lock(fd, offset, length, opts.exclusive ? 1 : 0)

  if (errno < 0) {
    const err = toError(errno)
    if (err.code === 'EAGAIN') return false
    throw err
  }

  return true
}

exports.unlock = function unlock (fd, offset = 0, length = 0) {
  const errno = binding.fsctl_napi_unlock(fd, offset, length)

  if (errno < 0) throw toError(errno)
}

exports.punchHole = function punchHole (fd, offset, length) {
  const req = Buffer.alloc(binding.sizeof_fsctl_napi_punch_hole_t)
  const ctx = {
    req,
    resolve: null,
    reject: null
  }

  const promise = new Promise((resolve, reject) => {
    ctx.resolve = resolve
    ctx.reject = reject
  })

  const errno = binding.fsctl_napi_punch_hole(req, fd, offset, length, ctx, onwork)

  if (errno < 0) throw toError(errno)

  return promise
}

exports.setSparse = function setSparse (fd) {
  const errno = binding.fsctl_napi_set_sparse(fd)

  if (errno < 0) throw toError(errno)
}

function toError (errno) {
  const [code, msg] = util.getSystemErrorMap().get(errno)

  const err = new Error(`${code}: ${msg}`)
  err.errno = errno
  err.code = code

  Error.captureStackTrace(err, toError)

  return err
}
