const char *blitf = STRINGIFY(
uniform sampler2D source;
uniform vec4 bkgd;

void main(void)
{
    vec4 t = texture2D(source, gl_TexCoord[0].st);
    gl_FragColor = t + bkgd;
}
);
