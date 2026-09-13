use bevy::prelude::*;

use bevy_rapier3d::prelude::KinematicCharacterControllerOutput;
use bevy_rapier3d::render::DebugRenderContext;

use crate::debug::components::*;
use crate::input::prelude::*;
use crate::player::prelude::*;
use crate::ui::prelude::*;

pub fn spawn_overlay(mut commands: Commands) {
    commands.spawn((
        DebugText,
        Text::new(""),
        TextFont::from_font_size(16.0),
        TextColor(Color::srgb(1.0, 1.0, 1.0)),
        Visibility::Hidden,
        Node {
            position_type: PositionType::Absolute,
            top: px(8.0),
            left: px(8.0),
            ..default()
        },
    ));
}

pub fn toggle_debug_text(
    ui_state: Res<UiState>,
    mut query: Query<&mut Visibility, With<DebugText>>,
) {
    if ui_state.is_changed() {
        if let Ok(mut visibility) = query.single_mut() {
            *visibility = if ui_state.visible(UiLayer::DebugText) {
                Visibility::Visible
            } else {
                Visibility::Hidden
            }
        }
    }
}

pub fn toggle_physics_debug(ui_state: Res<UiState>, ctx: Option<ResMut<DebugRenderContext>>) {
    let Some(mut ctx) = ctx else {
        return;
    };

    if ui_state.visible(UiLayer::DebugPhysics) {
        ctx.enabled = true;
    } else {
        ctx.enabled = false;
    }
}

pub fn update_overlay(
    ui_state: Res<UiState>,
    actions: Res<PlayerActions>,
    player: Query<(&Transform, &PlayerKinematics, &MovementMode), With<Player>>,
    controller_out: Query<&KinematicCharacterControllerOutput, With<Player>>,
    mut text: Single<&mut Text, With<DebugText>>,
) {
    if !ui_state.visible(UiLayer::DebugText) {
        return;
    }
    let Ok((t, kin, movement_mode)) = player.single() else {
        return;
    };

    let (desired, effective, collisions, grounded_out) = controller_out
        .iter()
        .next()
        .map(|out| {
            let d = Vec3::new(
                out.desired_translation.x,
                out.desired_translation.y,
                out.desired_translation.z,
            );
            let e = Vec3::new(
                out.effective_translation.x,
                out.effective_translation.y,
                out.effective_translation.z,
            );
            (d, e, out.collisions.len(), out.grounded)
        })
        .unwrap_or((Vec3::ZERO, Vec3::ZERO, 0, false));

    text.0 = format!(
        "pos: [{:.2}, {:.2}, {:.2}]\nvel: [{:.2}, {:.2}, {:.2}]\nmovement_mode: {:?}\ngrounded(ours/out): {}/{}\nsprinting: {}\ndesired dt: [{:.3}, {:.3}, {:.3}]\neffective dt: [{:.3}, {:.3}, {:.3}]\ncollisions: {}\ncoyote: {:.2}  jump_buf: {:.2}",
        t.translation.x,
        t.translation.y,
        t.translation.z,
        kin.velocity.x,
        kin.velocity.y,
        kin.velocity.z,
        movement_mode,
        kin.grounded,
        grounded_out,
        actions.sprinting,
        desired.x,
        desired.y,
        desired.z,
        effective.x,
        effective.y,
        effective.z,
        collisions,
        kin.coyote_time_left,
        kin.jump_buffer_left
    );
}
