#version 330

// input from rasterizer
in vec2 var_TexCoord;

// input from application
uniform sampler2D u_TexSampler;

// output to framebuffer
out vec4 out_Color;

void main()
{
    out_Color = texture2D(u_TexSampler, var_TexCoord);  // texture lookup
	//out_Color = vec4(var_TexCoord, 0, 1);    // write texture coordinate as color (useful for debugging)
}
