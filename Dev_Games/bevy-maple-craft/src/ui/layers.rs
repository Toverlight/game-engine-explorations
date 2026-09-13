use bevy::prelude::*;
use strum::EnumIter;

/// Priority order: low -> high
#[derive(Clone, Copy, PartialEq, Eq, Hash, PartialOrd, Ord, EnumIter)]
pub enum UiLayer {
    DebugPhysics,
    DebugText,
    DebugMap,
    GameMenu, // TODO: needs implementation
}

impl UiLayer {
    pub fn kind(self) -> UiKind {
        match self {
            UiLayer::DebugPhysics => UiKind::Overlay,
            UiLayer::DebugText => UiKind::Overlay,
            UiLayer::DebugMap => UiKind::Panel,
            UiLayer::GameMenu => UiKind::Modal,
        }
    }

    pub fn toggle_key(self) -> KeyCode {
        match self {
            UiLayer::DebugPhysics => KeyCode::F2,
            UiLayer::DebugText => KeyCode::F3,
            UiLayer::DebugMap => KeyCode::F1,
            UiLayer::GameMenu => KeyCode::Escape,
        }
    }
}

#[derive(Clone, Copy, PartialEq, Eq, Hash, PartialOrd, Ord)]
pub enum UiKind {
    Modal,
    Panel,
    Overlay,
}
