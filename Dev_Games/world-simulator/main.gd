extends Node2D
@export var padding: float = 10.0
@export var small_fish_scene: PackedScene
@export var big_fish_scene: PackedScene
@export var food_scene: PackedScene
var screen_size = Vector2.ZERO
@export var world_size = Vector2(5000, 5000)

func _ready() -> void:
	screen_size = get_viewport().get_visible_rect().size
	# spawn small fishes
	for i in range(80):
		var small_fish = small_fish_scene.instantiate() as SmallFish
		add_child(small_fish)
		small_fish.z_index = 1
		small_fish.world_size = world_size
		small_fish.position = Vector2(randf_range(padding, screen_size.x - padding), randf_range(padding, screen_size.y - padding))
	# spawn big fishes
	for i in range(5):
		var big_fish = big_fish_scene.instantiate() as BigFish
		add_child(big_fish)
		big_fish.z_index = 2
		big_fish.world_size = world_size
		big_fish.position = Vector2(randf_range(world_size.x - screen_size.x + padding, world_size.x - padding), randf_range(world_size.y - screen_size.y + padding, world_size.y - padding))
	
func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
		spawn_food(get_global_mouse_position())
		
	
func spawn_food(pos: Vector2) -> void:
	var food = food_scene.instantiate() as Food
	add_child(food)
	food.position = pos
	
func _draw() -> void:
	draw_rect(Rect2(Vector2.ZERO, Vector2(5000, 5000)), Color.WHITE, false, 5.0)
	
func _on_report_timer_timeout() -> void:
	report()

func report() -> void:
	for fish in get_tree().get_nodes_in_group("fishes"):
		if fish is SmallFish:
			print("Heat: ", fish.heat)
			return
