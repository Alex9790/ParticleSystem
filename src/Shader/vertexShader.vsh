#version 330

layout (location = 0) in vec3 s_vPosition;
layout (location = 1) in vec3 s_vNormal;
layout (location = 2) in vec2 s_vTexCoord;	
layout (location = 3) in vec4 VertexTangent;

out vec3 ReflectDir; 
uniform bool DrawSkyBox; 

out	vec3 Posicion;
out	vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;				// Projection * ModelView

void main () {
	if( DrawSkyBox ) {
		ReflectDir = s_vPosition;
	}else{

		// Transform normal and tangent to eye space
		Normal = normalize( NormalMatrix * s_vNormal);
	
		// Get the position in eye coordinates
		Posicion = vec3( ModelViewMatrix * vec4(s_vPosition,1.0) );		

		// Pass along the texture coordinate
		TexCoord = s_vTexCoord;
	}
	gl_Position = MVP * vec4(s_vPosition,1.0);
}