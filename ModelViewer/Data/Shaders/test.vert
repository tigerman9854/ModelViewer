attribute highp vec4 posAttr;
attribute lowp vec4 colAttr;
attribute highp vec3 normAttr;

varying lowp vec4 col;

uniform highp mat4 matrix;
uniform highp mat4 modelview;
uniform highp mat3 normalMat;

uniform vec3 uLightPos;

vec3 eyeLightPosition = uLightPos;

out lowp vec3 vNs;
out lowp vec3 vLs;
out lowp vec3 vEs;

void main() {
	vec4 ECposition = modelview * posAttr;

	vNs = normalize(normalMat * normAttr);

	vLs = eyeLightPosition - ECposition.xyz;

	vEs = vec3( 0., 0., 0. ) - ECposition.xyz;


   col = colAttr;
   gl_Position = matrix * posAttr;
}