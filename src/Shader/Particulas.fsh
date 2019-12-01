#version 330 core

layout(location=0) out vec4 vFragColor; // fragment shader output

//input from the vertex shader
smooth in vec4 vSmoothColor;	//lienarly interpolated particle colour

uniform sampler2D ParticleTex;	//particle texture
uniform bool Texture;
uniform bool FireWork;

void main()
{
	if(Texture){
		//use the particle smooth colour alpha value to fade the colour obtained
		//from the texture lookup 
		if(FireWork){
			vFragColor = texture(ParticleTex, gl_PointCoord)* vSmoothColor;
		}else{
			vFragColor = texture(ParticleTex, gl_PointCoord)* vSmoothColor.a; 
		}
	}else{
		//use the particle smooth colour as fragment output
		vFragColor = vSmoothColor;
	}
}