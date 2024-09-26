shader_type spatial;
render_mode cull_back, unshaded;

uniform sampler2D image : source_color;
uniform sampler2D DEPTH_TEXTURE: hint_depth_texture, filter_linear_mipmap;
uniform vec2 tiling = vec2(1.0);
uniform vec2 offset = vec2(0.0);

varying vec4 NODE_POSITION_CLIP;
varying mat4 CAMERA;

void vertex() {
	 NODE_POSITION_CLIP = (PROJECTION_MATRIX * vec4(NODE_POSITION_VIEW, 1.0));
	 CAMERA = INV_VIEW_MATRIX;
}

void fragment() {
	//mat4 start = mat4(1.0);
	//float depth = texture(DEPTH_TEXTURE, SCREEN_UV, 0.0).x;
	//vec3 ndc = vec3(SCREEN_UV * 2.0 - 1.0, depth);
	//vec2 local_uv = (SCREEN_UV * 2.0 - 1.0) * NODE_POSITION_CLIP.w - NODE_POSITION_CLIP.xy;

	//vec4 world = CAMERA * PROJECTION_MATRIX * vec4(local_uv, 1.0, 1.0);
	//vec3 world_position = world.xyz / world.w;
	//vec3 adjusted_position = (inverse(start) * vec4(world_position, 1.0)).xyz;
	// Adjust for aspect ratio and FOV
	//local_uv.x *= VIEWPORT_SIZE.x / VIEWPORT_SIZE.y;
	//local_uv *= -1.0 / PROJECTION_MATRIX[1][1];

	//ALBEDO = mix(original.rgb, color.rgb, adjusted_position);
	vec4 texture_albedo = texture(image, UV);
	vec3 world_abs_y = vec3((INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).y, 0.0, 0.0);
	
	vec3 world_avs_x = vec3((INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).x, 0.0, 0.0);
	vec3 step = smoothstep(vec3(0.0), world_avs_x, vec3(0.1));
	vec3 test = dot(vec3(1.0), world_abs_y) < 0.0 ? -world_abs_y : cross(cross(world_abs_y, vec3(1)), world_abs_y);
	test = test * abs(sin(TIME)) * step;
	//vec3 output = mix(test, vec3(0.0), fresnel);
	ALBEDO = test * texture_albedo.rgb;
	ALPHA = texture_albedo.r;
}