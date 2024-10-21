#version 330 core
out vec4 fragColor;

in vec3 gobalPosition;

uniform sampler3D fluidParticlesB;
uniform sampler3D fluidFlowB;

uniform sampler1D pallet;
uniform vec3 camPos;

float hash13(vec3 p3)
{
	p3  = fract(p3 * .1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}

void main()
{
	ivec3 textureSize = textureSize(fluidFlowB, 0);
	vec3 rayDir = normalize(gobalPosition - camPos);
	vec3 rayPos = gobalPosition * vec3(textureSize) + rayDir * 0.01;
	vec3 mapPos = floor(rayPos);
	vec3 deltaDist = abs(1.0 / rayDir);
	vec3 rayStep = sign(rayDir);
	vec3 sideDist = (rayStep * (0.5 - fract(rayPos)) + 0.5) * deltaDist; 	
	vec3 mask = vec3(0.0);
    vec4 voxel = vec4(0.0);
    
	while (true)
    {
        voxel = texelFetch(fluidFlowB, ivec3(mapPos), 0);
        
		if (voxel.w > 0.05)
        {
            fragColor = vec4(voxel.rgb, 1.0) - vec4(hash13(mapPos) / 4.0);
            return;
        }

        mask = vec3(lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy)));
		
		sideDist += mask * deltaDist;
		mapPos += mask * rayStep;

		if (any(lessThan(mapPos, ivec3(0))) || any(greaterThan(mapPos, textureSize)))
        {
			break;
        }
	}

	fragColor = vec4(0.6, 0.8, 1.0, 1.0);
}