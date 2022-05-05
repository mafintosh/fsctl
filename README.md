# fsctl

Native utilities for file manipulation, including locking and hole punching.

```
npm install fsctl
```

## Usage

``` js
// TODO
```

## API

#### `await fsctl.lock(fd[, offset[, length]][, options])`

Request a process level lock on a file, resolving when the lock is granted. If another process holds the lock, the lock will not be granted until the other process either exits or releases the lock.

To lock only a portion of the file, `offset` and `length` may be passed. A `length` of `0` will request a lock from `offset` to the end of the file.

Options include:

```js
{
  // If `true`, request an exclusive lock, i.e. a write lock, on the file. By
  // default, a shared lock, i.e. a read lock, is requested.
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

## License

MIT
