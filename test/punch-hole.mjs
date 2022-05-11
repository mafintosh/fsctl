import test from 'brittle'
import { open } from 'fs/promises'

import { punchHole, setSparse } from '../index.js'

test('punch hole', async (t) => {
  const file = await open('test/fixture/sparse.txt', 'w+')

  const empty = Buffer.alloc(1024 * 1024 * 10) // 10 MB
  await file.write(empty)

  const write = Buffer.from('hello world')
  await file.write(write)

  const { blocks: before } = await file.stat()

  setSparse(file.fd)
  await punchHole(file.fd, 0, empty.byteLength)

  const { blocks: after } = await file.stat()

  t.comment(`${before} -> ${after} blocks`)
  t.ok(after < before, 'blocks reduced')

  const read = Buffer.alloc(11)
  await file.read(read, 0, 11, empty.byteLength)

  t.alike(read, write, 'file is intact')
})
