extends Area2D
class_name Food
@export var heat: float = 50.0

func get_eaten() -> void:
	queue_free()
