import test from 'brittle'
import { join } from 'path'
import { writeFile, readFile } from 'fs/promises'
import { temporaryFile, temporaryDirectory } from 'tempy'

import { swap } from '../index.js'

const isWindows = process.platform === 'win32'

test('swap files', { skip: isWindows }, async (t) => {
  const a = temporaryFile()
  const b = temporaryFile()

  await writeFile(a, 'a')
  await writeFile(b, 'b')

  await swap(a, b)

  t.is(await readFile(a, 'utf8'), 'b')
  t.is(await readFile(b, 'utf8'), 'a')
})

test('swap directories', { skip: isWindows }, async (t) => {
  const a = temporaryDirectory()
  const b = temporaryDirectory()

  await writeFile(join(a, 'a'), 'a')
  await writeFile(join(b, 'b'), 'b')

  await swap(a, b)

  t.is(await readFile(join(a, 'b'), 'utf8'), 'b')
  t.is(await readFile(join(b, 'a'), 'utf8'), 'a')
})

test('swap file and directory', { skip: isWindows }, async (t) => {
  const a = temporaryDirectory()
  const b = temporaryFile()

  await writeFile(join(a, 'a'), 'a')
  await writeFile(b, 'b')

  await swap(a, b)

  t.is(await readFile(a, 'utf8'), 'b')
  t.is(await readFile(join(b, 'a'), 'utf8'), 'a')
})
