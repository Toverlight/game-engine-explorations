use bevy::prelude::*;
use bevy_rapier3d::{
    plugin::*,
    render::{DebugRenderContext, RapierDebugRenderPlugin},
};

use crate::prelude::AppState;

mod app_state;
mod debug;
mod input;
mod player;
mod ui;
mod world;

pub mod prelude;

pub struct MapleCraftPlugin;

impl Plugin for MapleCraftPlugin {
    fn build(&self, app: &mut App) {
        app.init_state::<AppState>()
            .insert_resource(Time::<Virtual>::default())
            .insert_resource(Time::<bevy::time::Fixed>::from_hz(64.0))
            .add_plugins(RapierPhysicsPlugin::<()>::default().in_fixed_schedule())
            .insert_resource(TimestepMode::Fixed {
                dt: 1.0 / 64.0,
                substeps: 1,
            })
            .add_plugins(RapierDebugRenderPlugin::default().disabled())
            .insert_resource(DebugRenderContext {
                enabled: false,
                ..default()
            })
            .add_plugins((
                world::WorldPlugin,
                player::PlayerPlugin,
                input::InputPlugin,
                debug::DebugPlugin,
                ui::UiPlugin,
            ));
    }
}
