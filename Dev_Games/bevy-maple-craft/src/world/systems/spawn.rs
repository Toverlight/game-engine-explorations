use bevy::prelude::*;

use crate::{player::prelude::*, world::prelude::*};

// pub fn update_last_player_chunk_pos(
//     player: Single<&Transform, With<Player>>,
//     mut last_chunk: ResMut<LastPlayerChunkPos>,
// ) {
//     let player_chunk = ChunkPos::from_world(player.translation);
//     last_chunk.0 = Some(player_chunk.0);
// }

pub fn spawn_chunks(
    mut commands: Commands,
    mut registry: ResMut<ChunkRegistry>,
    mut spawn_progress: ResMut<SpawnProgress>,
    player: Single<&Transform, With<Player>>,
    vision_offsets: Res<VisionOffsets>,
) {
    let player_chunk = ChunkPos::from_world(player.translation);
    let player_center = player_chunk.0;
    let mut sp_needs_reset = false;
    if let Some(sp_center) = spawn_progress.center {
        if sp_center != player_center {
            sp_needs_reset = true;
        }
    } else {
        sp_needs_reset = true;
    }
    if sp_needs_reset {
        spawn_progress.cursor = 0;
        spawn_progress.complete = false;
        spawn_progress.center = Some(player_center);
    }

    if spawn_progress.complete && spawn_progress.center == Some(player_center) {
        return;
    }

    let mut spawned = 0;
    for i in spawn_progress.cursor..vision_offsets.0.len() {
        let offset = &vision_offsets.0[i];
        let pos = player_center + offset;
        if registry.0.contains_key(&pos) {
            continue;
        }
        let chunk_pos = ChunkPos(pos);
        let transform = Transform::from_translation(chunk_pos.to_world_origin());
        let entity = commands
            .spawn((
                chunk_pos,
                ChunkState::Dirty,
                ChunkData::default(),
                transform,
                GlobalTransform::default(),
                Visibility::Visible,
                NeedsToGenerateTerrain,
            ))
            .id();
        registry.0.insert(pos, entity);
        spawned += 1;
        if spawned == MAX_CHUNKS_PER_FRAME {
            break;
        }
    }
    spawn_progress.cursor += spawned;
    if spawn_progress.cursor >= vision_offsets.0.len() - 1 {
        spawn_progress.complete = true;
    }
}

pub fn fill_terrain(
    registry: Res<ChunkRegistry>,
    noise: Res<TerrainNoise>,
    mut chunk_q: Query<
        (Entity, &ChunkPos, &mut ChunkData, &mut ChunkState),
        With<NeedsToGenerateTerrain>,
    >,
    mut neighbor_q: Query<&mut ChunkState, (Without<NeedsRemesh>, Without<NeedsToGenerateTerrain>)>,
    mut commands: Commands,
) {
    let mut filled = 0;
    let neighbor_offsets = [
        IVec3::X,
        IVec3::Y,
        IVec3::Z,
        IVec3::NEG_X,
        IVec3::NEG_Y,
        IVec3::NEG_Z,
    ];
    for (entity, pos, mut data, mut state) in &mut chunk_q {
        // if pos.0.y < 0 {
        //     data.blocks.fill(BlockId::Stone); // TODO: now pesudo terrain, needs change
        // }
        let world_origin_x = pos.0.x * CS as i32;
        let world_origin_z = pos.0.z * CS as i32;
        let chunk_base_y = pos.0.y * CS as i32;

        for lz in 0..CS {
            for lx in 0..CS {
                let wx = world_origin_x + lx as i32;
                let wz = world_origin_z + lz as i32;
                let target_h = terrain_height(&noise, wx, wz);

                for ly in 0..CS {
                    let world_y = chunk_base_y + ly as i32;
                    if world_y <= target_h {
                        let depth = target_h - world_y;
                        let bid = if depth == 0 {
                            BlockId::Grass
                        } else if depth <= 3 {
                            BlockId::Dirt
                        } else {
                            BlockId::Stone
                        };
                        data.blocks[lx + ly * CS + lz * CS2] = bid;
                    } else {
                        break;
                    }
                }
            }
        }

        commands.entity(entity).remove::<NeedsToGenerateTerrain>();
        *state = ChunkState::Dirty;
        for neighbor_offset in &neighbor_offsets {
            let neighbor_pos = pos.0 + neighbor_offset;
            if let Some(neighbor_entity) = registry.0.get(&neighbor_pos) {
                if let Ok(mut neighbor_state) = neighbor_q.get_mut(*neighbor_entity) {
                    match *neighbor_state {
                        ChunkState::Meshing => {
                            commands.entity(*neighbor_entity).insert(NeedsRemesh);
                        }
                        ChunkState::Ready => {
                            *neighbor_state = ChunkState::Dirty;
                        }
                        _ => {}
                    }
                }
            }
        }
        filled += 1;
        if filled == MAX_TERRAINS_PER_FRAME {
            break;
        }
    }
}
