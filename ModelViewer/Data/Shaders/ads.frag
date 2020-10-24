varying lowp vec4 col;


in lowp vec3 vNs;
in lowp vec3 vLs;
in lowp vec3 vEs;

uniform float uKa, uKd, uKs;
uniform vec4 uSpecularColor;
uniform float uShininess;

void main() {
	vec3 Normal;
	vec3 Light;
	vec3 Eye;
	
	Normal = normalize(vNs);
	Light = normalize(vLs);
	Eye = normalize(vEs);

	vec4 ambient = uKa * col;

	float d = max(dot(Normal, Light), 0.);
	vec4 diffuse = uKd * d * col;

	float s = 0.;
	if( dot(Normal,Light) > 0. ) // only do specular if the light can see the point
	{
		vec3 ref = normalize( 2. * Normal * dot(Normal,Light) - Light );
		s = pow( max( dot(Eye,ref),0. ), uShininess );
	}

	vec4 specular = uKs * s * uSpecularColor;

   gl_FragColor = vec4( ambient.rgb + diffuse.rgb + specular.rgb, 1. );
}