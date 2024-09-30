shader_type spatial;
render_mode diffuse_burley;
//instance uniform bool is_clickable = false;

uniform vec4 shine_color : source_color = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform float cycle_interval : hint_range(0.5, 5.0) = 1.0;
uniform float shine_speed : hint_range(1.0, 5.0) = 3.0;
uniform float shine_width : hint_range(1.0, 100.0) = 3.0;

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {

	//if(is_clickable == true) {
	//	ALBEDO = vec3(0.0, 0.45, 0.2);
	//} else {
		ALBEDO = vec3(0.2, 0.6, 0.6);
	//}
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
