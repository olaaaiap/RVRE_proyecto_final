#version 330
#extension GL_NV_shadow_samplers_cube : enable

uniform sampler2D textureColor;
uniform sampler2D textureNormal;
uniform sampler2D textureDepth;

uniform samplerCube cubetextureColor;



struct mat_t{
	int shinny;
	vec4 color;
	int enable;
	int usetextureDepth;
	int usetextureColor;
	int usecubetextureColor;
	int usetextureNormal;
	int receiveLight;
	int reflectionEnable;
	int refractionEnable;
	float refractionIndex;
	int shadowEnable;
	int computeBones;
};

struct light_t{
	int enable;
	vec3 pos;
	vec4 color;
};
uniform light_t light;
uniform mat_t mat;
uniform vec3 cameraPos;

in vec4 fDepthCoord;
in vec2 fCoordText;
in vec4 fColor;
in vec3 fNormal;
in vec3 fPos;
in vec3 fTexCube;
in mat3 TBN;




void main() {
	vec4 finalColor=vec4(0,0,0,1);
	vec4 baseColor=vec4(0,0,0,1);
	vec4 lightColor=vec4(1,1,1,1);
	vec3 normal=fNormal;
	float shadow=1.0f;

	if(mat.shadowEnable==1 && mat.usetextureDepth==1){
			if(texture2D(textureDepth,vec2(fDepthCoord.x,fDepthCoord.y)).z
							<(fDepthCoord.z - 0.0009))
				shadow=0.2f;//gradiente de sombra
	}


	if(mat.usetextureColor==1){
		baseColor= texture2D(textureColor,fCoordText);
	}
	else if(mat.usecubetextureColor==1)
	{
		baseColor=textureCube(cubetextureColor,fTexCube);
	}
	else
		baseColor=fColor;
	if(mat.enable==1)
	{	
		baseColor+=mat.color;//sumar o multiplicar
		baseColor/=2.0f;
	}

	if(mat.usetextureNormal==1)
	{
		vec3 normalTextValue=texture2D(textureNormal,fCoordText).xyz;
		normalTextValue=(normalTextValue*2.0f)-1.0f;
		normal=TBN*normalTextValue; 
		normal=normalize(normal);
	}

	if(mat.receiveLight==1 && light.enable==1)
	{
		//calcular ambiente
		float ambient=0.2f;

		//calcular difusa
		vec3 L=normalize(light.pos-fPos);

		float diffuse=max(dot(L,normal),0.0f);
		//calcular especular
		vec3 R=normalize(reflect(L,normal));
		vec3 EYE=normalize(fPos-cameraPos);
		float specular=pow(max(dot(R,EYE),0.0f),mat.shinny);
		lightColor=(ambient+diffuse+specular)*light.color;
	}

	finalColor=baseColor*lightColor*shadow;
	gl_FragColor=finalColor;
}
