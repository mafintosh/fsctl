import test from 'brittle'
import { open } from 'fs/promises'

import { punchHole, setSparse } from '../index.js'

test('punch hole', async (t) => {
  const file = await open('test/fixture/sparse.txt', 'w+')

  const { blksize } = await file.stat()

  const empty = Buffer.alloc(blksize * 1000)
  await file.write(empty)

  const write = Buffer.from('hello world')
  await file.write(write)

  await testPunchHole(t, file, 0, empty.byteLength)

  const read = Buffer.alloc(11)
  await file.read(read, 0, 11, empty.byteLength)

  t.alike(read, write, 'file is intact')
})

test('unaligned hole', { skip: process.platform === 'darwin' }, async (t) => {
  const file = await open('test/fixture/sparse.txt', 'w+')

  const { blksize } = await file.stat()

  const empty = Buffer.alloc(blksize * 1000)
  await file.write(empty)

  await testPunchHole(t, file, blksize / 2, empty.byteLength)
})

async function testPunchHole (t, file, offset, length) {
  const { blocks: before } = await file.stat()

  setSparse(file.fd)
  await punchHole(file.fd, offset, length)

  const { blocks: after } = await file.stat()

  t.comment(`${before} -> ${after} blocks`)
  t.ok(after < before, 'blocks reduced')
}
