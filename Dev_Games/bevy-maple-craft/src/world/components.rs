use bevy::{prelude::*, tasks::Task};
use bevy_rapier3d::prelude::Collider;

pub const CS: usize = 16; // chunk size, power of 2, <= 32
pub const CS2: usize = CS * CS;
pub const CS3: usize = CS * CS * CS;

pub const VISION_RADIUS: i32 = 8;
pub const VISION_HEIGHT: i32 = 3;
pub const UNLOAD_RADIUS: i32 = VISION_RADIUS + 2;
pub const UNLOAD_HEIGHT: i32 = VISION_HEIGHT + 2;

pub const MAX_CHUNKS_PER_FRAME: usize = 80;
pub const MAX_TERRAINS_PER_FRAME: usize = 60;
pub const MAX_UNLOADS_PER_FRAME: usize = 120;

#[derive(Clone, Copy, PartialEq, Eq, Default, Debug)]
#[repr(u8)]
pub enum BlockId {
    #[default]
    Air,
    Unknown,
    Stone,
    Dirt,
    Grass,
}

impl BlockId {
    pub const COUNT: usize = 5; // this should be manually maintained, sync this to block_id total

    pub fn as_usize(self) -> usize {
        self as u8 as usize
    }
}

impl TryFrom<usize> for BlockId {
    type Error = &'static str;

    fn try_from(value: usize) -> std::result::Result<Self, Self::Error> {
        // every time changed BlockId, sync this below
        match value {
            0 => Ok(BlockId::Air),
            1 => Ok(BlockId::Unknown),
            2 => Ok(BlockId::Stone),
            3 => Ok(BlockId::Dirt),
            4 => Ok(BlockId::Grass),
            _ => Err("Invalid enum index"),
        }
    }
}

#[derive(Component, Clone)]
pub struct ChunkData {
    pub blocks: Box<[BlockId; CS3]>,
}

impl ChunkData {
    #[inline(always)]
    pub fn get(&self, x: usize, y: usize, z: usize) -> BlockId {
        self.blocks[x + y * CS + z * CS2]
    }

    pub fn compute_chunk_surface(&self) -> Box<[i32; CS2]> {
        let mut hm = Box::new([i32::MIN; CS2]);
        for lz in 0..CS {
            for lx in 0..CS {
                for ly in (0..CS).rev() {
                    if self.blocks[lx + ly * CS + lz * CS2] != BlockId::Air {
                        hm[lx + lz * CS] = ly as i32;
                        break;
                    }
                }
            }
        }
        hm
    }
}

impl Default for ChunkData {
    fn default() -> Self {
        Self {
            blocks: Box::new([BlockId::Air; CS3]),
        }
    }
}

#[derive(Component, Clone, Copy, Hash, PartialEq, Eq, Reflect)]
pub struct ChunkPos(pub IVec3);

impl ChunkPos {
    pub fn from_world(world: Vec3) -> Self {
        Self(IVec3::new(
            (world.x / CS as f32).floor() as i32,
            (world.y / CS as f32).floor() as i32,
            (world.z / CS as f32).floor() as i32,
        ))
    }

    pub fn to_world_origin(&self) -> Vec3 {
        self.0.as_vec3() * CS as f32
    }
}

#[derive(Component, PartialEq, Eq, Clone, Copy, Reflect)]
pub enum ChunkState {
    Dirty,
    Meshing,
    Ready,
}

#[derive(Component)]
pub struct NeedsToGenerateTerrain;

#[derive(Component)]
pub struct NeedsRemesh;

#[derive(Default)]
pub struct MeshBuffers {
    pub positions: Vec<[f32; 3]>,
    pub normals: Vec<[f32; 3]>,
    pub uvs: Vec<[f32; 2]>,
    pub indices: Vec<u32>,
}

#[derive(Default)]
pub struct ColliderBuffers {
    pub positions: Vec<Vec3>,
    pub indices: Vec<[u32; 3]>,
}

#[derive(Default)]
pub struct ChunkMeshResult {
    pub parts: Vec<(BlockId, Mesh)>,
    pub collider: Option<Collider>,
}

#[derive(Component)]
pub struct ChunkMeshTask(pub Task<ChunkMeshResult>);

#[derive(Component, Clone, Copy, PartialEq, Eq)]
pub struct ChunkRenderChild(pub BlockId);
