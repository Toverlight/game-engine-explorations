use std::time::Duration;

use bevy::input::mouse::MouseMotion;
use bevy::prelude::*;
use bevy::window::{CursorOptions, PrimaryWindow};

use super::events::*;
use crate::player::prelude::Player;
use crate::ui::prelude::*;

#[derive(Resource, Debug, Default, Clone, Copy)]
pub struct LookState {
    pub mouse_delta: Vec2,
}

#[derive(Resource, Debug, Default, Clone, Copy)]
pub struct PlayerActions {
    pub move_xz: Vec2,
    pub jump_pressed: bool,
    pub upward_pressed: bool,
    pub downward_pressed: bool,
    pub sprinting: bool,
}

#[derive(Resource, Debug, Default, Clone, Copy)]
pub struct SprintDetector {
    pub last_forward_tap: Option<Duration>,
    pub armed: bool,
}

#[derive(Resource, Debug, Default, Clone, Copy)]
pub struct ToggleFlightDetector {
    pub last_upward_tap: Option<Duration>,
}

pub fn gather_movement_actions(
    keys: Res<ButtonInput<KeyCode>>,
    ui_state: Res<UiState>,
    mut actions: ResMut<PlayerActions>,
) {
    if ui_state.cursor_released() {
        return;
    }
    let mut x = 0.0;
    let mut y = 0.0;

    if keys.pressed(KeyCode::KeyW) {
        y += 1.0;
    }
    if keys.pressed(KeyCode::KeyS) {
        y -= 1.0;
    }
    if keys.pressed(KeyCode::KeyA) {
        x -= 1.0;
    }
    if keys.pressed(KeyCode::KeyD) {
        x += 1.0;
    }

    actions.move_xz = Vec2::new(x, y);
}

pub fn gather_jump_action(
    keys: Res<ButtonInput<KeyCode>>,
    ui_state: Res<UiState>,
    mut actions: ResMut<PlayerActions>,
) {
    if ui_state.cursor_released() {
        return;
    }
    actions.jump_pressed |= keys.just_pressed(KeyCode::Space);
}

pub fn gather_upward_action(
    keys: Res<ButtonInput<KeyCode>>,
    ui_state: Res<UiState>,
    mut actions: ResMut<PlayerActions>,
) {
    if ui_state.cursor_released() {
        return;
    }
    actions.upward_pressed = keys.pressed(KeyCode::Space);
}

pub fn gather_downward_action(
    keys: Res<ButtonInput<KeyCode>>,
    ui_state: Res<UiState>,
    mut actions: ResMut<PlayerActions>,
) {
    if ui_state.cursor_released() {
        return;
    }
    actions.downward_pressed = keys.pressed(KeyCode::ShiftLeft);
}

pub fn gather_sprint_action(
    time: Res<Time>,
    keys: Res<ButtonInput<KeyCode>>,
    ui_state: Res<UiState>,
    mut detector: ResMut<SprintDetector>,
    mut actions: ResMut<PlayerActions>,
) {
    if ui_state.cursor_released() {
        return;
    }
    const SPRINT_WINDOW: Duration = Duration::from_millis(250);

    let now = time.elapsed();

    if keys.just_pressed(KeyCode::KeyW) {
        if let Some(last) = detector.last_forward_tap {
            if now.saturating_sub(last) <= SPRINT_WINDOW {
                detector.armed = true;
            }
        }
        detector.last_forward_tap = Some(now);
    }

    if !keys.pressed(KeyCode::KeyW) {
        detector.armed = false;
    }

    actions.sprinting = detector.armed && keys.pressed(KeyCode::KeyW);
}

pub fn gather_toggle_flight_action(
    time: Res<Time>,
    keys: Res<ButtonInput<KeyCode>>,
    ui_state: Res<UiState>,
    mut detector: ResMut<ToggleFlightDetector>,
    player: Single<Entity, With<Player>>,
    mut commands: Commands,
) {
    if ui_state.cursor_released() {
        return;
    }
    const TOGGLE_FLIGHT_WINDOW: Duration = Duration::from_millis(250);

    let now = time.elapsed();

    if keys.just_pressed(KeyCode::Space) {
        if let Some(last) = detector.last_upward_tap {
            if now.saturating_sub(last) <= TOGGLE_FLIGHT_WINDOW {
                commands.trigger(ToggleFlightEvent { entity: *player });
            }
        }
        detector.last_upward_tap = Some(now);
    }
}

pub fn gather_look_input(
    mut mouse_motion: MessageReader<MouseMotion>,
    mut look: ResMut<LookState>,
    ui_state: Res<UiState>,
) {
    if ui_state.cursor_released() {
        mouse_motion.clear();
        look.mouse_delta = Vec2::ZERO;
        return;
    }

    let mut delta = Vec2::ZERO;
    for ev in mouse_motion.read() {
        delta += ev.delta;
    }
    look.mouse_delta = delta;
}

pub fn apply_cursor_grab(
    state: Res<UiState>,
    mut cursor: Single<&mut CursorOptions, With<PrimaryWindow>>,
) {
    if state.cursor_released() {
        cursor.grab_mode = bevy::window::CursorGrabMode::None;
        cursor.visible = true;
    } else {
        cursor.grab_mode = bevy::window::CursorGrabMode::Locked;
        cursor.visible = false;
    }
}

pub fn apply_pause(state: Res<UiState>, mut time: ResMut<Time<Virtual>>) {
    if state.game_paused() {
        time.pause();
    } else {
        time.unpause();
    }
}
