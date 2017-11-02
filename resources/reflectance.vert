#version 330

uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform vec4 specAlpha;

uniform mat4 P;  //projection matrix
uniform mat4 Lp;  //light projection matrix
uniform mat4 mT;
uniform mat4 C;   //camera matrix
uniform mat3 N;  //inverse transpose of upperleft 3x3 of M

uniform mat4 M;  //modelview matrix: M = C * mT
uniform mat4 Lv; //light view matrix
uniform mat4 Lr;  //light rotation matrix

uniform vec4 lightPos;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 colorIn;

out vec3 smoothPos;
out vec3 smoothNorm;
out vec4 smoothColor;
out vec4 shadowPos;
flat out vec4 flatColor;


vec4 reflectance(in vec3 pos, in vec3 norm, in vec3 colorIn, in float visibilityFactor)
{
    float alpha = 10;
    vec4 p = vec4(pos, 1);
    vec4 lp = lightPos;
    vec4 n = normalize(vec4(norm,0));
    vec4 c = vec4(0);
    vec4 Li = vec4(1);
    vec4 ka = Ka;
    vec4 kd = Kd;
    vec4 ks = Ks;
    
    lp = C*Lr*lp;
    p = M*p;
    n = vec4(N*n.xyz, 0);
    
    // diffuse coefficent
    vec4 l = normalize(lp - p);
    float d = clamp(dot(l,n), 0, 1);
    
    // specular coefficient
    vec4 v = normalize(c-p);
    vec4 r = normalize(reflect(-l,n));
    float s = clamp(dot(v,r), 0, 1);
    s = pow(s, alpha);
    
    return ka*Li + kd*d*Li + ks*s*Li;
}


vec4 positionInLightView(vec3 pos)
{
	mat4 offset =
	mat4(
		 0.5, 0.0, 0.0, 0.0,
		 0.0, 0.5, 0.0, 0.0,
		 0.0, 0.0, 0.5, 0.0,
		 0.5, 0.5, 0.5, 1.0);
	
	//TODO: move vertex, put in light view
	
	return vec4(pos, 1);
}

void main()
{
	smoothPos = pos;
	smoothNorm = colorIn*2 - 1;
	smoothColor = reflectance(smoothPos, smoothNorm, smoothColor.xyz, 1);
    //vec4(colorIn, 1);
	shadowPos = positionInLightView(pos);
	
	vec4 p = vec4(pos, 1);
	gl_Position = P*M*p;
    flatColor = reflectance(smoothPos, smoothNorm, smoothColor.xyz, 1);
}
