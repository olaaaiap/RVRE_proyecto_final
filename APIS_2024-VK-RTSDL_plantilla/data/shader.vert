#version 330

uniform mat4 MVP; //(location=0)
uniform mat4 M;
attribute vec4 vPos;//(location=0) 
attribute vec4 vColor;//(location=1) 
attribute vec4 vNormal; //(location=ERROR) vNormal no se usa, no existe Pantallazo en negro
attribute vec2 vCoordText;//coordenadas de textura
attribute vec4 vTangent;//coordenadas de textura
attribute vec4 vBoneIdxs;
attribute vec4 vBoneWeights;


uniform mat4 bones[50];

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

uniform mat_t mat;
uniform vec3 cameraPos;
uniform mat4 depthBias;

out vec4 fDepthCoord;
out vec4 fColor;
out vec3 fNormal;
out vec2 fCoordText;
out vec3 fPos;
out vec3 fTexCube;
out mat3 TBN;



void main() {
	mat4 invT=inverse(transpose(M));
	vec4 finalPos=vPos;
	//animación por huesos

	if(mat.computeBones==1) {
		mat4 boneMatrix=mat4(1.0f);
		for(int i=0;i<4;i++) {
			int index=int(vBoneIdxs[i]);
			if(index>-1){
				boneMatrix+=bones[index]*vBoneWeights[i];
			}
		}
		finalPos=boneMatrix*finalPos;
	}


	fDepthCoord=depthBias*finalPos;
	gl_Position = MVP * finalPos;

	fPos=(M * finalPos).xyz;

	fColor=vColor;
	fNormal=normalize((invT*vNormal).xyz);
	fCoordText=vCoordText;

	fTexCube=fPos.xyz;
	if(mat.reflectionEnable==1)
	{
		vec3 I = vec3(normalize(fPos - cameraPos));
		fTexCube = reflect(I, vec3(fNormal));
	}
	if(mat.refractionEnable==1)
	{
		vec3 I = vec3(normalize(fPos - cameraPos));
		fTexCube=refract(I,fNormal, 1.0f/mat.refractionIndex);
	}

	vec3 tan= normalize((invT*vTangent).xyz);
	vec3 bitan=cross(tan.xyz,fNormal.xyz);
	TBN=transpose(mat3(tan,bitan,fNormal.xyz));


}