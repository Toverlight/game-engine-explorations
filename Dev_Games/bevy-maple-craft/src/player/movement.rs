use bevy::prelude::*;
use bevy_rapier3d::prelude::*;

use crate::{input::prelude::*, ui::prelude::UiState};

use super::components::*;

pub fn simulate_movement(
    time: Res<Time<Fixed>>,
    tuning: Res<PlayerTuning>,
    ui_state: Res<UiState>,
    mut actions: ResMut<PlayerActions>,
    mut players: Query<
        (
            Entity,
            &Transform,
            &MovementMode,
            &mut PlayerKinematics,
            &mut KinematicCharacterController,
        ),
        With<Player>,
    >,
    controller_outputs: Query<&KinematicCharacterControllerOutput>,
) {
    if ui_state.game_paused() {
        return;
    }

    let dt = time.delta_secs();

    for (entity, transform, movement_mode, mut kin, mut controller) in &mut players {
        if dt > 0.0 {
            kin.coyote_time_left = (kin.coyote_time_left - dt).max(0.0);
            kin.jump_buffer_left = (kin.jump_buffer_left - dt).max(0.0);
        }

        if let Ok(output) = controller_outputs.get(entity) {
            const EPS: f32 = 1.0e-4;

            let desired_y = output.desired_translation.y;
            let effective_y = output.effective_translation.y;
            let blocked_down = desired_y < -EPS && effective_y > -EPS;

            let grounded = output.grounded || blocked_down;
            kin.grounded = grounded;
            if grounded {
                kin.coyote_time_left = tuning.coyote_time;

                if kin.velocity.y < 0.0 {
                    kin.velocity.y = 0.0;
                }
            }

            let blocked_up = desired_y > EPS && effective_y + EPS < desired_y;
            if blocked_up && kin.velocity.y > 0.0 {
                kin.velocity.y = 0.0;
            }
        } else {
            kin.grounded = false;
        }

        let mut wish_dir = Vec3::ZERO;
        if actions.move_xz != Vec2::ZERO {
            let local = actions.move_xz.normalize_or_zero();

            let forward = transform.rotation * Vec3::NEG_Z;
            let right = transform.rotation * Vec3::X;

            wish_dir = (right * local.x + forward * local.y).normalize_or_zero();
        }

        // TODO: accelerate to max_speed smoothly
        let max_speed = match *movement_mode {
            MovementMode::Walking => {
                if actions.sprinting {
                    tuning.sprint_speed
                } else {
                    tuning.walk_speed
                }
            }
            MovementMode::Flying => {
                if actions.sprinting {
                    tuning.sprint_fly_speed
                } else {
                    tuning.fly_speed
                }
            }
        };

        kin.velocity.x = wish_dir.x * max_speed;
        kin.velocity.z = wish_dir.z * max_speed;

        if actions.jump_pressed {
            kin.jump_buffer_left = tuning.jump_buffer;
            actions.jump_pressed = false;
        }

        match *movement_mode {
            MovementMode::Walking => {
                let can_jump = kin.grounded || kin.coyote_time_left > 0.0;
                if can_jump && kin.jump_buffer_left > 0.0 {
                    kin.velocity.y = tuning.jump_speed;
                    kin.grounded = false;
                    kin.coyote_time_left = 0.0;
                    kin.jump_buffer_left = 0.0;
                } else if kin.grounded {
                    kin.velocity.y = kin.velocity.y.min(0.0);
                } else {
                    kin.velocity.y -= tuning.gravity * dt;
                }
            }
            MovementMode::Flying => {
                if actions.upward_pressed {
                    wish_dir.y = 1.0;
                }
                if actions.downward_pressed {
                    wish_dir.y = -1.0;
                }
                kin.velocity.y = wish_dir.y * tuning.elevate_speed;
            }
        };

        controller.translation = Some(kin.velocity * dt);
    }
}
