use std::collections::HashSet;

use bevy::{
    prelude::*,
    tasks::{AsyncComputeTaskPool, futures::check_ready},
};
use bevy_rapier3d::prelude::{Collider, RigidBody};

use crate::world::{components::*, meshing::*, resources::*};

pub fn queue_meshing(
    mut chunk_q: Query<(Entity, &ChunkPos, &ChunkData, &mut ChunkState)>,
    registry: Res<ChunkRegistry>,
    data_q: Query<&ChunkData>,
    mut commands: Commands,
) {
    for (entity, pos, center_data, mut state) in &mut chunk_q {
        if *state != ChunkState::Dirty {
            continue;
        }
        *state = ChunkState::Meshing;

        let neighbor = |offset: IVec3| -> Option<ChunkData> {
            registry
                .0
                .get(&(pos.0 + offset))
                .and_then(|e| data_q.get(*e).ok())
                .cloned()
        };

        let input = MeshInputOwned {
            center: center_data.clone(),
            px: neighbor(IVec3::X),
            py: neighbor(IVec3::Y),
            pz: neighbor(IVec3::Z),
            nx: neighbor(IVec3::NEG_X),
            ny: neighbor(IVec3::NEG_Y),
            nz: neighbor(IVec3::NEG_Z),
        };

        let task = AsyncComputeTaskPool::get().spawn(async move { build_chunk_mesh(input) });
        commands.entity(entity).insert(ChunkMeshTask(task));
    }
}

pub fn poll_mesh_tasks(
    mut meshes: ResMut<Assets<Mesh>>,
    registry: Res<BlockMaterialRegistry>,
    mut tasks: Query<(
        Entity,
        &mut ChunkMeshTask,
        &mut ChunkState,
        Option<&NeedsRemesh>,
        Option<&Children>,
    )>,
    child_render_q: Query<&ChunkRenderChild>,
    mut child_mesh_q: Query<&mut Mesh3d, With<ChunkRenderChild>>,
    mut commands: Commands,
) {
    for (chunk, mut task, mut state, needs_remesh_opt, children_opt) in &mut tasks {
        let Some(res) = check_ready(&mut task.0) else {
            continue;
        };

        let ChunkMeshResult { parts, collider } = res;

        // Child is a fixed bundle. So the table is expected to contain all of the renderer children.
        let mut table: [Option<Entity>; BlockId::COUNT] = Default::default();
        if let Some(children) = children_opt {
            for &child in children {
                if let Ok(c) = child_render_q.get(child) {
                    let chunk_render = c;
                    table[chunk_render.0.as_usize()] = Some(child);
                }
            }
            let bids: HashSet<Entity> = parts
                .iter()
                .filter_map(|(bid, _)| table[bid.as_usize()])
                .collect();
            for child in children {
                if let Ok(_) = child_render_q.get(*child) {
                    if !bids.contains(child) {
                        commands.entity(*child).despawn();
                    }
                }
            }
        }

        let mat_unknown_opt = &registry.0[BlockId::Unknown.as_usize()];

        for (bid, mesh) in parts {
            let Some(handle_mat) = &registry.0[bid.as_usize()] else {
                warn_once!("Concrete material of BlockId {bid:?} has not been registered!");
                continue;
            };

            if let Some(child) = &table[bid.as_usize()] {
                let Ok(handle_mesh) = child_mesh_q.get_mut(*child) else {
                    error_once!(
                        "Unreachable case: Mesh3d should coexist with sub-mesh child entity!"
                    );
                    let h = meshes.add(mesh);
                    commands
                        .entity(*child)
                        .insert((Mesh3d(h), Visibility::Inherited));
                    if let Some(mat_unknown) = mat_unknown_opt {
                        commands
                            .entity(*child)
                            .insert(MeshMaterial3d(mat_unknown.clone()));
                    }
                    continue;
                };
                if let Some(m) = meshes.get_mut(&handle_mesh.0) {
                    *m = mesh;
                }
            } else {
                let h = meshes.add(mesh);
                commands.entity(chunk).with_children(|p| {
                    p.spawn((
                        ChunkRenderChild(bid),
                        Mesh3d(h),
                        MeshMaterial3d(handle_mat.clone()),
                        Visibility::Inherited,
                        Transform::IDENTITY,
                    ));
                });
            }
        }

        if let Some(collider) = collider {
            commands.entity(chunk).insert((RigidBody::Fixed, collider));
        } else {
            commands.entity(chunk).remove::<(RigidBody, Collider)>();
        }

        if let Some(_) = needs_remesh_opt {
            commands.entity(chunk).remove::<NeedsRemesh>();
            *state = ChunkState::Dirty;
        } else {
            *state = ChunkState::Ready;
        }
        commands.entity(chunk).remove::<ChunkMeshTask>();
    }
}
