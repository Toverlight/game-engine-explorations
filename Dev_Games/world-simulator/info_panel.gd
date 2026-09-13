extends PanelContainer

@onready var title_label = $VBoxContainer/Title
@onready var fields_container = $VBoxContainer/Fields

#func _ready() -> void:
	#title_label.add_theme_font_override()

func update_info(tile_data: Dictionary):
	title_label.text = "[ %s ]" % tile_data.get("biome", "Unknown")
	
	for child in fields_container.get_children():
		fields_container.remove_child(child)
		child.queue_free()
		
	for key in tile_data.keys():
		if key == "biome": continue
		
		var label = Label.new()
		
		var val = tile_data[key]
		var val_str = "%.2f" % val if val is float else str(val)
			
		label.text = "%s: %s" % [key.capitalize(), val_str]
		label.add_theme_font_size_override("font_size", 14)
		fields_container.add_child(label)
		
	reset_size()
		
func _add_fields(fields: Array[Label]):
	for field in fields:
		fields_container.add_child(field)
