#version 450 core
out vec4 FragColor;
// Normal can be either the normal or the texture coordinates, depending on isModel_
in vec3 Normal;
in flat int isModel_;

in vec3 Color_;
in vec3 SunLocation_;
in vec3 pos_;

uniform samplerCube cubeMap;
uniform sampler2D texture1;


uniform float time;

uniform float wavelength;
uniform float amplitude;
uniform float rr;

uniform vec3 viewDirection;

// We only rotate the x and y components
vec3 rotate(vec3 vec, float angle){
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    vec2 v = vec.xy;
    v = v*rot;

    return vec3(v.x,v.y,vec.z);

}


// Functions I made a long time ago to handle 3d rotations
mat4 RotateX(float phi) {
    return mat4(
        vec4(1., 0., 0., 0),
        vec4(0., cos(phi), -sin(phi), 0.),
        vec4(0., sin(phi), cos(phi), 0.),
        vec4(0., 0., 0., 1.));
}

mat4 RotateY(float theta) {
    return mat4(
        vec4(cos(theta), 0., -sin(theta), 0),
        vec4(0., 1., 0., 0.),
        vec4(sin(theta), 0., cos(theta), 0.),
        vec4(0., 0., 0., 1.));
}

mat4 RotateZ(float psi) {
    return mat4(
        vec4(cos(psi), -sin(psi), 0., 0),
        vec4(sin(psi), cos(psi), 0., 0.),
        vec4(0., 0., 1., 0.),
        vec4(0., 0., 0., 1.));
}

void main()
{

    // Get the coordinates in terms of -1 to 1 for x and y rather than 0 to 1
    float x = 2*(Normal.x - .5);
    float y = 2*(Normal.y - .5);

    // Get the distance to the center of the waves. The -100 is for scaling to make it look nicer
    float dist = -100*length(vec2(Normal.x, Normal.y) - vec2(.5,.5));

    float a = amplitude;
    // add a little scaling to the speed so we can see the waves move.
    float b = 10.0;

    // Get the slope at this point using the derivative of the wave height function
    float hPrime = a/wavelength * cos((dist + b*time)/wavelength);

    // We will have the slope as a vector be y = slope, x = 1 to keep the proportions right
    vec3 slopeVec = vec3(1, 0, (-1.0f/hPrime));

    // Get our angle so we can rotate the normal vector
    float angle = atan(y,x) * 180.0f/3.141592650f;


    // The normal needs to be rotated around the xy plane due to rotational symetry.
    vec3 normal = rotate(slopeVec, angle/180.0 * 3.1415);


    // Get the angle between the normal and the view direction
    vec3 viewDirection = vec3(0,0,-1);
    float nwater = 2.0f;

    // Get our alpha value and beta values
    float alpha = acos(dot(viewDirection, normalize(normal)));
    float beta = asin(1.0f/nwater * sin(alpha));



    // now get the new uv coords from the normal. This is the displacement we will add to the regular ray 
    // To emulate the snell's law effect

    // This is our view vector which we need to rotate by beta. 
    vec3 direction = vec3(0,0,-1);

    // Angle off x axis
    float a1 = acos(dot(direction, vec3(1,0,0)));

    // Angle off y axis
    float a2 = acos(dot(direction, vec3(0,1,0)));
    // Notice the above values are just 90 degrees due to our situation but in other cases would need to be rotated


    // We want the matrix to oppose the rotation
    mat4 a1Mat = RotateX(-a1);
    mat4 a2Mat = RotateY(-a2);

    // Create a direction vector, but as homogeneous coords
    vec4 dir = vec4(0,0,-1, 1);

    // Undo this rotation. Align along the z plane
    dir = a1Mat * a2Mat * dir;

    // Now rotate about the z axis since we are aligned about it
    dir = RotateZ(beta) * dir;

    // Redo the rotations
    dir = RotateX(a1) * RotateY(a2) * dir;


    // Our water component is the regular texture coordinates (called Normal) plus the normalized direction
    vec4 water = texture(texture1, Normal.xy + normalize(dir.xyz).xy);

    // Diffuse is the dot product of the light direction and the displaced light direction
    float dot_ = dot(vec3(0,0,-1), normalize(dir.xyz));

    // Now add in the diffuse lighting effect
    water.xyz -= water.xyz * abs(dot_);


    // The sky component is the normal vector reflecting the view direction
    normal = rotate(slopeVec, angle/180.0 * 3.1415);
    // I'm using the built in reflect function but i know the formula is: r = d - 2(d * n) * n
    vec3 direction_ = reflect(normalize(normal), vec3(0,0,-1));
    // Get the component from the cubeMap
    vec4 sky = texture(cubeMap, direction_);




    // We want rr = 1 to be only sky and rr = 0 to be only water
    // Notice 4* the sky because its very dim so I want it to be brighter
    FragColor = vec4(4* rr *sky.xyz + (1-rr) * water.xyz, 1);


}