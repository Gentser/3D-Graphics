#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normals;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjMatrix;

void main()
{
	gl_Position = u_ProjMatrix * u_ViewMatrix * u_ModelMatrix * position;

	v_Position = (u_ModelMatrix * position).xyz;
	v_Normal = mat3(u_ModelMatrix) * normals;
	v_TexCoord = texCoord;
};

// --------------------------------------------------

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec2 v_TexCoord;
in vec3 v_Normal;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform vec3 u_LightPos0;
uniform vec3 u_CamPose;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord);

	// Ambient light
	vec3 ambientLight = vec3(0.2f, 0.2f, 0.2f);

	// Diffuse light
	vec3 posToLightDirVec = normalize(u_LightPos0 - v_Position);
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	float diffuse = clamp(dot(posToLightDirVec, v_Normal), 0, 1);
	diffuseColor = diffuseColor * diffuse;

	// Specular light
	vec3 lightToPosDirVec = normalize(v_Position - u_LightPos0);
	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(v_Normal)));
	vec3 posToViewDirVec = normalize(u_CamPose - v_Position);
	float specularConstant = pow( max(dot(posToViewDirVec, reflectDirVec), 0), 30 );
	vec3 specularLight = vec3(1.f, 1.f, 1.f) * specularConstant;

	// Final color
	color = texColor * ( vec4(ambientLight, 1.f) + vec4(diffuseColor, 1.f) + vec4(specularLight, 1.f) );
	//color = vec4(1,1,1,1) * (vec4(ambientLight, 1.f) + vec4(diffuseColor, 1.f) + vec4(specularLight, 1.f));
};