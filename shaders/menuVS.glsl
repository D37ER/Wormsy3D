#version 330

//Atrybuty
in vec2 vertex;
in int texId;
in vec2 texC;

//Zmienne interpolowane
out float texIndex;
out vec2 texCord;

void main(void) { 
	texIndex=texId;
	texCord=texC;
    gl_Position=vec4(vertex.xy, 0, 1);
}
