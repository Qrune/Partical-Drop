#version 330

uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform vec4 specAlpha;

uniform mat4 P;   //projection matrix
uniform mat4 C;   //camera matrix
uniform mat4 M;   //modelview matrix: M = C * mT
uniform mat3 N;   //inverse transpose of upperleft 3x3 of M
uniform mat4 Lr;  //light rotation matrix
uniform vec4 lightPos;    //light position
uniform vec4 camPos;      //camera position
uniform int curMode; //current shading mode
uniform int colorChange;
uniform sampler2D texId;
uniform sampler2D texSampler;
//uniform sampler2DRect texId;
flat in vec4 flatColor;
in vec2 texMapping;
in float timeOut;

in vec4 smoothColor;
in vec3 smoothPos;
in vec3 smoothNorm;
in vec4 shadowPos;
vec4 changeColor;

layout(location = 0) out vec4 fragColor;
vec3 colorA = vec3(0.149,0.141,0.912);
vec3 colorB = vec3(1.000,0.833,0.224);


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
	float d = clamp(dot(l,n), 0, 1) * visibilityFactor;

	// specular coefficient
	vec4 v = normalize(c-p);
	vec4 r = normalize(reflect(-l,n));
	float s = clamp(dot(v,r), 0, 1);
	s = pow(s, alpha) * visibilityFactor;
	
	return ka*Li + kd*d*Li + ks*s*Li;
}


float scaleToRange(float v, vec2 bounds) {
	float range = bounds[1] - bounds[0];
	return (v - bounds[0]) / range;
}

vec4 redBlueMap(float v, vec3 bounds) {
	float m;
	if(v < bounds[1])
	{
		m = scaleToRange(v, bounds.xy);
		return mix (vec4(1,0,0,0), vec4(1,1,1,1), m);
	}
	
	m = scaleToRange(v, bounds.yz);
	return mix(vec4(1,1,1,1), vec4(0,0,1,0), m);
}

float getBias()
{
	return 0.004;
	
	//TODO: could do more complex stuff related to normals
}

vec4 getShadowPoint()
{
	//TODO: adjust for the perspective divide and bias
	vec4 shp = shadowPos;
	return shp;
}

float getVisibility()
{
	float visibilityFactor = 1.0;
	vec4 shadowPoint = getShadowPoint();
	
	//TODO lookup the distance in the shapow map, get surface and occlusion distance
	float surfaceDistance = 1.0;
	float occlusionDistance = 1.0;
	
	float diff = occlusionDistance - surfaceDistance;
	fragColor = redBlueMap(diff, vec3(-0.1, 0, 0.1));
	
	if(diff < 0)
		return 0.3;
	return 1.0;
}

void main()
{
	float visibilityFactor = getVisibility();
    if (curMode == 0){
        fragColor = reflectance(smoothPos, smoothNorm, smoothColor.xyz, visibilityFactor);
    }
    else if (curMode == 1){
        fragColor = smoothColor;
    }
    else if (curMode == 2)
    {
        vec3 edi = vec3((flatColor.x+flatColor.y+flatColor.z)/3);
        fragColor = vec4(edi,flatColor.x);
        changeColor = vec4(0,0,0,0);
        //fragColor = reflectance(smoothPos, smoothNorm, vec3(1,0.5,0.5), 1);
    }
    else if (curMode == 3){
        float pct = abs(sin(timeOut));
        
        // Mix uses pct (a value from 0-1) to
        // mix the two colors
        vec4 changeColor = smoothColor;
        if (colorChange == 1)
            changeColor = vec4(colorA,1);
        
        fragColor = changeColor;//vec4(colorB,1);
    }
    else{
        fragColor = vec4(1,0,1,0);
    }
}

