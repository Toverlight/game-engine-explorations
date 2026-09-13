extends Resource
class_name WorldState

@export var width: int
@export var height: int

var height_map: PackedFloat32Array # 0.0 ~ 2048.0
var moisture_map: PackedFloat32Array # 0.0 ~ 1.0
var salinity_map: PackedFloat32Array # 0.0 ~ 1.0
var fertility_map: PackedFloat32Array # 0.0 ~ 1.0
var temperature_map: PackedFloat32Array # -150.0 ~ 10000.0
var vegetation_map: PackedFloat32Array # 0.0 ~ 1.0

var water_depth_map: PackedFloat32Array
var snow_depth_map: PackedFloat32Array
var magma_depth_map: PackedFloat32Array

var vapor_map: PackedFloat32Array # 0.0 ~ 1.0

func _init(w: int, h: int) -> void:
	width = w
	height = h
	
	var size = w * h
	height_map = PackedFloat32Array()
	height_map.resize(size)
	height_map.fill(0.0)
	moisture_map = PackedFloat32Array()
	moisture_map.resize(size)
	moisture_map.fill(0.0)
	salinity_map = PackedFloat32Array()
	salinity_map.resize(size)
	salinity_map.fill(0.0)
	fertility_map = PackedFloat32Array()
	fertility_map.resize(size)
	fertility_map.fill(0.0)
	temperature_map = PackedFloat32Array()
	temperature_map.resize(size)
	temperature_map.fill(0.0)
	vegetation_map = PackedFloat32Array()
	vegetation_map.resize(size)
	vegetation_map.fill(0.0)
	water_depth_map = PackedFloat32Array()
	water_depth_map.resize(size)
	water_depth_map.fill(0.0)
	snow_depth_map = PackedFloat32Array()
	snow_depth_map.resize(size)
	snow_depth_map.fill(0.0)
	magma_depth_map = PackedFloat32Array()
	magma_depth_map.resize(size)
	magma_depth_map.fill(0.0)
	vapor_map = PackedFloat32Array()
	vapor_map.resize(size)
	vapor_map.fill(0.0)


func copy_from(other: WorldState) -> void:
	# Deep copy all maps from another state into this one.
	# Assumes width/height match.
	width = other.width
	height = other.height
	var n := other.width * other.height
	if height_map.size() != n:
		height_map.resize(n)
		moisture_map.resize(n)
		salinity_map.resize(n)
		fertility_map.resize(n)
		temperature_map.resize(n)
		vegetation_map.resize(n)
		water_depth_map.resize(n)
		snow_depth_map.resize(n)
		magma_depth_map.resize(n)
		vapor_map.resize(n)

	for i in range(n):
		height_map[i] = other.height_map[i]
		moisture_map[i] = other.moisture_map[i]
		salinity_map[i] = other.salinity_map[i]
		fertility_map[i] = other.fertility_map[i]
		temperature_map[i] = other.temperature_map[i]
		vegetation_map[i] = other.vegetation_map[i]
		water_depth_map[i] = other.water_depth_map[i]
		snow_depth_map[i] = other.snow_depth_map[i]
		magma_depth_map[i] = other.magma_depth_map[i]
		vapor_map[i] = other.vapor_map[i]
	
func get_idx(x: int, y: int) -> int:
	if x < 0 or x >= width or y < 0 or y >= height: return -1
	return y * width + x
	
	
