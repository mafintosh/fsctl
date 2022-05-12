import { open } from 'fs/promises'
import minimist from 'minimist'
import { lock, tryLock } from '../../index.js'

const argv = minimist(process.argv.slice(2), {
  boolean: ['exclusive'],
  string: ['mode', 'offset', 'length'],
  default: {
    mode: 'r',
    exclusive: false,
    offset: 0,
    length: 0
  }
})

const offset = parseInt(argv.offset, 10)

const length = parseInt(argv.length, 10)

const options = {
  exclusive: argv.exclusive
}

const file = await open(argv._[0], argv.mode)

if (!tryLock(file.fd, offset, length, options)) {
  process.send({ granted: false })
}

await lock(file.fd, offset, length, options)

process.send({ granted: true })

while (true) {
  //
}
