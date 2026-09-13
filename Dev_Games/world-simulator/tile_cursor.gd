extends Node2D
class_name TileCursor
signal hovered_tile_changed(new_coords: Vector2i)

@export var tile_size: int = 40
var highlight_tile: Vector2i = Vector2i(-1, -1)

func _ready() -> void:
	queue_redraw()
	
func _draw() -> void:
	var rect = Rect2(0, 0, tile_size, tile_size)
	draw_rect(rect, Color.YELLOW, false, 2.0)

func _process(delta: float) -> void:
	var mouse_pos = get_global_mouse_position()
	
	var new_tile = Vector2i(
		floor(mouse_pos.x / tile_size),
		floor(mouse_pos.y / tile_size)
	)
	
	#if new_tile.x < 0 or new_tile.x > map_width or new_tile.y < 0 or new_tile.y > map_height:
	
	if new_tile != highlight_tile:
		highlight_tile = new_tile
		global_position = Vector2(new_tile.x * tile_size, new_tile.y * tile_size)
		#print...
		hovered_tile_changed.emit(highlight_tile)
