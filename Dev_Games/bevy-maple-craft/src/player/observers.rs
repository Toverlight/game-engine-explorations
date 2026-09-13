use bevy::prelude::*;

use crate::{input::prelude::*, player::prelude::*};

pub fn toggle_flight_observer(
    ev: On<ToggleFlightEvent>,
    mut players: Query<&mut MovementMode, With<Player>>,
) {
    let Ok(mut movement_mode) = players.get_mut(ev.entity) else {
        return;
    };

    *movement_mode = match *movement_mode {
        MovementMode::Walking => MovementMode::Flying,
        MovementMode::Flying => MovementMode::Walking,
    };
}
