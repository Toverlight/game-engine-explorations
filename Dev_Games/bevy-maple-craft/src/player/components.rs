use bevy::prelude::*;

#[derive(Component, Debug)]
pub struct Player;

#[derive(Resource, Debug, Clone, Copy)]
pub struct PlayerTuning {
    pub mouse_sensitivity: f32,
    pub pitch_limit: f32,
    pub walk_speed: f32,
    pub fly_speed: f32,
    pub sprint_speed: f32,
    pub sprint_fly_speed: f32,
    pub jump_speed: f32,
    pub elevate_speed: f32,
    pub gravity: f32,
    pub coyote_time: f32,
    pub jump_buffer: f32,
}

const GRAVITY: f32 = 18.0; // meters per sec^2
const JUMP_CLIMAX: f32 = 1.25; // in meters

impl Default for PlayerTuning {
    fn default() -> Self {
        Self {
            mouse_sensitivity: 0.006,
            pitch_limit: 1.54,
            walk_speed: 4.3,
            fly_speed: 9.7,
            sprint_speed: 7.4,
            sprint_fly_speed: 14.5,
            jump_speed: (2.0_f32 * GRAVITY * JUMP_CLIMAX).sqrt(),
            elevate_speed: 6.7,
            gravity: GRAVITY,
            coyote_time: 0.10,
            jump_buffer: 0.11,
        }
    }
}

#[derive(Component, Debug, Default, Clone, Copy)]
pub struct LookAngles {
    pub pitch: f32,
    pub yaw: f32,
}

#[derive(Component, Debug)]
pub struct PlayerCamera;

#[derive(Component, Default, Debug, Clone, Copy)]
pub struct PlayerKinematics {
    pub velocity: Vec3,
    pub grounded: bool,
    pub coyote_time_left: f32,
    pub jump_buffer_left: f32,
}

#[derive(Component, Default, PartialEq, Eq, Debug, Clone, Copy)]
pub enum MovementMode {
    #[default]
    Walking,
    Flying,
}
