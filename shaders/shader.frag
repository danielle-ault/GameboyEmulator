# version 330 core

in vec3 Color;
in vec2 TexCoord;

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
	//FragColor = vec4(1.0, 0.0, 0.5, 1.0);
	//FragColor = vec4(Color, 1.0);
	FragColor = texture(Texture, TexCoord);
}