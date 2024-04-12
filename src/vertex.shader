#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normals; // Or is textCoords

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int isModel; // Depends on if we have a model or not
uniform vec3 Color;
uniform vec3 SunLocation;

out vec3 Normal;
out int isModel_;
out vec3 Color_;
out vec3 SunLocation_;
out vec3 pos_;

void main()
{
    // Copy values over
    Normal = normals;
    isModel_ = isModel;
    Color_ = Color;
    SunLocation_ = SunLocation;
    pos_ = pos;

    // If not a model, do not use model matrix
    if(isModel == 1){
        gl_Position = projection * view * model * vec4(pos,1);
    }else{
        gl_Position = projection * view * vec4(pos,1);
        
    }
}