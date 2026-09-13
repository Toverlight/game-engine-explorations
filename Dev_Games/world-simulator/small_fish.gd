extends Area2D
class_name SmallFish
signal eat

enum State { WANDER, SEEK_FOOD, FLEE }
var current_state: State = State.WANDER

@onready var perception: Area2D = $Perception
@onready var states_timer: Timer = $StatesTimer
@onready var state_label: Label = $StateLabel
@onready var flee_timer: Timer = $FleeTimer

@export var label_offset: Vector2 = Vector2(0, -80)

var current_speed: float = 200.0
@export var max_speed: float = 550.0
@export var friction: float = 0.998

var velocity: Vector2 = Vector2.UP
var world_size: Vector2 = Vector2.ZERO
@export var burst_speed_boundary: float = 120.0
@export var desired_velocity: Vector2 = Vector2.UP

# Heat Management
# -- Hunger: volatile
@export var max_heat: float = 100.0
@export var heat_decrease_speed: float = 5.0
@export var food_finding_heat_boundary: float = 60.0
var heat: float = 100.0
# -- Nutrition: influencing survival
@export var steady_heat: float = 150.0
@export var steady_heat_top: float = 180.0
@export var fatal_boundary: float = 100.0

# Boids Algorithm
var separation_force: Vector2 = Vector2.ZERO
var alignment_force: Vector2 = Vector2.ZERO
var cohesion_force: Vector2 = Vector2.ZERO
@export var separation_weight: float = 18.0
@export var alignment_weight: float = 0.4
@export var cohesion_weight: float = 160

# Food Seeking
var target_food: Food = null

# Fleeing
var nearest_predator: BigFish = null

# Collision Compensation
@export var min_dist: float = 32.0

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	velocity = Vector2.UP.rotated(randf() * TAU)
	desired_velocity = Vector2.UP.rotated(randf() * TAU)
	state_label.text = State.keys()[State.WANDER]
	start_states_timer()

func start_states_timer() -> void:
	states_timer.wait_time = randf_range(0.1, 0.3)
	states_timer.start()
	
func start_flee_timer():
	flee_timer.wait_time = randf_range(2.5, 6.0)
	flee_timer.start()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	match current_state:
		State.WANDER:
			apply_wander_logic(delta)
		State.SEEK_FOOD:
			apply_seek_logic(delta)
		State.FLEE:
			apply_flee_logic(delta)
			
	try_burst_speed()
	current_speed *= friction
	move_fish(delta)
	heat -= heat_decrease_speed * delta
	if heat < 0:
		heat = 0.0
		
	state_label.global_position = global_position + label_offset
	state_label.global_position.x -= state_label.size.x / 2
	
func check_state_transitions():
	# Survival's PRIMARY
	nearest_predator = check_predator_nearby()
	match current_state:
		State.FLEE: return
	if is_instance_valid(nearest_predator):
		current_state = State.FLEE
		start_flee_timer()
		return
	# Heat is MAINTAINANCE of life activities
	if heat < food_finding_heat_boundary:
		if not is_instance_valid(target_food):
			target_food = find_closest_food()
			
	if is_instance_valid(target_food):
		current_state = State.SEEK_FOOD
		return
	# SmallFish's SOCIAL creature
	current_state = State.WANDER
	
func update_state_label():
	var state_name = State.keys()[current_state]
	var heat_info = "Heat %d" % heat
	state_label.text = "[%s]\n%s" % [state_name, heat_info]
	
	match current_state:
		State.WANDER: state_label.modulate = Color.WHITE
		State.SEEK_FOOD: state_label.modulate = Color.YELLOW
		State.FLEE: state_label.modulate = Color.RED
	
func find_closest_food() -> Food:
	var neighbors = perception.get_overlapping_areas()
	if neighbors.size() == 0:
		return null
	
	var min_dist_squared = INF
	var food = null
	
	for neighbor in neighbors:
		if neighbor is Food:
			var dist = global_position.distance_squared_to(neighbor.global_position)
			if dist < min_dist_squared:
				min_dist_squared = dist
				food = neighbor
				
	return food
	

func check_predator_nearby() -> BigFish:
	var neighbors = perception.get_overlapping_areas()
	if neighbors.size() == 0:
		return null
		
	var min_dist_squared = INF
	var predator = null
	
	for neighbor in neighbors:
		if neighbor is BigFish:
			var dist_squared = global_position.distance_squared_to(neighbor.global_position)
			if dist_squared < min_dist_squared:
				min_dist_squared = dist_squared
				predator = neighbor
			
	return predator
	
func try_burst_speed():
	match current_state:
		State.WANDER:
			if current_speed < burst_speed_boundary and randf() < 0.08:
				current_speed = randf_range(150, max_speed * 0.5)
		State.SEEK_FOOD:
			if current_speed < burst_speed_boundary * 1.2 and randf() < 0.16:
				current_speed = randf_range(180, max_speed * 0.75)
		State.FLEE:
			if current_speed < burst_speed_boundary * 1.5 and randf() < 0.4:
				current_speed = randf_range(400, max_speed)
	
func apply_wander_logic(delta: float):
	velocity += (separation_force * separation_weight + alignment_force * alignment_weight + cohesion_force * cohesion_weight) * delta
	velocity = velocity.normalized()
	
func move_fish(delta: float):
	var next_pos = position + velocity * current_speed * delta
	if next_pos.x < 0 or next_pos.x > world_size.x:
		velocity.x *= -1
		desired_velocity.x *= -1
		position.x = clampf(position.x, next_pos.x, world_size.x)
	if next_pos.y < 0 or next_pos.y > world_size.y:
		velocity.y *= -1
		desired_velocity.y *= -1
		position.y = clampf(position.y, next_pos.y, world_size.y)
	position += velocity * current_speed * delta
	rotation = velocity.angle()
	
func on_food_consumed(food: Food):
	heat += food.heat
	if heat > max_heat:
		heat = max_heat
		
func _on_states_timer_timeout() -> void:
	check_state_transitions()
	
	match current_state:
		State.WANDER:
			calc_boids()
		State.FLEE:
			evade_fluctuate()
	
	collision_compensation()
	start_states_timer()

func collision_compensation():
	var neighbors = get_overlapping_areas()
	if neighbors.size() == 0:
		return
		
	for neighbor in neighbors:
		if neighbor is not SmallFish:
			continue
		var diff = global_position - neighbor.global_position
		var diff_norm = diff.normalized()
		var dist = diff.length()
		
		if dist < min_dist:
			var push_depth = min_dist - dist
			var push_vector = diff_norm * (push_depth * 0.25)
			
			global_position += push_vector
			neighbor.global_position -= push_vector
	
	

func calc_boids():
	var neighbors = perception.get_overlapping_areas()
	
	var count = 0
	separation_force = Vector2.ZERO
	alignment_force = Vector2.ZERO
	cohesion_force = Vector2.ZERO
	
	for neighbor in neighbors:
		if neighbor == self or not neighbor is SmallFish:
			continue
		count += 1
		var diff = global_position - neighbor.global_position
		var diff_norm = diff.normalized()
		var dist = diff.length()
		
		separation_force += diff_norm / max(dist, 10.0)
		alignment_force += neighbor.velocity
		cohesion_force += neighbor.global_position
	
	if count > 0:
		alignment_force /= count
		var g_pos_diff = (cohesion_force / count) - global_position
		cohesion_force = g_pos_diff.normalized() / max(g_pos_diff.length(), 100.0)
	
func evade_fluctuate():
	if is_instance_valid(nearest_predator):
		desired_velocity = (global_position - nearest_predator.global_position).normalized()
		if randf() < 0.4:
			desired_velocity = desired_velocity.rotated(randf_range(-0.4, 0.4))
	else:
		if randf() < 0.2:
			desired_velocity = desired_velocity.rotated(randf_range(-0.3, 0.3))

func apply_seek_logic(delta: float):
	if is_instance_valid(target_food):
		desired_velocity = (target_food.global_position - global_position).normalized()
		velocity = velocity.lerp(desired_velocity, 6.0 * delta)
func apply_flee_logic(delta: float):
	velocity = velocity.lerp(desired_velocity, 10.0 * delta)

func _on_area_entered(area: Area2D) -> void:
	if area.is_in_group("foods"):
		if heat > food_finding_heat_boundary:
			#print("不吃")
			return
		#print("吃")
		eat.emit()
		area.get_eaten()
		on_food_consumed(area)

func get_eaten():
	queue_free()


func _on_msg_sync_timer_timeout() -> void:
	update_state_label()


func _on_flee_timer_timeout() -> void:
	current_state = State.WANDER
