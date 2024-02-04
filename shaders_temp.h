#ifndef COLLISION_H
#define COLLSIION_H

#ifdef _WIN32
#pragma once
#endif

const char* shader_vert_src = R"glsl(
	#version 150 core
	
	uniform mat4 u_proj;
	uniform mat4 u_view;
	uniform mat4 u_model;

	in vec3 position;
	in vec4 color;
	in vec3 normal;
	in vec2 texcoord;
	
	out vec4 Color;
	out vec2 Texcoord;
	out vec3 Normal;
	
	void main() {
		Texcoord = texcoord;
		Color = color;
		Normal = normal;
		
		vec4 modelPos = u_model * vec4(position, 1.0f);
		vec4 viewPos = u_view * modelPos;

		gl_Position = u_proj * viewPos;
	}
)glsl";

const char* shader_frag_src = R"glsl(
	#version 150 core
	
	in vec4 Color;
	in vec2 Texcoord;
	in vec3 Normal;
	
	out vec4 outColor;
	
	uniform sampler2D tex;
	
	void main() {
		//outColor = vec4(Normal, 0.0f);
		//outColor = texture(tex, Texcoord) * vec4(Normal, 0.0f);
		//outColor = Color * dot(Normal, vec3(0.0f, 1.0f, 0.0f));
		
		//outColor = texture(tex, Texcoord) * ( vec4(1.0f, 1.0f, 1.0f, 1.0f) - (-0.5f * dot(Normal, vec3(0.5f, 1.0f, 0.0f))) );
		
		outColor = texture(tex, Texcoord) * Color;
		//outColor = texture(tex, Texcoord) * vec4(Color, 1.0f);
		//outColor = texture(tex, Texcoord);
		//outColor = vec4(Color, 1.0f);
	}
)glsl";

// For the debugging stuff
const char* shader_simple_vert_src = R"glsl(
	#version 150 core
	
	uniform mat4 u_proj;
	uniform mat4 u_view;
	uniform mat4 u_model;

	in vec3 position;
	in vec3 color;
	
	out vec3 Color;
	
	void main() {
		Color = color;
		
		vec4 modelPos = u_model * vec4(position, 1.0f);
		vec4 viewPos = u_view * modelPos;

		gl_Position = u_proj * viewPos;
	}
)glsl";

const char* shader_simple_frag_src = R"glsl(
	#version 150 core
	
	in vec3 Color;
	
	out vec4 outColor;
	
	void main() {
		outColor = vec4(Color, 1.0f);
		//outColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	}
)glsl";

#endif