#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

uniform sampler2D s_Diffuse;
uniform sampler2D s_Diffuse2;
uniform sampler2D s_Specular;

uniform vec3  u_AmbientCol;
uniform float u_AmbientStrength;

uniform vec3  u_LightPos;
uniform vec3  u_LightCol;
uniform float u_AmbientLightStrength;
uniform float u_SpecularLightStrength;
uniform float u_Shininess;
// NEW in week 7, see https://learnopengl.com/Lighting/Light-casters for a good reference on how this all works, or
// https://developer.valvesoftware.com/wiki/Constant-Linear-Quadratic_Falloff
uniform float u_LightAttenuationConstant;
uniform float u_LightAttenuationLinear;
uniform float u_LightAttenuationQuadratic;

//cel shading & outline
//https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Toon_Shading
uniform vec4 _OutlineColor;
uniform float _LitOutlineThickness;
uniform float _UnlitOutlineThickness;

uniform float u_TextureMix;

uniform vec3  u_CamPos;

//for light toggle
uniform int u_Lightingtoggle;

out vec4 frag_color;

//apply cel shading
const int bands = 6;
const float scaleFactor = 1.0 / 6;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Lecture 5
	vec3 ambient = u_AmbientLightStrength * u_LightCol;

	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(u_LightPos - inPos);

	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * u_LightCol;// add diffuse intensity

    if (u_Lightingtoggle == 5)
    {
	//the banding effect for cel-shading
	diffuse = floor(diffuse * bands)*scaleFactor;
    }
    else
    {
        diffuse = dif * u_LightCol;
    }
	//Attenuation
	float dist = length(u_LightPos - inPos);
	float attenuation = 1.0f / (
		u_LightAttenuationConstant + 
		u_LightAttenuationLinear * dist +
		u_LightAttenuationQuadratic * dist * dist);

	// Specular
	vec3 viewDir  = normalize(u_CamPos - inPos);
	vec3 h        = normalize(lightDir + viewDir);

	// Get the specular power from the specular map
	float texSpec = texture(s_Specular, inUV).x;
	float spec = pow(max(dot(N, h), 0.0), u_Shininess); // Shininess coefficient (can be a uniform)
	vec3 specular = u_SpecularLightStrength * texSpec * spec * u_LightCol; // Can also use a specular color

	// Get the albedo from the diffuse / albedo map
	vec4 textureColor1 = texture(s_Diffuse, inUV);
	vec4 textureColor2 = texture(s_Diffuse2, inUV);
	vec4 textureColor = mix(textureColor1, textureColor2, u_TextureMix);




    if (u_Lightingtoggle == 1)
    {
     //do nothing since they are defined as 0 and should remain 0 for this
     ambient = vec3(0,0,0);
     diffuse = vec3(0,0,0);
     specular = vec3(0,0,0);
    }
    if (u_Lightingtoggle == 2)
    {
         
    ambient = u_AmbientLightStrength * u_LightCol;
    diffuse = vec3(0,0,0);
    specular = vec3(0,0,0);
    }
    if (u_Lightingtoggle == 3)
    {
         
    ambient = vec3(0,0,0);
    diffuse = dif * u_LightCol;
    specular = vec3(0,0,0);
    }
    if (u_Lightingtoggle == 4)
    {
       
    ambient =  u_AmbientLightStrength * u_LightCol;
    diffuse = dif * u_LightCol;
    specular = u_SpecularLightStrength * texSpec * spec * u_LightCol;
    }
    if (u_Lightingtoggle == 5)
    {
    ambient =  u_AmbientLightStrength * u_LightCol;
    specular = u_SpecularLightStrength * texSpec * spec * u_LightCol;
    }

	vec3 result = (
		(u_AmbientCol * u_AmbientStrength) + // global ambient light
		(ambient + diffuse + specular) * attenuation // light factors from our single light
		) * inColor * textureColor.rgb; // Object color

		





	frag_color = vec4(result, textureColor.a);
}