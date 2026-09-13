use bevy::prelude::*;

use crate::{
    player::prelude::*,
    world::{components::*, resources::*},
};

pub fn unload_far_chunks(
    mut commands: Commands,
    registry: Res<ChunkRegistry>,
    mut unreg_list: ResMut<ChunksToUnregister>,
    player: Single<&Transform, With<Player>>,
) {
    let player_chunk = ChunkPos::from_world(player.translation);

    let center = player_chunk.0;
    let mut unloaded = 0;

    for (pos, chunk) in &registry.0 {
        let offset = pos - center;
        let IVec3 {
            x: dx,
            y: dy,
            z: dz,
        } = offset;

        if dy.abs() <= UNLOAD_HEIGHT && dx * dx + dz * dz <= UNLOAD_RADIUS * UNLOAD_RADIUS {
            continue;
        }

        commands.entity(*chunk).despawn();
        unreg_list.0.push(*pos);
        unloaded += 1;

        if unloaded == MAX_UNLOADS_PER_FRAME {
            break;
        }
    }
}

pub fn unregister_unloaded_chunks(
    mut registry: ResMut<ChunkRegistry>,
    mut height_cache: ResMut<ChunkHeightCache>,
    mut unreg_list: ResMut<ChunksToUnregister>,
) {
    unreg_list.0.iter().for_each(|pos| {
        registry.0.remove(pos);
        height_cache.0.remove(pos);
    });
    unreg_list.0.clear();
}
