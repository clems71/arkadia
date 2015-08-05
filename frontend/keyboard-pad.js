'use strict'

let events = require('events')

class KeyboardPad extends events.EventEmitter {

  constructor (shell) {
    super()
    this._shell = shell
    this._keys = []
    this._states = new Map()
    var self = this
    this._shell.on('tick', x => self._tick() )
  }

  _tick() {
    for (let k of this._keys) {
      let state = this._shell.wasDown(k)
      if (state && !this._states.get(k)) {
        this.emit('pressed', k)
      } else if (!state && this._states.get(k)) {
        this.emit('released', k)
      }
      this._states.set(k, state)
    }
  }

  listen(name) {
    this._keys.push(name)
    this._shell.bind(name, name)
  }
}

module.exports = KeyboardPad
