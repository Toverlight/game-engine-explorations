extends Area2D
class_name BigFish
signal eat
enum State { WANDER, PREY }
var current_state: State = State.WANDER

@onready var perception: Area2D = $Perception
@onready var states_timer: Timer = $StatesTimer
@onready var state_label: Label = $StateLabel

@export var label_offset: Vector2 = Vector2(0, -80)

var current_speed: float = 180.0
@export var max_speed: float = 450.0
@export var friction: float = 0.998

var velocity: Vector2 = Vector2.UP
var world_size: Vector2 = Vector2.ZERO
@export var burst_speed_boundary: float = 120.0
@export var max_steer: float = 0.3
var desired_velocity: Vector2 = Vector2.UP

# Heat Management
# -- Hunger: volatile
@export var max_heat: float = 400.0
@export var heat_decrease_speed: float = 10.0
@export var prey_finding_heat_boundary: float = 270.0
var heat: float = 400.0
# -- Nutrition: influencing survival
@export var steady_heat: float = 600.0
@export var steady_heat_top: float = 700.0
@export var fatal_boundary: float = 450.0

# Prey Seeking
var target_prey: SmallFish = null

# Collision Compensation
@export var min_dist: float = 78.0

func _ready() -> void:
	velocity = Vector2.UP.rotated(randf() * TAU)
	desired_velocity = Vector2.UP.rotated(randf() * TAU)
	start_states_timer()

func start_states_timer():
	states_timer.wait_time = randf_range(0.15, 0.25)
	states_timer.start()
	
func collision_compensation():
	var neighbors = get_overlapping_areas()
	if neighbors.size() == 0:
		return
		
	for neighbor in neighbors:
		if neighbor is not BigFish:
			continue
		var diff = global_position - neighbor.global_position
		var diff_norm = diff.normalized()
		var dist = diff.length()
		
		if dist < min_dist:
			var push_depth = min_dist - dist
			var push_vector = diff_norm * (push_depth * 0.25)
			
			global_position += push_vector
			neighbor.global_position -= push_vector
	

func _process(delta: float) -> void:
	match current_state:
		State.WANDER:
			apply_wander_logic(delta)
		State.PREY:
			apply_prey_logic(delta)
	try_burst_speed()
	current_speed *= friction
	move_fish(delta)
	heat -= heat_decrease_speed * delta
	if heat < 0:
		heat = 0.0
		
	state_label.global_position = global_position + label_offset
	state_label.global_position.x -= state_label.size.x / 2

func try_burst_speed():
	match current_state:
		State.WANDER:
			if current_speed < burst_speed_boundary and randf() < 0.04:
				current_speed = randf_range(150, max_speed)
		State.PREY:
			if current_speed < burst_speed_boundary * 1.2 and randf() < 0.2:
				current_speed = randf_range(200, max_speed)
			
func check_state_transitions():
	# PREYING is BigFish's unique heat source
	if heat < prey_finding_heat_boundary:
		if not is_instance_valid(target_prey):
			target_prey = seek_cloest_prey()
	
	if is_instance_valid(target_prey):
		current_state = State.PREY
		return
	
	# Wander without purpose
	current_state = State.WANDER

func apply_wander_logic(_delta: float):
	if randf() < 0.01 and velocity.angle_to(desired_velocity) < 0.0001:
		var steer = randf_range(-max_steer, max_steer)
		desired_velocity = velocity.rotated(steer)
		
func apply_prey_logic(_delta: float):
	if is_instance_valid(target_prey):
		desired_velocity = (target_prey.global_position - global_position).normalized()
		

func seek_cloest_prey() -> SmallFish:
	var neighbors = perception.get_overlapping_areas()
	if neighbors.size() == 0:
		return null
	
	var prey = null
	var min_dist_squared = INF
	
	for neighbor in neighbors:
		if neighbor is SmallFish:
			var dist_squared = global_position.distance_squared_to(neighbor.global_position)
			if dist_squared < min_dist_squared:
				min_dist_squared = dist_squared
				prey = neighbor
				
	return prey

func move_fish(delta: float):
	velocity = velocity.lerp(desired_velocity, 5.0 * delta)
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

func on_prey_consumed(small_fish: SmallFish):
	heat += small_fish.steady_heat # algorithm to be complicated
	if heat > max_heat:
		heat = max_heat
		
func update_state_label():
	var state_name = State.keys()[current_state]
	var heat_info = "Heat %d" % heat
	state_label.text = "[%s]\n%s" % [state_name, heat_info]
	
	match current_state:
		State.WANDER: state_label.modulate = Color.WHITE
		State.PREY: state_label.modulate = Color.CHARTREUSE

func _on_area_entered(area: Area2D) -> void:
	if area.is_in_group("small_fishes"):
		if heat > prey_finding_heat_boundary:
			#print("不吃")
			return
		#print("吃")
		eat.emit()
		area.get_eaten()
		on_prey_consumed(area)


func _on_msg_sync_timer_timeout() -> void:
	update_state_label()


func _on_states_timer_timeout() -> void:
	check_state_transitions()
	
	collision_compensation()
	start_states_timer()
