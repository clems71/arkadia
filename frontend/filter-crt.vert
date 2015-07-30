precision mediump float;

attribute vec2 position;

varying vec2 uv;

void main() {
  uv = (position.xy * vec2(0.5, -0.5) + vec2(0.5));
  gl_Position = vec4(position.xy, 0.0, 1.0);
}
