import test from 'brittle'
import { fork } from 'child_process'
import { open } from 'fs/promises'
import { temporaryFile } from 'tempy'

test('2 shared + 1 exclusive lock', async (t) => {
  const shared = t.test('grant shared locks')
  shared.plan(2)

  const file = temporaryFile()
  const handle = await open(file, 'w+')

  const p1 = fork('test/fixture/lock.mjs', [file])
  const p2 = fork('test/fixture/lock.mjs', [file])

  p1.on('message', (message) => {
    shared.alike(message, { granted: true }, 'lock granted')
  })

  p2.on('message', (message) => {
    shared.alike(message, { granted: true }, 'lock granted')
  })

  await shared

  const deny = t.test('deny exclusive lock')
  deny.plan(1)

  const p3 = fork('test/fixture/lock.mjs', [file,
    '--mode', 'a+',
    '--exclusive'
  ])

  p3.once('message', (message) => {
    deny.alike(message, { granted: false }, 'lock denied')
  })

  await deny

  const release = t.test('release shared locks')
  release.plan(2)

  p1.on('close', () => release.pass('lock released')).kill()
  p2.on('close', () => release.pass('lock released')).kill()

  await release

  const grant = t.test('grant exclusive lock')
  grant.plan(1)

  p3.on('message', (message) => {
    grant.alike(message, { granted: true }, 'lock granted')
  })

  await grant

  p3.kill()

  await handle.close()
})
