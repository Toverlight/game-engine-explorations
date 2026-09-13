use bevy::prelude::*;

#[derive(EntityEvent)]
pub struct ToggleFlightEvent {
    pub entity: Entity,
}
