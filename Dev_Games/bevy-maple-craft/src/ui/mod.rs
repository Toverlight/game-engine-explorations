use bevy::prelude::*;

use crate::ui::resources::*;

mod layers;
mod resources;
mod toggle;

pub mod prelude {
    pub use super::layers::*;
    pub use super::resources::*;
}

pub struct UiPlugin;

impl Plugin for UiPlugin {
    fn build(&self, app: &mut App) {
        app.init_resource::<UiState>()
            .add_systems(Update, toggle::toggle_ui);
    }
}
