use bevy::prelude::*;

use crate::world::{components::*, resources::*};

pub fn register_block_materials(
    mut materials: ResMut<Assets<StandardMaterial>>,
    mut registry: ResMut<BlockMaterialRegistry>,
) {
    // hard code (temporarily)
    let block_materials = [
        (
            BlockId::Unknown,
            StandardMaterial {
                base_color: Color::srgb_u8(91, 47, 179),
                ..default()
            },
        ),
        (
            BlockId::Stone,
            StandardMaterial {
                base_color: Color::srgb_u8(172, 172, 172),
                // perceptual_roughness: 0.1,
                ..default()
            },
        ),
        (
            BlockId::Dirt,
            StandardMaterial {
                base_color: Color::srgb(0.45, 0.30, 0.18),
                // perceptual_roughness: 0.7,
                ..default()
            },
        ),
        (
            BlockId::Grass,
            StandardMaterial {
                base_color: Color::srgb(0.22, 0.65, 0.28),
                // perceptual_roughness: 0.95,
                ..default()
            },
        ),
    ];

    for (bid, material) in block_materials {
        let handle = materials.add(material);
        registry.0[bid.as_usize()] = Some(handle);
    }
}

pub fn initialize_lighting(mut commands: Commands, mut ambient: ResMut<GlobalAmbientLight>) {
    ambient.brightness = 120.0;

    commands.insert_resource(ClearColor(Color::srgb(0.53, 0.75, 0.93)));

    commands.spawn((
        DirectionalLight {
            illuminance: 2_000.0,
            shadows_enabled: true,
            ..default()
        },
        Transform::from_rotation(Quat::from_euler(EulerRot::XYZ, -1.1, 0.4, 0.0)),
    ));
}

pub fn generate_vision_offsets(mut vision_offsets: ResMut<VisionOffsets>) {
    let r = VISION_RADIUS;
    let h = VISION_HEIGHT;
    let mut v: Vec<(i32, IVec3)> = Vec::new();
    // TODO: needs to be optimized
    for dy in -h..=h {
        for dz in -r..=r {
            for dx in -r..=r {
                let d2_xz = dx * dx + dz * dz;
                if d2_xz > r * r {
                    continue;
                }
                // order is d2_xz first, then height (Y) distance
                let key = d2_xz * 100 + dy.abs(); // mul 100 is to make d2_xz far more important than dy.abs()
                v.push((key, IVec3::new(dx, dy, dz)));
            }
        }
    }
    v.sort_by_key(|(key, _)| *key); // closest first
    vision_offsets.0 = v.into_iter().map(|(_, o)| o).collect();
}

pub fn generate_keep_offsets(mut keep_offsets: ResMut<KeepOffsets>) {
    let r = UNLOAD_RADIUS;
    let h = UNLOAD_HEIGHT;
    let mut v: Vec<(i32, IVec3)> = Vec::new();
    // TODO: needs to be optimized
    for dy in -h..=h {
        for dz in -r..=r {
            for dx in -r..=r {
                let d2_xz = dx * dx + dz * dz;
                if d2_xz > r * r {
                    continue;
                }
                // order is d2_xz first, then height (Y) distance
                let key = d2_xz * 100 + dy.abs(); // mul 100 is to make d2_xz far more important than dy.abs()
                v.push((key, IVec3::new(dx, dy, dz)));
            }
        }
    }
    // v.sort_by_key(|(key, _)| -key); // farthest first
    v.sort_by_key(|(key, _)| *key); // closest first
    keep_offsets.0 = v.into_iter().map(|(_, o)| o).collect();
}
