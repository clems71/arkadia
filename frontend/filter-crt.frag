precision mediump float;

uniform sampler2D tex0;
uniform vec2 tex0Size;
uniform vec2 outSize;

varying vec2 uv;

vec4 TEX2D(vec2 c) {
  vec2 c2 = abs(c-vec2(0.5));
  if (c2.x > 0.5 || c2.y > 0.5) return vec4(0.0);
  return texture2D(tex0, c);
}

// #define TEX2D(c) if (c - vec2(0.5)) texture2D(tex0,(c))
#define PI 3.141592653589
#define phase 0.0
#define gamma 2.7
#define distortion 0.2

vec2 barrelDistortion(vec2 coord) {
  vec2 cc = coord - vec2(0.5);
  float dist = dot(cc, cc);
  return coord + (cc * (dist + distortion * dist * dist) * distortion);
}

void main()
{
  vec2 xy = barrelDistortion(uv);
  vec2 one = 1.0/tex0Size;
  vec2 uv_ratio = fract(xy*tex0Size);
  xy.x = floor(xy.x/one.x)*one.x;

  vec4 col, col2;

  vec4 coeffs = vec4(1.0 + uv_ratio.x, uv_ratio.x, 1.0 - uv_ratio.x, 2.0 - uv_ratio.x);
  coeffs = mix((sin(PI * coeffs) * sin(PI * coeffs / 2.0)) / (coeffs * coeffs), vec4(1.0), vec4(lessThan(abs(coeffs), vec4(0.01))));
  coeffs = coeffs / (coeffs.x+coeffs.y+coeffs.z+coeffs.w);

  col  = clamp(coeffs.x * TEX2D(xy + vec2(-one.x,0.0)) + coeffs.y * TEX2D(xy) + coeffs.z * TEX2D(xy + vec2(one.x, 0.0)) + coeffs.w * TEX2D(xy + vec2(2.0 * one.x, 0.0)),0.0,1.0);
  col2 = clamp(coeffs.x * TEX2D(xy + vec2(-one.x,one.y)) + coeffs.y * TEX2D(xy + vec2(0.0, one.y)) + coeffs.z * TEX2D(xy + one) + coeffs.w * TEX2D(xy + vec2(2.0 * one.x, one.y)),0.0,1.0);
  col = pow(col, vec4(gamma));
  col2 = pow(col2, vec4(gamma));

  vec4 wid = 2.0 + 2.0 * pow(col, vec4(4.0));
  vec4 weights = vec4(uv_ratio.y/0.3);
  weights = 0.51*exp(-pow(weights*sqrt(2.0/wid),wid))/0.3/(0.6+0.2*wid);
  wid = 2.0 + 2.0 * pow(col2,vec4(4.0));
  vec4 weights2 = vec4((1.0-uv_ratio.y)/0.3);
  weights2 = 0.51*exp(-pow(weights2*sqrt(2.0/wid),wid))/0.3/(0.6+0.2*wid);

  vec4 mcol = vec4(1.0);
  if ( mod(uv.x*outSize.x,2.0) < 1.0)
  mcol.g = 0.7;
  else
  mcol.rb = vec2(0.7);

  gl_FragColor = pow(mcol*(col * weights + col2 * weights2), vec4(1.0/2.2));
}
