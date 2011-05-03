const char *row5f = STRINGIFY(
uniform sampler2D source;
uniform float coefficients[5];
uniform vec2 offsets[5];

void main(void)
{
    float d = 0.1;
    vec4 c = vec4(0, 0, 0, 0);
    vec2 tc = gl_TexCoord[0].st;

    c += coefficients[0] * texture2D(source, tc + offsets[0]);
    c += coefficients[1] * texture2D(source, tc + offsets[1]);
    c += coefficients[2] * texture2D(source, tc + offsets[2]);
    c += coefficients[3] * texture2D(source, tc + offsets[3]);
    c += coefficients[4] * texture2D(source, tc + offsets[4]);

    gl_FragColor = c;
}
);
