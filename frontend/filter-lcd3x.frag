precision mediump float;

const float brighten_scanlines = 16.0;
const float brighten_lcd = 4.0;

uniform sampler2D tex0;
uniform vec2 tex0Size;
uniform vec2 outSize;

varying vec2 uv;
varying vec2 omega;

const vec3 offsets = 3.141592654 * vec3(1.0/2.0, 1.0/2.0 - 2.0/3.0, 1.0/2.0-4.0/3.0);

void main()
{
  vec3 res = texture2D(tex0, uv).xyz;
  vec2 angle = uv * omega;

  float yfactor = (brighten_scanlines + sin(angle.y)) / (brighten_scanlines + 1.0);
  vec3 xfactors = (brighten_lcd + sin(angle.x + offsets)) / (brighten_lcd + 1.0);

  vec3 color = yfactor * xfactors * res;

  gl_FragColor = vec4(color.x, color.y, color.z, 1.0);
}
