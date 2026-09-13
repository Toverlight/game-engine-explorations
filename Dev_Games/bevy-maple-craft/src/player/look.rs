use bevy::prelude::*;

use crate::{input::prelude::*, ui::prelude::UiState};

use super::components::*;

pub fn apply_look(
    tuning: Res<PlayerTuning>,
    look: Res<LookState>,
    ui_state: Res<UiState>,
    mut players: Query<(&mut Transform, &mut LookAngles, &Children), With<Player>>,
    mut cameras: Query<&mut Transform, (With<PlayerCamera>, Without<Player>)>,
) {
    if look.mouse_delta == Vec2::ZERO || ui_state.cursor_released() {
        return;
    }

    for (mut body_transform, mut angles, children) in &mut players {
        angles.yaw -= look.mouse_delta.x * tuning.mouse_sensitivity;
        angles.pitch -= look.mouse_delta.y * tuning.mouse_sensitivity;
        angles.pitch = angles.pitch.clamp(-tuning.pitch_limit, tuning.pitch_limit);

        body_transform.rotation = Quat::from_rotation_y(angles.yaw);
        for &child in children {
            if let Ok(mut cam_transform) = cameras.get_mut(child) {
                cam_transform.rotation = Quat::from_rotation_x(angles.pitch);
            }
        }
    }
}
