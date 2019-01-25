#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normals;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec3 v_Normal;
out vec4 v_ShadowCoord;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjMatrix;
uniform mat4 u_DepthBiasMVP;

void main()
{
	gl_Position = u_ProjMatrix * u_ViewMatrix * u_ModelMatrix * position;

	v_Position = (u_ModelMatrix * position).xyz;
	v_Normal = mat3(u_ModelMatrix) * normals;
	v_TexCoord = texCoord;

	v_ShadowCoord = u_DepthBiasMVP * position;
};

// --------------------------------------------------

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec2 v_TexCoord;
in vec3 v_Normal;
in vec4 v_ShadowCoord;

//uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform sampler2D u_depthTexture;
uniform vec3 u_LightPos0;
uniform vec3 u_CamPose;

vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
	);

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

	// Light map
	float visibility = 1.0;
	
	if (v_ShadowCoord.z < 1.0)
	{
		//float bias = 0.005; - одинаковое смещение! (слишком маленькое для для изогнутых поверхностей, слишком большое для ровных)
		float bias = max(0.05 * (1.0 - dot(v_Normal, posToLightDirVec)), 0.005); // изменение сдвига в зависимости от угла, под которым свет падает на поверхность. 
		for (int i = 0; i < 16; i++)
		{ 	
			if (texture(u_depthTexture, (v_ShadowCoord.xy/v_ShadowCoord.w) + poissonDisk[i]/700.0).z < (v_ShadowCoord.z/* - bias*/)/v_ShadowCoord.w)
			{ visibility -= 0.0625; }	// -= 1/16

			if (i == 3 && visibility <= 0.75)	// 4 samples in a row are in the shadow!
			{
				visibility = 0.0;
				break;
			}
		}
	}

	// Final color
	color = texColor * ( 
		vec4(ambientLight, 1.f) + 
		visibility * vec4(diffuseColor, 1.f) +
		visibility * vec4(specularLight, 1.f)
	);
};