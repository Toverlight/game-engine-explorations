use bevy::{
    asset::RenderAssetUsages,
    ecs::relationship::RelatedSpawnerCommands,
    image::{ImageFilterMode, ImageSampler, ImageSamplerDescriptor},
    input::mouse::{MouseMotion, MouseWheel},
    prelude::*,
    render::render_resource::*,
    ui::RelativeCursorPosition,
};

use crate::debug::{components::*, resources::*};
use crate::{ui::prelude::*, world::prelude::*};

pub fn setup_debug_map(
    mut commands: Commands,
    mut images: ResMut<Assets<Image>>,
    state: Res<DebugMapState>,
) {
    let res = state.resolution;
    let mut pixel_data = vec![0u8; (res.x * res.y * 4) as usize];

    for y in 0..res.y {
        for x in 0..res.x {
            let index = ((y * res.x + x) * 4) as usize;

            if x % 16 == 0 || y % 16 == 0 {
                pixel_data[index] = 120;
                pixel_data[index + 1] = 120;
                pixel_data[index + 2] = 120;
                pixel_data[index + 3] = 255;
            } else {
                pixel_data[index] = 0;
                pixel_data[index + 1] = 0;
                pixel_data[index + 2] = 0;
                pixel_data[index + 3] = 180;
            }
        }
    }

    let mut image = Image::new(
        Extent3d {
            width: res.x,
            height: res.y,
            depth_or_array_layers: 1,
        },
        TextureDimension::D2,
        pixel_data,
        TextureFormat::Rgba8UnormSrgb,
        RenderAssetUsages::default(),
    );

    image.sampler = ImageSampler::Descriptor(ImageSamplerDescriptor {
        mag_filter: ImageFilterMode::Linear,
        min_filter: ImageFilterMode::Nearest,
        mipmap_filter: ImageFilterMode::Linear,
        ..default()
    });

    let texture_handle = images.add(image);

    commands
        .spawn((
            Node {
                position_type: PositionType::Absolute,
                width: Val::Percent(100.0),
                height: Val::Percent(100.0),
                display: Display::Flex,
                flex_direction: FlexDirection::Row,
                align_items: AlignItems::Center,
                justify_content: JustifyContent::Center,
                column_gap: Val::Px(20.0),
                ..default()
            },
            Visibility::Hidden,
            DebugMapMarker,
        ))
        .with_children(|parent| {
            // left panel
            parent
                .spawn((
                    Node {
                        width: Val::Px(320.0),
                        height: Val::Percent(100.0),
                        flex_direction: FlexDirection::Column,
                        padding: UiRect::all(Val::Px(12.0)),
                        row_gap: Val::Px(8.0),
                        ..default()
                    },
                    BackgroundColor(Color::srgba(0.05, 0.05, 0.08, 0.85)),
                    BorderColor::all(Color::srgb_u8(181, 118, 88)),
                    DebugMapLeftPanel,
                ))
                .with_children(|panel| {
                    build_debug_map_panel_left(panel);
                });

            // map
            parent
                .spawn((
                    Node {
                        flex_grow: 1.0,
                        flex_shrink: 1.0,
                        aspect_ratio: Some(1.0),
                        border: UiRect::all(Val::Px(5.0)),
                        min_width: Val::Px(100.0),
                        min_height: Val::Px(100.0),
                        ..default()
                    },
                    BorderColor::all(Color::srgb_u8(181, 118, 88)),
                    BackgroundColor(Color::NONE),
                ))
                .with_children(|img_parent| {
                    img_parent.spawn((
                        Node {
                            width: Val::Percent(100.0),
                            height: Val::Percent(100.0),
                            ..default()
                        },
                        ImageNode::new(texture_handle),
                        RelativeCursorPosition::default(),
                        DebugMapImage,
                    ));
                });

            // right panel
            parent
                .spawn((
                    Node {
                        width: Val::Px(320.0),
                        height: Val::Percent(100.0),
                        flex_direction: FlexDirection::Column,
                        padding: UiRect::all(Val::Px(12.0)),
                        row_gap: Val::Px(8.0),
                        ..default()
                    },
                    BackgroundColor(Color::srgba(0.05, 0.05, 0.08, 0.85)),
                    BorderColor::all(Color::srgb_u8(181, 118, 88)),
                    DebugMapRightPanel,
                ))
                .with_children(|panel| {
                    build_debug_map_panel_right(panel);
                });
        });
}

fn build_debug_map_panel_left(panel: &mut RelatedSpawnerCommands<'_, ChildOf>) {
    // TODO
    // a title
    panel.spawn((
        Text::new("Debug Panel - Left"),
        TextFont {
            font_size: 20.0,
            ..default()
        },
        Node {
            margin: UiRect::bottom(Val::Px(12.0)),
            ..default()
        },
    ));
    // a "text: button"
    panel
        .spawn((Node {
            flex_direction: FlexDirection::Row,
            align_items: AlignItems::Center,
            justify_content: JustifyContent::SpaceBetween,
            width: Val::Percent(100.0),
            height: Val::Px(40.0),
            ..default()
        },))
        .with_children(|row| {
            row.spawn((
                Text::new("Some Setting:"),
                TextFont {
                    // font: asset_server.load("fonts/CONSTAN.TTF"),
                    font_size: 12.0,
                    ..default()
                },
            ));
            row.spawn((
                Button,
                Node {
                    width: px(60.0),
                    height: px(30.0),
                    border: UiRect::all(px(2.0)),
                    justify_content: JustifyContent::Center,
                    align_items: AlignItems::Center,
                    ..default()
                },
                BackgroundColor(Color::srgb(0.22, 0.22, 0.22)),
                BorderColor::all(Color::WHITE),
            ))
            .with_children(|button_parent| {
                button_parent.spawn((
                    Text::new("LeftBtn"),
                    TextFont {
                        // font: asset_server.load("fonts/CONSTAN.TTF"),
                        font_size: 12.0,
                        ..default()
                    },
                    // TextColor(Color::WHITE),
                ));
            });
        });
    // a seperator
    panel.spawn((
        Node {
            width: Val::Percent(100.0),
            height: Val::Px(2.0),
            margin: UiRect::vertical(Val::Px(6.0)),
            ..default()
        },
        BackgroundColor(Color::srgba(0.6, 0.6, 0.6, 0.3)),
    ));
}

fn build_debug_map_panel_right(panel: &mut RelatedSpawnerCommands<'_, ChildOf>) {
    // TODO
    // a title
    panel.spawn((
        Text::new("Debug Panel - Right"),
        TextFont {
            font_size: 20.0,
            ..default()
        },
        Node {
            margin: UiRect::bottom(Val::Px(12.0)),
            ..default()
        },
    ));
    // a "text: button"
    panel
        .spawn((Node {
            flex_direction: FlexDirection::Row,
            align_items: AlignItems::Center,
            justify_content: JustifyContent::SpaceBetween,
            width: Val::Percent(100.0),
            height: Val::Px(40.0),
            ..default()
        },))
        .with_children(|row| {
            row.spawn((
                Text::new("Some Setting:"),
                TextFont {
                    // font: asset_server.load("fonts/CONSTAN.TTF"),
                    font_size: 12.0,
                    ..default()
                },
            ));
            row.spawn((
                Button,
                Node {
                    width: px(60.0),
                    height: px(30.0),
                    border: UiRect::all(px(2.0)),
                    justify_content: JustifyContent::Center,
                    align_items: AlignItems::Center,
                    ..default()
                },
                BackgroundColor(Color::srgb(0.22, 0.22, 0.22)),
                BorderColor::all(Color::WHITE),
            ))
            .with_children(|button_parent| {
                button_parent.spawn((
                    Text::new("RightBtn"),
                    TextFont {
                        // font: asset_server.load("fonts/CONSTAN.TTF"),
                        font_size: 12.0,
                        ..default()
                    },
                    // TextColor(Color::WHITE),
                ));
            });
        });
    // a seperator
    panel.spawn((
        Node {
            width: Val::Percent(100.0),
            height: Val::Px(2.0),
            margin: UiRect::vertical(Val::Px(6.0)),
            ..default()
        },
        BackgroundColor(Color::srgba(0.6, 0.6, 0.6, 0.3)),
    ));
}

pub fn debug_map_input(
    mut state: ResMut<DebugMapState>,
    ui_state: Res<UiState>,
    mouse_button: Res<ButtonInput<MouseButton>>,
    mut motion: MessageReader<MouseMotion>,
    mut wheel: MessageReader<MouseWheel>,
    map_q: Query<&RelativeCursorPosition, With<DebugMapImage>>,
) {
    if !ui_state.active(UiLayer::DebugMap) {
        return;
    }

    let Ok(rel_cursor) = map_q.single() else {
        return;
    };
    if !rel_cursor.cursor_over() {
        return;
    }

    let rel_cursor = rel_cursor.normalized.unwrap_or(Vec2::ZERO); // (-0.5, -0.5) ~ (0.5, 0.5)
    let cursor_world_before = state.pan
        + (rel_cursor * Vec2::new(state.resolution.x as f32, state.resolution.y as f32)
            / state.zoom);

    if mouse_button.pressed(MouseButton::Left) {
        for ev in motion.read() {
            let delta_world = ev.delta / state.zoom;
            state.pan -= delta_world;
            state.dirty = true;
        }
    }

    for ev in wheel.read() {
        let zoom_factor = 1.1f32.powf(ev.y);
        // let old_zoom = state.zoom;
        state.zoom = (state.zoom * zoom_factor).clamp(0.75, 16.0);

        let cursor_world_after = state.pan
            + (rel_cursor * Vec2::new(state.resolution.x as f32, state.resolution.y as f32)
                / state.zoom);
        state.pan += cursor_world_before - cursor_world_after;

        state.dirty = true;
    }
}

pub fn update_debug_map_visibility(
    ui_state: Res<UiState>,
    mut query: Query<&mut Visibility, With<DebugMapMarker>>,
) {
    if ui_state.is_changed() {
        if let Ok(mut visibility) = query.single_mut() {
            *visibility = if ui_state.visible(UiLayer::DebugMap) {
                Visibility::Visible
            } else {
                Visibility::Hidden
            };
        }
    }
}

pub fn update_debug_map_image(
    mut state: ResMut<DebugMapState>,
    mut images: ResMut<Assets<Image>>,
    ui_state: Res<UiState>,
    image_q: Query<&ImageNode, With<DebugMapImage>>,
    noise: Res<TerrainNoise>,
    registry: Res<ChunkRegistry>,
    chunk_q: Query<&ChunkData>,
    mut height_cache: ResMut<ChunkHeightCache>,
) {
    if !ui_state.active(UiLayer::DebugMap) || !state.dirty {
        return;
    }
    state.dirty = false;

    let Ok(image_node) = image_q.single() else {
        return;
    };
    let Some(image) = images.get_mut(&image_node.image) else {
        return;
    };

    let width = state.resolution.x as usize;
    let height = state.resolution.y as usize;

    let mut data = vec![0u8; width * height * 4];

    let half_size = Vec2::new(width as f32, height as f32) / (2.0 * state.zoom);

    for py in 0..height {
        for px in 0..width {
            let wx = (state.pan.x - half_size.x + (px as f32 / state.zoom)).floor() as i32;
            let wz = (state.pan.y - half_size.y + (py as f32 / state.zoom)).floor() as i32;

            let cx = wx.div_euclid(CS as i32);
            let cz = wz.div_euclid(CS as i32);
            let lx = wx.rem_euclid(CS as i32) as usize;
            let lz = wz.rem_euclid(CS as i32) as usize;

            let est_h = terrain_height(&noise, wx, wz);
            let est_cy = est_h.div_euclid(CS as i32);

            let (mut surface_y, mut loaded): (i32, bool) = (i32::MIN, false);
            for cy in (est_cy - 1..=est_cy + 1).rev() {
                let cp = IVec3::new(cx, cy, cz);
                (surface_y, loaded) = if let Some(hm) = height_cache.0.get(&cp) {
                    // loaded and in cache
                    let local = hm[lx + lz * CS];
                    if local == i32::MIN {
                        continue;
                    } else {
                        (cp.y * CS as i32 + local, true)
                    }
                } else if let Some(entity) = registry.0.get(&cp) {
                    // loaded but not in cache
                    if let Ok(data) = chunk_q.get(*entity) {
                        let hm = data.compute_chunk_surface();
                        let local = hm[lx + lz * CS];
                        let h = if local == i32::MIN {
                            continue;
                        } else {
                            cp.y * CS as i32 + local
                        };
                        height_cache.0.insert(cp, hm);
                        (h, true)
                    } else {
                        (terrain_height(&noise, wx, wz), false) // unreachable theoretically!
                    }
                } else {
                    // unloaded
                    continue;
                };
            }
            if surface_y == i32::MIN {
                surface_y = terrain_height(&noise, wx, wz);
            }

            let color = height_to_color(surface_y, loaded, wx, wz);
            let index = (py * width + px) * 4;
            data[index..index + 4].copy_from_slice(&color);
        }
    }

    image.data = Some(data);
}

fn height_to_color(height: i32, loaded: bool, wx: i32, wz: i32) -> [u8; 4] {
    const SEA_LEVEL: i32 = 0;
    const H_RANGE: i32 = 64; // Height display range: between SEA_LEVEL ± H_RANGE
    const CONTOUR_STEP: i32 = 8;

    // Grey level
    let normalized =
        ((height - SEA_LEVEL + H_RANGE) as f32 / (2.0 * H_RANGE as f32)).clamp(0.0, 1.0);
    let brightness = (normalized * 200.0 + 30.0) as u8; // 30..230
    let mut r = brightness;
    let mut g = brightness;
    let mut b = brightness;

    // Load/Unload brightness distinction
    if loaded {
        let factor = 1.15;
        r = (r as f32 * factor).min(255.0) as u8;
        g = (g as f32 * factor).min(255.0) as u8;
        b = (b as f32 * factor).min(255.0) as u8;
    } else {
        let factor = 0.85;
        r = (r as f32 * factor) as u8;
        g = (g as f32 * factor) as u8;
        b = (b as f32 * factor) as u8;
    }

    // Sea level: blue
    if height == SEA_LEVEL {
        return [20, 80, 220, 255];
    }

    // Contour: green
    if (height - SEA_LEVEL).rem_euclid(CONTOUR_STEP) == 0 {
        let factor = 0.5;
        g = ((g as f32 + 200.0) * factor) as u8;
        r = (r as f32 * factor) as u8;
        b = (b as f32 * factor) as u8;
    }

    // chunk boundary: yellow
    if wx.rem_euclid(CS as i32) == 0 || wz.rem_euclid(CS as i32) == 0 {
        r = (r as f32 * 0.3 + 255.0 * 0.7) as u8;
        g = (g as f32 * 0.3 + 200.0 * 0.7) as u8;
        b = (b as f32 * 0.3) as u8;
    }

    [r, g, b, 255]
}
