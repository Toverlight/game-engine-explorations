use std::collections::BTreeSet;

use bevy::prelude::*;

use crate::ui::layers::{UiKind, UiLayer};

#[derive(Resource, Default)]
pub struct UiState {
    pub active_layer: Option<UiLayer>, // panel or modal
    pub overlays: BTreeSet<UiLayer>,   // overlay
}

// derived information
impl UiState {
    pub fn cursor_released(&self) -> bool {
        self.active_layer
            .map_or(false, |l| l.kind() != UiKind::Overlay)
    }
    pub fn game_paused(&self) -> bool {
        self.active_layer
            .map_or(false, |l| l.kind() == UiKind::Modal)
    }
    pub fn visible(&self, layer: UiLayer) -> bool {
        match layer.kind() {
            UiKind::Overlay => self.overlays.contains(&layer),
            UiKind::Panel | UiKind::Modal => self.active_layer == Some(layer),
        }
    }
    pub fn active(&self, layer: UiLayer) -> bool {
        match layer.kind() {
            UiKind::Panel | UiKind::Modal => self.active_layer == Some(layer),
            UiKind::Overlay => false, // Overlay can't be activated
        }
    }
}
