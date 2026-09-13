shader_type canvas_item;
uniform sampler2D world_data: filter_nearest;

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
vec4 data = texture(world_data, UV);
	float h = data.r; // 高度
	float m = data.g; // 水分
	float v = data.b; // 植被
	float c = data.a; // 覆盖层 (水/雪/岩浆)

	// 1. 基础土地层：水分越高，颜色越暗、越深
	vec3 soil_dry = vec3(0.5, 0.4, 0.3);
	vec3 soil_wet = vec3(0.2, 0.15, 0.1); 
	vec3 terrain_color = mix(soil_dry, soil_wet, m);

	// 2. 植被层：叠加绿色
	vec3 grass_dry = vec3(0.5, 0.5, 0.2);
	vec3 grass_wet = vec3(0.1, 0.4, 0.1);
	vec3 current_grass = mix(grass_dry, grass_wet, m);
	terrain_color = mix(terrain_color, current_grass, v);

	vec3 final_color = terrain_color;

	// 3. 覆盖层解码渲染
	if (c > 0.0 && c <= 0.4) {
		// 水体 (0.0-0.4)：c 越大水越深
		float d_factor = c / 0.4;
		
		vec3 water_shallow = vec3(0.4, 0.6, 0.9); // 浅水蓝
		vec3 water_deep = vec3(0.05, 0.1, 0.3);   // 深水蓝
		vec3 water_col = mix(water_shallow, water_deep, d_factor);
		
		// 当 d_factor 很小时（水很浅），混合系数接近 0，显示更多 terrain_color（池底）
		// 使用 smoothstep 让浅水处更透明，深水处更厚重
		float transparency = smoothstep(0.0, 0.5, d_factor); 
		final_color = mix(terrain_color, water_col, transparency);
	}
	else if (c > 0.4 && c <= 0.7) {
		// 积雪 (0.4-0.7)
		float snow_factor = (c - 0.4) / 0.3;
		final_color = mix(terrain_color, vec3(0.95, 0.95, 1.0), snow_factor);
	}
	else if (c > 0.7) {
		// 岩浆 (0.7-1.0)
		float magma_factor = (c - 0.7) / 0.3;
		vec3 magma_col = mix(vec3(0.8, 0.2, 0.0), vec3(1.0, 0.5, 0.0), magma_factor);
		final_color = mix(terrain_color, magma_col, magma_factor);
	}

	COLOR = vec4(final_color, 1.0);
}

//void light() {
//	// Called for every pixel for every light affecting the CanvasItem.
//	// Uncomment to replace the default light processing function with this one.
//}
