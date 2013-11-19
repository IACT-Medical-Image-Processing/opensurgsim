// This file is a part of the OpenSurgSim project.
// Copyright 2013, SimQuest Solutions Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file shadow_map.frag
/// Calculate a shadow map that can be used for modulating the color of each
/// fragment by the amount of shadow that should be applied to that color

/// Texture rendered from the view of the light, containing the distance of each fragment
/// from the light, for higher resolution, if the z value of the incoming fragment projected into 
/// light space is close to the value in the texture map then the fragment is lit, otherwise 
/// it is not
uniform sampler2D oss_encodedLightDepthMap;


/// The coordinates of the fragment in the space of the projected depth map
varying vec4 lightCoord;

float decodeDepth(vec4 color)
{
	return dot(color, vec4(1.0, 1.0/255.0, 1.0/(255.0*255.0), 1.0/(255.0*255.0*255.0)));
}

void main(void) 
{	
    vec3 lightCoord3 = (lightCoord.xyz / lightCoord.w) * vec3(0.5) + vec3(0.5);
	float depth = decodeDepth(texture2D(oss_encodedLightDepthMap, lightCoord3.xy));
	float a = (depth + 0.00001 > lightCoord3.z ? 0.0 : 1.0);
	gl_FragColor = vec4(depth + 0.00001 > lightCoord3.z ? 0.0 : 1.0);
}