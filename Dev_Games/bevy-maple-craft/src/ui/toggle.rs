use bevy::prelude::*;
use strum::IntoEnumIterator;

use crate::ui::{
    layers::{UiKind, UiLayer},
    resources::UiState,
};

pub fn toggle_ui(keys: Res<ButtonInput<KeyCode>>, mut state: ResMut<UiState>) {
    for layer in UiLayer::iter() {
        if keys.just_pressed(layer.toggle_key()) {
            match layer.kind() {
                UiKind::Overlay => {
                    if state.overlays.remove(&layer) {
                        // was shown, now hide
                    } else {
                        state.overlays.insert(layer);
                    }
                }
                UiKind::Panel | UiKind::Modal => match state.active_layer {
                    Some(active) if active == layer => {
                        state.active_layer = None;
                    }
                    _ => {
                        state.active_layer = Some(layer);
                    }
                },
            }
            return;
        }
    }
}
