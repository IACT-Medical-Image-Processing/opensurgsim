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

/// \file ds_mapping.frag
/// Phong material with diffuse and shadow map

// These are 'free' uniforms to be set for this shader, they won't be provided by OSS
uniform float shininess;
uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;

// Oss provided uniforms
uniform vec4 ambientColor;

// Incoming from the vertex shader
varying vec3 lightDir;
varying vec3 eyeDir;
varying vec3 normalDir;

varying vec2 texCoord0;

varying vec3 vertexDiffuseColor;
varying vec3 vertexSpecularColor;

varying vec4 clipCoord;

void main(void) 
{	
	vec3 vAmbient = ambientColor.xyz; // Old Term ...  osg_ambientColor * _lightColor;

	vec2 shadowCoord = clipCoord.xy / clipCoord.w * vec2(0.5) + vec2(0.5);
	float shadowAmount = 1.0 - texture2D(shadowMap, shadowCoord).r;
    
	// Disable shadows for now
	// shadowAmount = 1.0;
	
	float diffuse = max(dot(lightDir, normalDir), 0.0);
	vec3 vDiffuse = vertexDiffuseColor * diffuse * shadowAmount;	
    
    float temp = max(dot(reflect(lightDir, normalDir), eyeDir), 0.0);
    float specular = temp / (shininess - temp * shininess + temp);
 	vec3 vSpecular = vertexSpecularColor * specular * shadowAmount;		

	vec3 base = texture2D(diffuseMap, texCoord0).rgb;
	vec3 color = (vAmbient + vDiffuse) * base + vSpecular;
	
    
	gl_FragColor.rgb = color;
	gl_FragColor.a = 1.0;
}