import test from 'brittle'
import { open } from 'fs/promises'
import { temporaryFile } from 'tempy'

import { punchHole, sparse } from '../index.js'

test('explicit hole', async (t) => {
  const file = await open(temporaryFile(), 'w+')
  await sparse(file.fd)

  const { blksize } = await file.stat()

  const empty = Buffer.alloc(blksize * 1000)
  await file.write(empty)

  const write = Buffer.from('hello world')
  await file.write(write)

  await testPunchHole(t, file, 0, empty.byteLength)

  const read = Buffer.alloc(write.byteLength)
  await file.read(read, 0, write.byteLength, empty.byteLength)

  t.alike(read, write, 'file is intact')
})

test('implicit hole', async (t) => {
  const file = await open(temporaryFile(), 'w+')
  await sparse(file.fd)

  const { blksize } = await file.stat()

  const empty = blksize * 1000

  const write = Buffer.from('hello world')
  await file.write(write, 0, write.byteLength, empty)

  const { blocks } = await file.stat()

  t.comment(`${blocks} blocks`)
  t.pass()
})

test('unaligned hole', { skip: process.platform === 'darwin' }, async (t) => {
  const file = await open(temporaryFile(), 'w+')
  await sparse(file.fd)

  const { blksize } = await file.stat()

  const empty = Buffer.alloc(blksize * 1000)
  await file.write(empty)

  await testPunchHole(t, file, blksize / 2, empty.byteLength)
})

async function testPunchHole (t, file, offset, length) {
  await testReducesBlocks(t, file, () => punchHole(file.fd, offset, length))
}

async function testReducesBlocks (t, file, fn) {
  const { blocks: before } = await file.stat()

  await fn()

  const { blocks: after } = await file.stat()

  t.comment(`${before} -> ${after} blocks`)
  t.ok(after < before, 'blocks reduced')
}
