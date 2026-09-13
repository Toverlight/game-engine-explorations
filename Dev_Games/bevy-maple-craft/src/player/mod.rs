use bevy::prelude::*;
use bevy_rapier3d::plugin::PhysicsSet;

mod components;
mod look;
mod movement;
mod observers;
mod spawn;

pub mod prelude {
    pub use super::components::*;
}

#[derive(SystemSet, Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum PlayerSet {
    Spawn,
    Movement,
}

pub struct PlayerPlugin;

impl Plugin for PlayerPlugin {
    fn build(&self, app: &mut App) {
        app.init_resource::<components::PlayerTuning>()
            .add_observer(observers::toggle_flight_observer)
            .add_systems(Startup, spawn::spawn_player.in_set(PlayerSet::Spawn))
            .add_systems(
                FixedUpdate,
                (movement::simulate_movement, look::apply_look)
                    .in_set(PlayerSet::Movement)
                    .before(PhysicsSet::SyncBackend),
            );
    }
}
