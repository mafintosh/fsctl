# fsctl

Native utilities for file manipulation, including locking and hole punching.

```
npm install fsctl
```

## Usage

Write to a file using an exclusive lock:

``` js
const { open } = require('fs/promises')
const { lock, unlock } = require('fsctl')

const file = await open('file.txt', 'a+')

await lock(file.fd, { exclusive: true })

try {
  await file.write('hello world')
} finally {
  unlock(file.fd)
}
```

## API

#### `await fsctl.lock(fd[, offset[, length]][, options])`

Request a process level lock on a file, resolving when the lock is granted. If another process holds the lock, the lock will not be granted until the other process either exits or releases the lock.

To lock only a portion of the file, `offset` and `length` may be passed. A `length` of `0` will request a lock from `offset` to the end of the file.

Note that the lock is only advisory and there is nothing stopping another process from accessing the file by simply ignoring the lock.

Options include:

```js
{
  // If `true`, request an exclusive lock, i.e. a write lock, on the file. By
  // default, a shared lock, i.e. a read lock, is requested.
  // Be aware that an exclusive lock can only be granted to files that are
  // writable! A request for an exclusive lock on a read-only file is ignored 
  // and treated as a request for a shared lock.
  exclusive: false
}
```

#### `const granted = fsctl.tryLock(fd[, offset[, length]][, options])`

Request a process level lock on a file, returning `true` if the lock was granted or `false` if another process currently holds the lock.

Options are the same as `fsctl.lock()`.

#### `fsctl.unlock(fd[, offset[, length]])`

Release a process level lock on a file.

#### `await fsctl.punchHole(fd, offset, length)`

Punch a hole in a file at `offset` for `length` bytes. On file systems that support sparse files, holes will take up no physical space.

On Windows, the file must first be marked sparse using `fsctl.setSparse(fd)`. Otherwise, zeros will be explicitly written to the hole.

On macOS, the hole must be aligned to block boundaries as the call will otherwise fail.

#### `fsctl.setSparse(fd)`

Mark a file as sparse. On Windows, this operation is required before holes can be punched in the file. On other systems, this operation has no effect.

## License

MIT
