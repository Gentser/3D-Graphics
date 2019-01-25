#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_DepthMVP;

void main()
{
	gl_Position = u_DepthMVP * vec4(position, 1);
};

// --------------------------------------------------

#shader fragment
#version 330 core

layout(location = 0) out float fragmentdepth;

void main()
{
	fragmentdepth = gl_FragCoord.z;
};