use bevy::prelude::*;

use crate::world::{components::*, resources::*};

mod components;
// mod conversion;
mod meshing;
mod resources;
mod systems;
mod terrain;

pub mod prelude {
    pub use super::components::*;
    pub use super::resources::*;
    pub use super::terrain::*;
}

use systems::*;

#[derive(SystemSet, Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum WorldSet {
    Load,
}

#[derive(SystemSet, Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum ChunkSet {
    Spawn,
    Mesh,
    Unload,
}

pub struct WorldPlugin;

impl Plugin for WorldPlugin {
    fn build(&self, app: &mut App) {
        // types
        app.register_type::<ChunkState>()
            .register_type::<ChunkPos>();
        // resources
        app.init_resource::<ChunkRegistry>()
            .init_resource::<ChunkHeightCache>()
            // .init_resource::<LastPlayerChunkPos>()
            .init_resource::<TerrainNoise>()
            .init_resource::<SpawnProgress>()
            .init_resource::<VisionOffsets>()
            .init_resource::<KeepOffsets>()
            .init_resource::<ChunksToUnregister>()
            .init_resource::<BlockMaterialRegistry>();
        // events: TODO

        // system sets order
        app.configure_sets(
            Update,
            (ChunkSet::Spawn, ChunkSet::Mesh, ChunkSet::Unload).chain(),
        );
        // systems
        app.add_systems(
            Startup,
            (
                load::register_block_materials,
                load::initialize_lighting,
                load::generate_vision_offsets,
                load::generate_keep_offsets,
            )
                .in_set(WorldSet::Load),
        );
        app.add_systems(
            Update,
            (
                (
                    // spawn::update_last_player_chunk_pos,
                    spawn::spawn_chunks,
                    spawn::fill_terrain,
                )
                    .chain()
                    .in_set(ChunkSet::Spawn),
                (mesh::queue_meshing, mesh::poll_mesh_tasks)
                    .chain()
                    .in_set(ChunkSet::Mesh),
                (
                    unload::unload_far_chunks,
                    unload::unregister_unloaded_chunks,
                )
                    .chain()
                    .in_set(ChunkSet::Unload),
            ),
        ); // TODO
    }
}
