extends Node2D

@onready var map_display_node: ColorRect = $MapDisplayNode
@onready var tile_cursor: TileCursor = $TileCursor
@export var info_panel_scene: PackedScene
var info_panel: Control

var last_highlight_idx: int = -1

# Map Settings
@export var map_width: int = 256
@export var map_height: int = 256
@export var tile_size: int = 32

# Data Storage
var data_current: WorldState
var data_next: WorldState

# Noise Settings
var noise: FastNoiseLite = FastNoiseLite.new()
var fluc_noise_list: Array[FastNoiseLite] = []

# Erosion
@export var erosion_threshold: float = 10.0
@export var erosion_rate: float = 0.1
@export var river_erosion_rate: float = 0.25
@export var max_drop_lifespan: int = 64

var erosion_rate_corner: float =  0.1 #/ sqrt(2)
var river_erosion_rate_corner: float = 0.25 #/ sqrt(2)

# Threads
@export var chunk_size: int = 64
var chunks: Array = []
var active_threads: Array[Thread] = []
var chunks_completed: int = 0
var is_updating: bool = false
var next_chunk_idx: int = 0
var semaphore = Semaphore.new()
var mutex = Mutex.new()
var _is_shutting_down: bool = false

# Water Flow Update Interval
@export var water_flow_update_interval_sec: float = 0.5
var water_flow_elapsed_sec: float = 0.0

# Water Cycle & Vegetation Update Interval
@export var eco_update_interval_sec: float = 1.0
var eco_elapsed_sec: float = 0.0

# Water Cycle Settings
@export var evap_rate: float = 0.03 # water -> vapor per tick
@export var condense_rate: float = 0.10 # vapor -> rain (water) when saturated
@export var infiltration_rate: float = 0.04 # water -> soil moisture per tick
@export var soil_dry_rate: float = 0.01 # moisture -> vapor per tick
@export var soil_capacity_base: float = 0.35
@export var soil_capacity_veg_bonus: float = 0.35
@export var rain_threshold_vapor: float = 0.65
@export var rain_amount: float = 3.0

# Vapor Transport (Weather)
@export var wind_dir: Vector2 = Vector2(1.0, 0.0) # +x east, +y south
@export var vapor_advect_rate: float = 0.35 # fraction of vapor moved per eco tick
@export var vapor_diffuse_rate: float = 0.03 # post-advection smoothing

# Orographic Effects (mountain rain)
@export var orographic_condense_strength: float = 0.25
@export var orographic_height_scale: float = 220.0
@export var orographic_rain_amount: float = 2.0

# Vegetation Settings
@export var veg_growth_rate: float = 0.02
@export var veg_decay_rate: float = 0.02
@export var veg_opt_moisture: float = 0.55
@export var veg_moisture_width: float = 0.35
@export var veg_waterlog_decay: float = 0.03

# Eco scratch buffers (reused to avoid allocations)
var _eco_w1 := PackedFloat32Array()
var _eco_m1 := PackedFloat32Array()
var _eco_v1 := PackedFloat32Array()
var _eco_vapor1 := PackedFloat32Array()
var _eco_vapor2 := PackedFloat32Array()
var _eco_vapor3 := PackedFloat32Array()

# Instant Refreshment
var need_regenerate: bool = false
var need_thermally_erode: bool = false
var need_drop_water: bool = false

class Chunk:
	var x_start: int
	var y_start: int
	var width: int
	var height: int
	var is_processed: bool = false

func _ready() -> void:
	data_current = WorldState.new(map_width, map_height)
	data_next = WorldState.new(map_width, map_height)
	
	map_display_node.custom_minimum_size = Vector2(map_width * tile_size, map_height * tile_size)
	map_display_node.size = map_display_node.custom_minimum_size
	
	tile_cursor.tile_size = tile_size
	
	erosion_rate_corner = erosion_rate / 1.414
	river_erosion_rate_corner = river_erosion_rate / 1.414
	
	noise.frequency = 0.02
	noise.noise_type = FastNoiseLite.TYPE_PERLIN
	
	fluc_noise_list.resize(3)
	var freq: float = noise.frequency
	for fluc_n_idx in range(fluc_noise_list.size()):
		fluc_noise_list[fluc_n_idx] = FastNoiseLite.new()
		freq *= 2.0
		fluc_noise_list[fluc_n_idx].frequency = freq
		fluc_noise_list[fluc_n_idx].noise_type = FastNoiseLite.TYPE_PERLIN
	
	generate_map()
	_setup_ui()
	tile_cursor.hovered_tile_changed.connect(_on_hovered_tile_changed)
	
	_init_chunks()
	_start_worker_threads()
	
func _init_chunks():
	chunks.clear()
	for y in range(0, map_height, chunk_size):
		for x in range(0, map_width, chunk_size):
			var c = Chunk.new()
			c.x_start = x
			c.y_start = y
			c.width = min(chunk_size, map_width - x)
			c.height = min(chunk_size, map_height - y)
			chunks.append(c)	
			
	
func _start_worker_threads():
	var thread_count = OS.get_processor_count() - 1
	for i in range(thread_count):
		var t = Thread.new()
		t.start(_thread_worker_loop)
		active_threads.append(t)
		
func _thread_worker_loop():
	while true:
		semaphore.wait()
		
		if _is_shutting_down:
			break
		
		while true:
			var chunk = _get_next_available_chunk()
			if chunk == null: break
			_process_water_flow_chunk(chunk)
			mutex.lock()
			chunks_completed += 1
			mutex.unlock()
			
func _process_water_flow_chunk(chunk: Chunk):
	# Major hot path: avoid per-tile repeated neighbor scans via caching.
	var h_curr := data_current.height_map
	var w_curr := data_current.water_depth_map
	var m_curr := data_current.moisture_map

	# 注意：我们这一轮只写入属于本 chunk 的 idx，保证 100% 线程安全
	var w_next := data_next.water_depth_map
	var m_next := data_next.moisture_map

	# Cache for chunk + 1-tile border (halo), so inflow checks are O(1).
	var rx0 := maxi(0, chunk.x_start - 1)
	var ry0 := maxi(0, chunk.y_start - 1)
	var rx1 := mini(map_width - 1, chunk.x_start + chunk.width) # inclusive
	var ry1 := mini(map_height - 1, chunk.y_start + chunk.height) # inclusive
	var r_w := rx1 - rx0 + 1
	var r_h := ry1 - ry0 + 1
	var r_size := r_w * r_h

	var best_neighbor := PackedInt32Array()
	best_neighbor.resize(r_size)
	var min_surface := PackedFloat32Array()
	min_surface.resize(r_size)

	# Pass 1: compute each tile's best downhill neighbor + local minimum surface.
	for y in range(ry0, ry1 + 1):
		var row_base := y * map_width
		var ly := y - ry0
		for x in range(rx0, rx1 + 1):
			var idx := row_base + x
			var surface_here := float(h_curr[idx]) + float(w_curr[idx])
			var best_idx := -1
			var min_h := surface_here

			var y0 := maxi(0, y - 1)
			var y1 := mini(map_height - 1, y + 1)
			var x0 := maxi(0, x - 1)
			var x1 := mini(map_width - 1, x + 1)
			for ny in range(y0, y1 + 1):
				var n_row := ny * map_width
				for nx in range(x0, x1 + 1):
					if nx == x and ny == y:
						continue
					var n_idx := n_row + nx
					var n_surface := float(h_curr[n_idx]) + float(w_curr[n_idx])
					if n_surface < min_h:
						min_h = n_surface
						best_idx = n_idx

			var lidx := ly * r_w + (x - rx0)
			best_neighbor[lidx] = best_idx
			min_surface[lidx] = min_h

		# Pass 2: update next water/moisture for the chunk.
		# Note: eco (vapor/vegetation/moisture evolution) is handled separately.
	for y in range(chunk.y_start, chunk.y_start + chunk.height):
		var row_base := y * map_width
		var ly := y - ry0
		for x in range(chunk.x_start, chunk.x_start + chunk.width):
			var idx := row_base + x
			var water_here := float(w_curr[idx])
			var surface_here := float(h_curr[idx]) + water_here
			var lidx := ly * r_w + (x - rx0)

			# --- 1. outflow ---
			var flow_out := 0.0
			if best_neighbor[lidx] != -1 and water_here > 0.0:
				flow_out = (surface_here - float(min_surface[lidx])) * 0.5
				flow_out = clampf(flow_out, 0.0, water_here)

			# --- 2. inflow ---
			var flow_in := 0.0
			var y0 := maxi(ry0, y - 1)
			var y1 := mini(ry1, y + 1)
			var x0 := maxi(rx0, x - 1)
			var x1 := mini(rx1, x + 1)
			for ny in range(y0, y1 + 1):
				var n_row := ny * map_width
				var n_ly := ny - ry0
				for nx in range(x0, x1 + 1):
					if nx == x and ny == y:
						continue
					var n_idx := n_row + nx
					var n_lidx := n_ly * r_w + (nx - rx0)
					if best_neighbor[n_lidx] != idx:
						continue

					var n_water := float(w_curr[n_idx])
					if n_water <= 0.0:
						continue
					var n_surface := float(h_curr[n_idx]) + n_water
					var n_out := (n_surface - float(min_surface[n_lidx])) * 0.5
					n_out = clampf(n_out, 0.0, n_water)
					flow_in += n_out

			# --- 3. update ---
			w_next[idx] = clampf(water_here - flow_out + flow_in, 0.0, 512.0)
			# Map border is treated as an infinite ocean at sea level (512).
			# If a border tile is below sea level, force its water depth so that
			# (height + water) stays at 512, preventing the edge ocean from draining.
			if x == 0 or x == map_width - 1 or y == 0 or y == map_height - 1:
				var h_border := float(h_curr[idx])
				if h_border < 512.0:
					w_next[idx] = 512.0 - h_border
			if flow_in > 0.0:
				m_next[idx] = clampf(float(m_curr[idx]) + 0.001, 0.0, 1.0)
			else:
				m_next[idx] = m_curr[idx]
			
			# Optional: keep vegetation tied to moisture so it evolves without extra passes.
			# data_next.vegetation_map[idx] = float(m_next[idx]) * 0.7
				
func _is_best_neighbor(src_x, src_y, target_x, target_y) -> bool:
	var h_curr = data_current.height_map
	var w_curr = data_current.water_depth_map
	var src_idx = data_current.get_idx(src_x, src_y)
	var src_h = h_curr[src_idx] + w_curr[src_idx]
	
	var min_h = src_h
	var best_x = -1
	var best_y = -1
	
	for dy in range(-1, 2):
		for dx in range(-1, 2):
			if dx == 0 and dy == 0: continue
			var nx = src_x + dx
			var ny = src_y + dy
			var n_idx = data_current.get_idx(nx, ny)
			if n_idx == -1: continue
			
			var nh = h_curr[n_idx] + w_curr[n_idx]
			if nh < min_h:
				min_h = nh
				best_x = nx
				best_y = ny
	
	return best_x == target_x and best_y == target_y
	
func _get_min_neighbor_height(x, y) -> float:
	var h_curr = data_current.height_map
	var w_curr = data_current.water_depth_map
	var idx = data_current.get_idx(x,y)
	var min_h = h_curr[idx] + w_curr[idx]
	for dy in range(-1, 2):
		for dx in range(-1, 2):
			if dx == 0 and dy == 0: continue
			var n_idx = data_current.get_idx(x + dx, y + dy)
			if n_idx != -1:
				min_h = min(min_h, h_curr[n_idx] + w_curr[n_idx])
	return min_h
	
func _get_next_available_chunk() -> Chunk:
	mutex.lock()
	if next_chunk_idx < chunks.size():
		var c: Chunk = chunks[next_chunk_idx]
		next_chunk_idx += 1
		c.is_processed = true
		mutex.unlock()
		return c
	mutex.unlock()
	return null
	
func _setup_ui():
	var canvas_layer = CanvasLayer.new()
	add_child(canvas_layer)
	info_panel = info_panel_scene.instantiate()
	canvas_layer.add_child(info_panel)
	info_panel.hide()
	
func swap_buffers():
	var temp = data_current
	data_current = data_next
	data_next = temp
	
	is_updating = false
	update_render_texture()
	# Make the next auto water update eligible immediately.
	water_flow_elapsed_sec = water_flow_update_interval_sec
	# Keep eco updates decoupled; don't auto-trigger immediately.
	
func _on_hovered_tile_changed(coords: Vector2i):
	if Input.is_key_pressed(KEY_ALT):
		_refresh_info_panel(coords)
		
func _refresh_info_panel(coords: Vector2i):
	var idx = data_current.get_idx(coords.x, coords.y)
	if idx != -1:
		var stats = {
			"biome": BiomeLibrary.get_biome_name(data_current.height_map[idx], data_current.moisture_map[idx]),
			"position": str(coords),
			"height": data_current.height_map[idx],
			"moisture": data_current.moisture_map[idx],
			"vapor": data_current.vapor_map[idx],
			"salinity": data_current.salinity_map[idx],
			"fertility": data_current.fertility_map[idx],
			"temperature": data_current.temperature_map[idx],
			"vegetation": data_current.vegetation_map[idx],
			"water_depth": data_current.water_depth_map[idx],
			"snow_depth": data_current.snow_depth_map[idx],
			"magma_depth": data_current.magma_depth_map[idx]
		}
		info_panel.update_info(stats)
		info_panel.show()
		update_panel_position(coords)
		last_highlight_idx = idx
	else:
		info_panel.hide()
	
func _process(delta: float) -> void:
	if not is_updating:
		if need_regenerate:
			generate_map()
			need_regenerate = false
		if need_thermally_erode:
			apply_thermal_erosion()
			need_thermally_erode = false
		if need_drop_water:
			simulate_rain()
			need_drop_water = false
		# Eco updates (water cycle + vegetation)
		if eco_update_interval_sec <= 0.0:
			_run_eco_tick_full()
		else:
			eco_elapsed_sec += delta
			if eco_elapsed_sec >= eco_update_interval_sec:
				eco_elapsed_sec = 0.0
				_run_eco_tick_full()

		# Water flow updates
		if water_flow_update_interval_sec <= 0.0:
			_prepare_new_cycle()
			return
		water_flow_elapsed_sec += delta
		if water_flow_elapsed_sec >= water_flow_update_interval_sec:
			water_flow_elapsed_sec = 0.0
			_prepare_new_cycle()
		return
		
	if chunks_completed >= chunks.size():
		swap_buffers()
		return
	
	handle_info_display()
	
func _prepare_new_cycle():
	# Prepare next buffer so that fields not written by the simulation this cycle
	# keep consistent values after swap. Otherwise, after the first swap the
	# height/vegetation/etc. may revert to old (often zero) data.
	data_next.copy_from(data_current)
	# Ensure the simulation writes a fresh next-frame result for these.
	# (If a chunk ever gets skipped due to logic errors, this avoids stale water.)
	data_next.water_depth_map.fill(0.0)
	# Keep this cycle's vapor stable (eco tick handles it).
	# Not clearing here avoids needless work.
	# Eco tick writes these; water flow cycle keeps them as-is.
	next_chunk_idx = 0
	is_updating = true
	chunks_completed = 0
	for c in chunks: 
		c.is_processed = false
	semaphore.post(active_threads.size())
	
func handle_info_display():
	if Input.is_key_pressed(KEY_ALT):
		var coords = tile_cursor.highlight_tile
		var idx = data_current.get_idx(coords.x, coords.y)
		if idx != -1:
			if idx != last_highlight_idx:
				var stats = {
					"biome": BiomeLibrary.get_biome_name(data_current.height_map[idx], data_current.moisture_map[idx]),
					"position": str(coords),
					"height": data_current.height_map[idx],
					"moisture": data_current.moisture_map[idx],
					"vapor": data_current.vapor_map[idx],
					"salinity": data_current.salinity_map[idx],
					"fertility": data_current.fertility_map[idx],
					"temperature": data_current.temperature_map[idx],
					"vegetation": data_current.vegetation_map[idx],
					"water_depth": data_current.water_depth_map[idx],
					"snow_depth": data_current.snow_depth_map[idx],
					"magma_depth": data_current.magma_depth_map[idx]
				}
				info_panel.update_info(stats)
				update_panel_position(coords)
				info_panel.show()
				last_highlight_idx = idx
		else:
			info_panel.hide()
	else:
		info_panel.hide()
		
func update_panel_position(coords: Vector2i):	
	var screen_pos = get_viewport().get_canvas_transform() * Vector2(coords * tile_size)
	var target_pos = screen_pos + Vector2(tile_size, -info_panel.size.y - 10)
	var view_size = get_viewport_rect().size
	if target_pos.x + info_panel.size.x > view_size.x:
		target_pos.x = screen_pos.x - info_panel.size.x - 10
	if target_pos.y < 0:
		target_pos.y = screen_pos.y + tile_size + 10
	target_pos.x = clamp(target_pos.x, 0, view_size.x - info_panel.size.x)
	target_pos.y = clamp(target_pos.y, 0, view_size.y - info_panel.size.y)
	info_panel.position = target_pos
	
func generate_map():
	noise.seed = randi()
	data_next.water_depth_map.fill(0.0)
	data_next.snow_depth_map.fill(0.0)
	data_next.magma_depth_map.fill(0.0)
	data_next.vapor_map.fill(0.0)
	# Reset other maps to avoid stale values from previous simulations.
	data_next.salinity_map.fill(0.0)
	data_next.fertility_map.fill(0.0)
	data_next.temperature_map.fill(0.0)
	for fluc_n in fluc_noise_list:
		fluc_n.seed = noise.seed
	var center = Vector2(map_width, map_height) / 2.0
	var max_dist_squared = center.length() ** 2
	for y in range(map_height):
		for x in range(map_width):
			var idx = y * map_width + x
			var dist_squared_ratio = Vector2(x, y).distance_squared_to(center) / max_dist_squared
			var n_val = remap(noise.get_noise_2d(x, y), -1, 1, 0, 2048)
			
			var fluc_val = 0.0
			var fluc_infl = 800.0
			for fluc_n in fluc_noise_list:
				fluc_val += fluc_n.get_noise_2d(x, y) * fluc_infl
				fluc_infl /= 2.0

			var h = clampf(n_val - dist_squared_ratio * 1800.0 + fluc_val, 0, 2048)
			data_next.height_map[idx] = h
			data_next.moisture_map[idx] = clampf((1.0 - (h / 2048.0)) ** 2, 0, 1)
			data_next.vegetation_map[idx] = data_next.moisture_map[idx] * 0.7
			data_next.vapor_map[idx] = 0.2 + (randf() - 0.5) * 0.1
			if h < 512.0:
				data_next.water_depth_map[idx] = 512.0 - h
			
	swap_buffers()
			
func _draw() -> void:
	pass
			

func apply_thermal_erosion():
	for y in range(map_height):
		for x in range(map_width):
			var idx = y * map_width + x
			var h = data_current.height_map[idx]
			if h <= 512: continue
			
			for dx in range(-1, 2):
				for dy in range(-1, 2):
					if dx == 0 and dy == 0: continue
					var n_idx = data_current.get_idx(x + dx, y + dy)
					if n_idx == -1: continue
					
					var n_h = data_current.height_map[n_idx]
					var diff = h - n_h
					if diff > erosion_threshold:
						var erosion_rate_final = erosion_rate
						if dx != 0 and dy != 0:
							erosion_rate_final = erosion_rate_corner
						var amount = diff * erosion_rate_final
						data_next.height_map[idx] -= amount
						data_next.height_map[n_idx] += amount
	swap_buffers()
	
func simulate_rain(drops: int = 500):
	for d in range(drops):
		var x = randi() % map_width
		var y = randi() % map_height
		
		var curr_x = x
		var curr_y = y
		
		# 雨滴寿命：它能滑行多少格
		for step in range(max_drop_lifespan):
			var idx = data_current.get_idx(curr_x, curr_y)
			var h = data_current.height_map[idx]
			
			# 寻找周围最低的点
			var best_nx = curr_x
			var best_ny = curr_y
			var min_h = h
			
			for dx in range(-1, 2):
				for dy in range(-1, 2):
					var ni = data_current.get_idx(curr_x + dx, curr_y + dy)
					if ni == -1: continue
					if data_next.height_map[ni] < min_h:
						min_h = data_next.height_map[ni]
						best_nx = curr_x + dx
						best_ny = curr_y + dy
			
			var erosion_rate_final = river_erosion_rate
			# 如果已经在局部最低点（坑底），雨滴停止移动并变成积水
			if best_nx == curr_x and best_ny == curr_y:
				data_next.water_depth_map[idx] += randf() * 20
				data_next.moisture_map[idx] = clampf(data_next.moisture_map[idx] + 0.001, 0.0, 1.0)
				break
			
			# 对角，应用不同侵蚀率
			if best_nx != curr_x and best_ny != curr_y:
				erosion_rate_final = river_erosion_rate_corner
			
			# 移动过程中的侵蚀
			var n_idx = data_current.get_idx(best_nx, best_ny)
			# 剥蚀当前点，填充到低点（河流侵蚀感）
			data_next.height_map[idx] -= erosion_rate_final
			data_next.height_map[n_idx] += erosion_rate_final * 0.15 # 沉淀一小部分
			
			# 增加沿途湿度和植被肥力潜力
			data_next.moisture_map[n_idx] = clampf(data_next.moisture_map[n_idx] + 0.002, 0.0, 1.0)
			data_next.vegetation_map[n_idx] = clampf(data_next.vegetation_map[n_idx] + 0.003, 0.0, 1.0)
			
			curr_x = best_nx
			curr_y = best_ny
			
			# 如果流进了海里 (海平面 512)
			if min_h < 512:
				break
			
	swap_buffers()


func _run_eco_tick_full() -> void:
	# Eco tick runs on main thread for now.
	# Stage A: local tile processes (no cross-tile writes).
	# Stage B: vapor transport (wind) + orographic rain.
	# Stage C: condensation (threshold rain) + clamps + border ocean.
	var h := data_current.height_map
	var w0 := data_current.water_depth_map
	var m0 := data_current.moisture_map
	var v0 := data_current.vegetation_map
	var vapor0 := data_current.vapor_map

	var size := map_width * map_height
	if _eco_w1.size() != size:
		_eco_w1.resize(size)
		_eco_m1.resize(size)
		_eco_v1.resize(size)
		_eco_vapor1.resize(size)
		_eco_vapor2.resize(size)
		_eco_vapor3.resize(size)

	# Stage A
	for idx in range(size):
		var height_here := float(h[idx])
		var water_here := float(w0[idx])
		var moisture_here := float(m0[idx])
		var veg_here := float(v0[idx])
		var vapor_here := float(vapor0[idx])

		if water_here > 0.01:
			var evap := evap_rate * (1.0 - 0.5 * veg_here)
			evap = minf(evap, water_here)
			water_here -= evap
			vapor_here = clampf(vapor_here + evap * 0.02, 0.0, 1.0)

		var soil_cap := soil_capacity_base + soil_capacity_veg_bonus * veg_here
		soil_cap = clampf(soil_cap, 0.05, 1.0)
		if water_here > 0.01 and moisture_here < soil_cap:
			var infil := minf(infiltration_rate, water_here)
			var need := soil_cap - moisture_here
			infil = minf(infil, need * 20.0)
			water_here -= infil
			moisture_here = clampf(moisture_here + infil * 0.05, 0.0, 1.0)

		if moisture_here > 0.0:
			var dry := soil_dry_rate * (1.0 - 0.3 * veg_here)
			dry = minf(dry, moisture_here)
			moisture_here -= dry
			vapor_here = clampf(vapor_here + dry * 0.5, 0.0, 1.0)

		var moist_factor := 1.0 - absf(moisture_here - veg_opt_moisture) / maxf(0.001, veg_moisture_width)
		moist_factor = clampf(moist_factor, 0.0, 1.0)
		var grow := veg_growth_rate * moist_factor
		var decay := veg_decay_rate * (1.0 - moist_factor)
		if water_here > 1.0:
			decay += veg_waterlog_decay * clampf(water_here / 30.0, 0.0, 1.0)
		veg_here = clampf(veg_here + grow - decay, 0.0, 1.0)
		moisture_here = clampf(moisture_here + veg_here * 0.0005, 0.0, 1.0)

		_eco_w1[idx] = water_here
		_eco_m1[idx] = moisture_here
		_eco_v1[idx] = veg_here
		_eco_vapor1[idx] = vapor_here

	# Stage B: advection + orographic rain
	_eco_vapor2.fill(0.0)
	# Work on a local copy for water so we don't affect Stage C reads.
	var w2 := PackedFloat32Array(_eco_w1)
	var dir := wind_dir
	if dir.length_squared() > 0.0001:
		dir = dir.normalized()
	else:
		dir = Vector2.ZERO

	var ax := absf(dir.x)
	var ay := absf(dir.y)
	var sx := 0
	var sy := 0
	if dir.x > 0.05:
		sx = 1
	elif dir.x < -0.05:
		sx = -1
	if dir.y > 0.05:
		sy = 1
	elif dir.y < -0.05:
		sy = -1

	var diag_w := minf(ax, ay)
	var x_w := maxf(0.0, ax - diag_w)
	var y_w := maxf(0.0, ay - diag_w)
	var w_sum := x_w + y_w + diag_w
	if w_sum <= 0.0001:
		x_w = 1.0
		y_w = 0.0
		diag_w = 0.0
		w_sum = 1.0
	x_w /= w_sum
	y_w /= w_sum
	diag_w /= w_sum

	var adv := clampf(vapor_advect_rate, 0.0, 1.0)
	for idx in range(size):
		var v_here := float(_eco_vapor1[idx])
		var send := v_here * adv
		var remain := v_here - send
		_eco_vapor2[idx] += remain
		if send <= 0.0 or (sx == 0 and sy == 0):
			continue

		var x := idx % map_width
		var y := idx / map_width
		var h_from := float(h[idx])

		# x neighbor
		if sx != 0 and x_w > 0.0:
			var tx := x + sx
			var ty := y
			if tx >= 0 and tx < map_width:
				var to_idx := ty * map_width + tx
				var lift := maxf(0.0, float(h[to_idx]) - h_from)
				var cond := send * x_w * orographic_condense_strength * (lift / maxf(1.0, orographic_height_scale))
				cond = clampf(cond, 0.0, send * x_w)
				if cond > 0.0 and float(h[to_idx]) >= 512.0:
					w2[to_idx] = clampf(float(w2[to_idx]) + cond * orographic_rain_amount, 0.0, 512.0)
				_eco_vapor2[to_idx] += (send * x_w - cond)
			else:
				_eco_vapor2[idx] += send * x_w

		# y neighbor
		if sy != 0 and y_w > 0.0:
			var tx := x
			var ty := y + sy
			if ty >= 0 and ty < map_height:
				var to_idx := ty * map_width + tx
				var lift := maxf(0.0, float(h[to_idx]) - h_from)
				var cond := send * y_w * orographic_condense_strength * (lift / maxf(1.0, orographic_height_scale))
				cond = clampf(cond, 0.0, send * y_w)
				if cond > 0.0 and float(h[to_idx]) >= 512.0:
					w2[to_idx] = clampf(float(w2[to_idx]) + cond * orographic_rain_amount, 0.0, 512.0)
				_eco_vapor2[to_idx] += (send * y_w - cond)
			else:
				_eco_vapor2[idx] += send * y_w

		# diagonal neighbor
		if sx != 0 and sy != 0 and diag_w > 0.0:
			var tx := x + sx
			var ty := y + sy
			if tx >= 0 and tx < map_width and ty >= 0 and ty < map_height:
				var to_idx := ty * map_width + tx
				var lift := maxf(0.0, float(h[to_idx]) - h_from)
				var cond := send * diag_w * orographic_condense_strength * (lift / maxf(1.0, orographic_height_scale))
				cond = clampf(cond, 0.0, send * diag_w)
				if cond > 0.0 and float(h[to_idx]) >= 512.0:
					w2[to_idx] = clampf(float(w2[to_idx]) + cond * orographic_rain_amount, 0.0, 512.0)
				_eco_vapor2[to_idx] += (send * diag_w - cond)
			else:
				_eco_vapor2[idx] += send * diag_w

	# diffusion
	var d := clampf(vapor_diffuse_rate, 0.0, 1.0)
	if d > 0.0:
		for idx in range(size):
			var x := idx % map_width
			var y := idx / map_width
			var acc := float(_eco_vapor2[idx])
			var cnt := 1.0
			if x > 0:
				acc += float(_eco_vapor2[idx - 1]); cnt += 1.0
			if x < map_width - 1:
				acc += float(_eco_vapor2[idx + 1]); cnt += 1.0
			if y > 0:
				acc += float(_eco_vapor2[idx - map_width]); cnt += 1.0
			if y < map_height - 1:
				acc += float(_eco_vapor2[idx + map_width]); cnt += 1.0
			var avg := acc / cnt
			_eco_vapor3[idx] = lerpf(float(_eco_vapor2[idx]), avg, d)
		# swap (Packed arrays are copy-on-write, swapping is safe)
		var tmp := _eco_vapor2
		_eco_vapor2 = _eco_vapor3
		_eco_vapor3 = tmp

	# Stage C
	for idx in range(size):
		var height_here := float(h[idx])
		var vapor_here := clampf(float(_eco_vapor2[idx]), 0.0, 1.0)
		var water_here := clampf(float(w2[idx]), 0.0, 512.0)
		if vapor_here >= rain_threshold_vapor and height_here >= 512.0:
			var rain := (vapor_here - rain_threshold_vapor) * condense_rate * rain_amount
			rain = clampf(rain, 0.0, vapor_here * 10.0)
			vapor_here = clampf(vapor_here - rain * 0.02, 0.0, 1.0)
			water_here = clampf(water_here + rain, 0.0, 512.0)

		var x := idx % map_width
		var y := idx / map_width
		if x == 0 or x == map_width - 1 or y == 0 or y == map_height - 1:
			if height_here < 512.0:
				water_here = 512.0 - height_here

		data_current.water_depth_map[idx] = water_here
		data_current.moisture_map[idx] = _eco_m1[idx]
		data_current.vegetation_map[idx] = _eco_v1[idx]
		data_current.vapor_map[idx] = vapor_here

	update_render_texture()
	
func update_render_texture():
	var img = Image.create(map_width, map_height, false, Image.FORMAT_RGBAF)
	var data_buffer = PackedFloat32Array()
	data_buffer.resize(map_width * map_height * 4)
	
	var fast_height_map := data_current.height_map
	var fast_moisture_map := data_current.moisture_map
	var fast_vegetation_map := data_current.vegetation_map
	var fast_water_depth_map := data_current.water_depth_map
	var fast_snow_depth_map := data_current.snow_depth_map
	var fast_magma_depth_map := data_current.magma_depth_map
	
	for i in range(map_width * map_height):
		var base_idx = i * 4
		
		# R: 基础高度 h 
		data_buffer[base_idx + 0] = fast_height_map[i] / 2048.0
		# G: 土壤水分 m (影响基础土地的明暗) 
		data_buffer[base_idx + 1] = fast_moisture_map[i]
		# B: 植被 v 
		data_buffer[base_idx + 2] = fast_vegetation_map[i]
		
		# A: 互斥覆盖层通道 (c)
		var cover_val = 0.0
		var w_d = fast_water_depth_map[i]
		var s_d = fast_snow_depth_map[i]
		var m_d = fast_magma_depth_map[i]
		
		if m_d > 0.01:
			# 0.7 ~ 1.0 为岩浆区
			cover_val = 0.7 + clampf(m_d / 100.0, 0.0, 0.3)
		elif s_d > 0.01:
			# 0.4 ~ 0.7 为积雪区
			cover_val = 0.4 + clampf(s_d / 5.0, 0.0, 0.3)
		elif w_d > 0.01:
			# 0.0 ~ 0.4 为水体区 (0.001 是极浅水)
			cover_val = clampf(w_d / 400.0, 0.0, 0.4)
			
		data_buffer[base_idx + 3] = cover_val
	img.set_data(map_width, map_height, false, Image.FORMAT_RGBAF, data_buffer.to_byte_array())
	
	var tex = ImageTexture.create_from_image(img)
	map_display_node.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	map_display_node.material.set_shader_parameter("world_data", tex)
	
#func wait_for_chunks_processed() -> bool:
	#while true:
		#for c in chunks:
			#if !c.is_processed:
				#break
		#OS.delay_msec(100)

func _input(event: InputEvent) -> void:
	if event is InputEventKey:
		match event.keycode:
			KEY_R:
				if event.pressed and not event.is_echo():
					need_thermally_erode = true
			KEY_G:
				if event.pressed and not event.is_echo():
					need_regenerate = true
			KEY_F:
				if event.pressed and not event.is_echo():
					need_drop_water = true
			KEY_ALT:
				if event.pressed:
					_refresh_info_panel(tile_cursor.highlight_tile)
				else:
					info_panel.hide()
			
			
func _exit_tree() -> void:
	_is_shutting_down = true
	semaphore.post(active_threads.size())
	for t in active_threads:
		if t.is_started():
			t.wait_to_finish()
			
	print("All worker threads have been shut down gracefully.")
