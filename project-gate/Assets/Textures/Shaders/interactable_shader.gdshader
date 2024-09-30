shader_type spatial;
render_mode diffuse_burley;
//instance uniform bool is_clickable = false;

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	//if(is_clickable == true) {
	//	ALBEDO = vec3(0.0, 0.45, 0.2);
	//} else {
		ALBEDO = vec3(0.2, 0.2, 0.75);
	//}
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
