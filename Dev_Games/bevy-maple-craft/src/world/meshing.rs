use bevy::{
    asset::RenderAssetUsages,
    mesh::{Indices, PrimitiveTopology},
    prelude::*,
};
use bevy_rapier3d::prelude::Collider;

use crate::world::components::*;

pub struct MeshInput<'a> {
    pub center: &'a ChunkData,
    pub nx: Option<&'a ChunkData>,
    pub px: Option<&'a ChunkData>,
    pub ny: Option<&'a ChunkData>,
    pub py: Option<&'a ChunkData>,
    pub nz: Option<&'a ChunkData>,
    pub pz: Option<&'a ChunkData>,
}

pub struct MeshInputOwned {
    pub center: ChunkData,
    pub nx: Option<ChunkData>,
    pub px: Option<ChunkData>,
    pub ny: Option<ChunkData>,
    pub py: Option<ChunkData>,
    pub nz: Option<ChunkData>,
    pub pz: Option<ChunkData>,
}

impl MeshInputOwned {
    pub fn as_ref(&self) -> MeshInput<'_> {
        MeshInput {
            center: &self.center,
            px: self.px.as_ref(),
            py: self.py.as_ref(),
            pz: self.pz.as_ref(),
            nx: self.nx.as_ref(),
            ny: self.ny.as_ref(),
            nz: self.nz.as_ref(),
        }
    }
}

fn axis_to_xyz(axis: usize, slice: usize, u: usize, v: usize) -> [usize; 3] {
    match axis {
        // (1, 2)
        0 => [slice, u, v],
        // (2, 0)
        1 => [v, slice, u],
        // (0, 1)
        _ => [u, v, slice],
    }
}

fn axis_to_xyz_f(axis: usize, slice: f32, u: f32, v: f32) -> [f32; 3] {
    match axis {
        // (1, 2)
        0 => [slice, u, v],
        // (2, 0)
        1 => [v, slice, u],
        // (0, 1)
        _ => [u, v, slice],
    }
}

fn neighbor_block(
    input: &MeshInput,
    x: usize,
    y: usize,
    z: usize,
    axis: usize,
    positive: bool,
) -> BlockId {
    match (axis, positive) {
        (0, true) => {
            if x + 1 < CS {
                input.center.get(x + 1, y, z)
            } else {
                input.px.map_or(BlockId::Stone, |c| c.get(0, y, z))
            }
        }
        (0, false) => {
            if x > 0 {
                input.center.get(x - 1, y, z)
            } else {
                input.nx.map_or(BlockId::Stone, |c| c.get(CS - 1, y, z))
            }
        }
        (1, true) => {
            if y + 1 < CS {
                input.center.get(x, y + 1, z)
            } else {
                input.py.map_or(BlockId::Stone, |c| c.get(x, 0, z))
            }
        }
        (1, false) => {
            if y > 0 {
                input.center.get(x, y - 1, z)
            } else {
                input.ny.map_or(BlockId::Stone, |c| c.get(x, CS - 1, z))
            }
        }
        (2, true) => {
            if z + 1 < CS {
                input.center.get(x, y, z + 1)
            } else {
                input.pz.map_or(BlockId::Stone, |c| c.get(x, y, 0))
            }
        }
        (2, false) => {
            if z > 0 {
                input.center.get(x, y, z - 1)
            } else {
                input.nz.map_or(BlockId::Stone, |c| c.get(x, y, CS - 1))
            }
        }
        _ => BlockId::Air,
    }
}

pub fn greedy_pass(
    per_id: &mut [MeshBuffers; BlockId::COUNT],
    collider_buf: &mut ColliderBuffers,
    input: &MeshInput,
    axis: usize,
    positive: bool,
) {
    for slice in 0..CS {
        // masking
        let mut mask = [[None::<BlockId>; CS]; CS];
        for u in 0..CS {
            for v in 0..CS {
                let [x, y, z] = axis_to_xyz(axis, slice, u, v);
                let block = input.center.get(x, y, z);
                let neighbor = neighbor_block(input, x, y, z, axis, positive);
                if block != BlockId::Air && neighbor == BlockId::Air {
                    mask[u][v] = Some(block);
                }
            }
        }
        // merging
        let mut done = [[false; CS]; CS];
        for u in 0..CS {
            for v in 0..CS {
                let Some(bid) = mask[u][v] else { continue };
                if done[u][v] {
                    continue;
                }

                let mut w = 1;
                while v + w < CS && mask[u][v + w] == Some(bid) {
                    w += 1;
                }

                let mut h = 1;
                'outer: while u + h < CS {
                    for k in 0..w {
                        if mask[u + h][v + k] != Some(bid) {
                            break 'outer;
                        }
                    }
                    h += 1;
                }

                let mesh_buf = &mut per_id[bid.as_usize()];
                emit_quad(mesh_buf, collider_buf, axis, positive, slice, u, v, w, h);
                for du in 0..h {
                    for dv in 0..w {
                        done[u + du][v + dv] = true;
                    }
                }
            }
        }
    }
}

fn emit_quad(
    mesh_buffers: &mut MeshBuffers,
    collider_buf: &mut ColliderBuffers,
    axis: usize,
    positive: bool,
    slice: usize,
    u: usize,
    v: usize,
    w: usize,
    h: usize,
) {
    // face plane depth
    let s = if positive {
        (slice + 1) as f32
    } else {
        slice as f32
    };
    let (uf, vf, wf, hf) = (u as f32, v as f32, w as f32, h as f32);

    // four corners
    let corners = [
        axis_to_xyz_f(axis, s, uf, vf),
        axis_to_xyz_f(axis, s, uf + hf, vf),
        axis_to_xyz_f(axis, s, uf + hf, vf + wf),
        axis_to_xyz_f(axis, s, uf, vf + wf),
    ];

    // normals(same)
    let sign = if positive { 1.0_f32 } else { -1.0 };
    let normal: [f32; 3] = match axis {
        0 => [sign, 0.0, 0.0],
        1 => [0.0, sign, 0.0],
        _ => [0.0, 0.0, sign],
    };

    // uvs(tiled)
    let quad_uvs: [[f32; 2]; 4] = [[0.0, 0.0], [hf, 0.0], [hf, wf], [0.0, wf]];

    // push geometry
    let base_mesh = mesh_buffers.positions.len() as u32;
    let base_collider = collider_buf.positions.len() as u32;

    let vertices = corners
        .clone()
        .into_iter()
        .map(Vec3::from)
        .collect::<Vec<Vec3>>();
    collider_buf.positions.extend(vertices);

    mesh_buffers.positions.extend(corners);
    mesh_buffers.uvs.extend(quad_uvs);
    for _ in 0..4 {
        mesh_buffers.normals.push(normal);
    }

    // two winding triangles
    if !positive {
        mesh_buffers.indices.extend([
            base_mesh,
            base_mesh + 2,
            base_mesh + 1,
            base_mesh,
            base_mesh + 3,
            base_mesh + 2,
        ]);
        collider_buf.indices.extend([
            [base_collider, base_collider + 2, base_collider + 1],
            [base_collider, base_collider + 3, base_collider + 2],
        ]);
    } else {
        mesh_buffers.indices.extend([
            base_mesh,
            base_mesh + 1,
            base_mesh + 2,
            base_mesh,
            base_mesh + 2,
            base_mesh + 3,
        ]);
        collider_buf.indices.extend([
            [base_collider, base_collider + 1, base_collider + 2],
            [base_collider, base_collider + 2, base_collider + 3],
        ]);
    }
}

pub fn build_chunk_mesh(input: MeshInputOwned) -> ChunkMeshResult {
    let mut per_id: [MeshBuffers; BlockId::COUNT] = Default::default();
    let mut collider_buf = ColliderBuffers::default();

    let input = &input.as_ref();

    for axis in 0..3usize {
        for &positive in &[true, false] {
            greedy_pass(&mut per_id, &mut collider_buf, input, axis, positive);
        }
    }

    let mut res = ChunkMeshResult::default();

    // mesh output
    // the first one is Air, skip
    for bid_u in 1..BlockId::COUNT {
        if per_id[bid_u].indices.is_empty() {
            continue;
        }
        let Ok(bid) = BlockId::try_from(bid_u) else {
            continue;
        };
        let mut mesh = Mesh::new(
            PrimitiveTopology::TriangleList,
            RenderAssetUsages::MAIN_WORLD | RenderAssetUsages::RENDER_WORLD,
        );
        mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, per_id[bid_u].positions.clone());
        mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, per_id[bid_u].normals.clone());
        mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, per_id[bid_u].uvs.clone());
        mesh.insert_indices(Indices::U32(per_id[bid_u].indices.clone()));

        res.parts.push((bid, mesh));
    }

    // collider output
    if collider_buf.indices.is_empty() {
        res.collider = None;
    } else {
        let collider = Collider::trimesh(collider_buf.positions, collider_buf.indices).ok();
        res.collider = collider;
    }

    res
}
