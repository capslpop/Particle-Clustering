#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4x4 camMat;
uniform vec3 camPos;

struct ChunkComputeList
{
	ivec4 vVolumeLocation;
    ivec4 pVolumeLocation;
    ivec4 location;
};

out vec3 gobalPosition;

void main()
{
    gl_Position = camMat * vec4(aPos, 1.0);

    gobalPosition = aPos;
}