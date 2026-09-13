use bevy::prelude::*;

mod actions;
mod events;

pub mod prelude {
    pub use super::actions::*;
    pub use super::events::*;
}

#[derive(SystemSet, Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum InputSet {
    Gather,
    Apply,
}

pub struct InputPlugin;

impl Plugin for InputPlugin {
    fn build(&self, app: &mut App) {
        app.init_resource::<actions::LookState>()
            .init_resource::<actions::PlayerActions>()
            .init_resource::<actions::SprintDetector>()
            .init_resource::<actions::ToggleFlightDetector>()
            .configure_sets(PreUpdate, (InputSet::Gather, InputSet::Apply).chain())
            .add_systems(
                PreUpdate,
                (
                    actions::gather_look_input,
                    actions::gather_movement_actions,
                    actions::gather_jump_action,
                    actions::gather_sprint_action,
                    actions::gather_toggle_flight_action,
                    actions::gather_upward_action,
                    actions::gather_downward_action,
                )
                    .in_set(InputSet::Gather),
            )
            .add_systems(
                PreUpdate,
                (actions::apply_cursor_grab, actions::apply_pause).in_set(InputSet::Apply),
            );
    }
}
