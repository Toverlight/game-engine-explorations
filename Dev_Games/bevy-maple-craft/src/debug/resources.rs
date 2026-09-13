use bevy::prelude::*;

#[derive(Resource)]
pub struct DebugMapState {
    pub zoom: f32, // how many pixels per block
    pub pan: Vec2, // the world xz corresponding to map center
    pub resolution: UVec2,
    pub dirty: bool,
}

impl Default for DebugMapState {
    fn default() -> Self {
        Self {
            zoom: 4.0,
            pan: Vec2::ZERO,
            resolution: UVec2::new(256, 256),
            dirty: true,
        }
    }
}
