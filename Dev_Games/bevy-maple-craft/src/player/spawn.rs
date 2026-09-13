use bevy::{
    prelude::*,
    window::{CursorGrabMode, CursorOptions, PrimaryWindow},
};
use bevy_rapier3d::prelude::*;

use super::components::*;

pub fn spawn_player(
    mut commands: Commands,
    mut cursor: Single<&mut CursorOptions, With<PrimaryWindow>>,
) {
    cursor.grab_mode = CursorGrabMode::Locked;
    cursor.visible = false;

    let body = commands
        .spawn((
            Player,
            PlayerKinematics::default(),
            MovementMode::default(),
            LookAngles::default(),
            Visibility::Visible,
            Transform::from_translation(Vec3::new(0.0, 21.0, 0.0)), // y of player should be higher than body cuboid hy + ground y
            GlobalTransform::default(),
            RigidBody::KinematicPositionBased,
            Collider::cuboid(0.32, 0.85, 0.16),
            KinematicCharacterController {
                offset: CharacterLength::Absolute(0.06),
                slide: true,
                autostep: None,
                // Some(CharacterAutostep {
                //     max_height: CharacterLength::Absolute(0.06),
                //     min_width: CharacterLength::Absolute(0.2),
                //     include_dynamic_bodies: false,
                // }),
                snap_to_ground: Some(CharacterLength::Absolute(0.03)),
                normal_nudge_factor: 0.005,
                apply_impulse_to_dynamic_bodies: false,
                ..default()
            },
        ))
        .id();

    commands.entity(body).with_children(|c| {
        c.spawn((
            PlayerCamera,
            Camera3d::default(),
            Visibility::Visible,
            Transform::from_translation(Vec3::new(0.0, 0.65, 0.0)),
        ));
    });
}
