extends Camera2D

@export var move_speed: float = 600.0
@export var zoom_speed: float = 0.1
@export var min_zoom: float = 0.2
@export var max_zoom: float = 2.0
var screen_size: Vector2 = Vector2.ZERO

func _ready() -> void:
	screen_size = get_viewport_rect().size
	global_position = screen_size / 2


func _process(delta: float) -> void:
	var input_dir = Input.get_vector("move_left", "move_right", "move_up", "move_down")
	global_position += input_dir * move_speed * delta / zoom.x

func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_WHEEL_UP:
			set_zoom_level(zoom.x + zoom_speed)
		if event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			set_zoom_level(zoom.x - zoom_speed)
			
func set_zoom_level(value: float):
	var final_zoom = clampf(value, min_zoom, max_zoom)
	zoom = Vector2(final_zoom, final_zoom)
