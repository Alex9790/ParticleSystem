#version 330 core
  
layout (location = 0) in vec3 VertexInitVel; // Particle initial velocity
layout (location = 1) in vec3 Color; // Particle initial velocity

smooth out vec4 vSmoothColor;	//output to fragment shader

//shader uniforms
uniform mat4 MVP;				//combined modelview matrix 
uniform float time;				//current time
 
//particle attributes
uniform vec3 a;					//acceleration of particles
vec3 g = vec3(0,-9.8,0);			//if you want acceleration due to gravity 
uniform vec2 RangoX;
uniform vec2 RangoZ;

uniform float rate;				//rate of emission
uniform float life;				//life of particle

//constants
const float PI = 3.14159;
const float TWO_PI = 2*PI;

//colormap colours
uniform vec3 COLOR1;
uniform vec3 COLOR2; 

uniform float Size;
uniform float SizeParticle;
uniform float SizeMinParticle;
uniform bool lluvia;
uniform bool Fuego;
uniform bool Fuego2;
uniform bool Humo;
uniform bool Tornado;
uniform bool FireWork;

//pseudorandom number generator
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

//pseudorandom direction on a sphere
vec3 uniformRadomDir(vec2 v, out vec2 r, float thetaVal) {
	r.x = rand(v.xy);
	r.y = rand(v.yx);
	float theta = mix(0.0, PI / thetaVal, r.x);
	float phi = mix(0.0, TWO_PI, r.y);
	return vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
}

void main()
{
	//local variables
	vec3 pos=vec3(0);
	vec2 rect;

	//get the spawn time by multiplying the current vertex ID with
	//the rate of emission
	float t = gl_VertexID*rate;
	//start with alpha of 1 (fully visible)
	float alpha = 1;
	
	if(time>t) {
		float dt = mod((time-t), life);
		vec2 xy = vec2(gl_VertexID,t); 
		vec2 rdm = vec2(0);
		float agePct = dt/life;

		//point emitter source
		//pos = ((uniformRadomDir(xy, rdm, 6.0) + 0.5*(a+g)*dt)*dt); //for adding gravity 	   
	    	
		if(lluvia){
			pos = ( uniformRadomDir(xy, rdm, 6.0) + 0.5*a*dt)*dt;       //for fire effect from a quad emitter
			//rect = (rdm*2.0 - 1.0);
			rect = (rdm*Size);
			pos += vec3(rect.x, 0, rect.y);
			gl_PointSize = SizeParticle;
	    }		
	    			
	    if(Fuego){
			pos = ( uniformRadomDir(xy, rdm, 6.0) + 0.5*a*dt)*dt;       //for fire effect from a disc emitter
			rect = (rdm*2.0 - 1.0);
			float dotP = dot(rect, rect);
	    
			if(dotP<1)
				pos += vec3(rect.x*Size, 0, rect.y*Size);

			gl_PointSize = SizeParticle;
	    }

		if(Fuego2){
			vec3 Hola = uniformRadomDir(xy, rdm, 6.0); 
			float aux = 1.0*dt;
			pos = vec3(mix(RangoX.x, RangoX.y, rdm.x) + 0.5*a.x*dt*dt, aux*a.y, mix(RangoZ.x, RangoZ.y, rdm.y) + 0.5*a.z*dt*dt);
			gl_PointSize = mix(SizeParticle, 1.0, agePct);
		}
	    
		if(Humo){
			pos = ((uniformRadomDir(xy, rdm, 1.5) + 0.5*a*dt)*dt);     
			gl_PointSize = mix(SizeMinParticle, SizeParticle, agePct);
		}

		if(Tornado){
			pos = ((uniformRadomDir(xy, rdm, 1.5) + 0.5*a*dt)*dt);     
			gl_PointSize = mix(SizeMinParticle, SizeParticle, agePct);
		}		

		alpha = 1.0 - (dt/life);	

		//linearly interpolate between red and yellow colour
		vSmoothColor = vec4(mix(COLOR1,COLOR2,alpha),alpha);

		if(FireWork){
			pos += VertexInitVel * dt * a.x;		
			gl_PointSize = SizeParticle;
			vSmoothColor = vec4(Color, alpha);
		}

		 
	}
   
	
	//get clipspace position
	gl_Position = MVP*vec4(pos,1);
}
