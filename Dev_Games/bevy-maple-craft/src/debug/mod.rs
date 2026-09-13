use bevy::prelude::*;

use crate::debug::resources::*;

mod components;
mod map;
mod overlay;
mod resources;

pub struct DebugPlugin;

impl Plugin for DebugPlugin {
    fn build(&self, app: &mut App) {
        app.init_resource::<DebugMapState>()
            .add_systems(Startup, (overlay::spawn_overlay, map::setup_debug_map))
            .add_systems(
                PreUpdate,
                (
                    overlay::toggle_debug_text,
                    overlay::toggle_physics_debug,
                    map::debug_map_input,
                ),
            )
            .add_systems(
                Update,
                (
                    overlay::update_overlay,
                    map::update_debug_map_visibility,
                    map::update_debug_map_image,
                ),
            );
    }
}
