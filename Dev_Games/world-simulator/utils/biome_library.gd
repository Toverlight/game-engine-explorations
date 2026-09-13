class_name BiomeLibrary

static func get_biome_name(h: float, m: float) -> String:
	if h < 512: return "Ocean"
	if h < 600: return "Beach"
	if h > 1600: return "Snowy Mountain"
	
	if m < 0.2: return "Desert"
	if m < 0.5: return "GrassLand"
	if m < 0.8: return "Forest"
	return "RainForest"
	
static func get_biome_color(h: float, m: float, flow: float) -> Color:
	if flow > 50.0: return Color(0.2, 0.4, 0.9)
	if flow > 10.0: return Color(0.3, 0.5, 0.8)
	
	if h < 512: return Color.DARK_BLUE
	if h < 600: return Color.BEIGE
	if h > 1600: return Color.WHITE
	
	if m < 0.2: return Color.PALE_GOLDENROD
	if m < 0.5: return Color.LIME_GREEN
	if m < 0.8: return Color.FOREST_GREEN
	return Color.DARK_GREEN
