const char *hipassf = STRINGIFY(
uniform sampler2D source;

void main(void)
{
	vec4 color = texture2D(source, gl_TexCoord[0].st);

	color.x = color.x > 1.0 ? 1.0 : 0.0;
	color.y = color.y > 1.0 ? 1.0 : 0.0;
	color.z = color.z > 1.0 ? 1.0 : 0.0;
	
	gl_FragColor = color;
}
);
