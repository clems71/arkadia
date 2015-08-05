'use strict'

let events = require('events')

class InputSystem extends events.EventEmitter {

  constructor() {
    super()
    this._handlers = new Map()
    this._nameMapping = new Map()
  }

  register(name, handler) {
    this._handlers.set(name, handler)
    let self = this
    handler.on('pressed', k => self.emit('pressed', self._nameMapping.get(name+k)))
    handler.on('released', k => self.emit('released', self._nameMapping.get(name+k)))
  }

  bind(name, inputs) {
    inputs = [].concat(inputs) // Normalize as array, even if scalar

    for (let k of inputs) {
      this._nameMapping.set(k, name)
      for (let handlerElmt of this._handlers) {

        let handlerName = handlerElmt[0]
        let handler = handlerElmt[1]

        if (!k.startsWith(handlerName)) continue

        // ex: kb:Z -> Z
        k = k.substring(handlerName.length)
        handler.listen(k)
        break

      } // end for all handlers
    } // end for all inputs

  } // end function bind

}

module.exports = InputSystem
