use std::{collections::HashMap, f32::consts::PI};

use crate::world::terrain::make_noise;

use super::components::*;
use bevy::prelude::*;

use fastnoise_lite::*;

#[derive(Resource, Default)]
pub struct ChunkRegistry(pub HashMap<IVec3, Entity>);

#[derive(Resource, Default)]
pub struct ChunkHeightCache(pub HashMap<IVec3, Box<[i32; CS2]>>);

#[derive(Resource)]
pub struct TerrainNoise(pub FastNoiseLite);

impl Default for TerrainNoise {
    fn default() -> Self {
        Self(make_noise())
    }
}

// #[derive(Resource, Default)]
// pub struct LastPlayerChunkPos(pub Option<IVec3>);

#[derive(Resource, Default)]
pub struct SpawnProgress {
    pub cursor: usize,
    /// for security, ensure unload doesn't drop chunks in vision, and do set this to false when block destruction happens in vision.
    pub complete: bool,
    pub center: Option<IVec3>,
}

#[derive(Resource)]
pub struct VisionOffsets(pub Vec<IVec3>);

impl Default for VisionOffsets {
    fn default() -> Self {
        Self(Vec::with_capacity(
            (((VISION_RADIUS * VISION_RADIUS) as f32 * PI + 1.0) * (VISION_HEIGHT * 2) as f32)
                as usize,
        )) // Is this reasonable?
    }
}

/// TODO: KeepOffsets may be useful in the future objects refresh in keep bound(closest first).
///
/// It's also useful for sorting in place for any instant use, such as map generating.
///
/// Meanwhile it means its order is NOT STABLE.For any case, which is order-dependent, should sort before the algorithm.
#[derive(Resource)]
pub struct KeepOffsets(pub Vec<IVec3>);

impl Default for KeepOffsets {
    fn default() -> Self {
        Self(Vec::with_capacity(
            (((UNLOAD_RADIUS * UNLOAD_RADIUS) as f32 * PI + 1.0) * (UNLOAD_HEIGHT * 2) as f32)
                as usize,
        )) // Is this reasonable?
    }
}

#[derive(Resource)]
pub struct ChunksToUnregister(pub Vec<IVec3>);

impl Default for ChunksToUnregister {
    fn default() -> Self {
        Self(Vec::with_capacity(300))
    }
}

#[derive(Resource, Default)]
pub struct BlockMaterialRegistry(pub [Option<Handle<StandardMaterial>>; BlockId::COUNT]);
