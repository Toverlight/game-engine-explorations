// use bevy::prelude::*;
use super::resources::*;
use fastnoise_lite::*;

/// mock only
// pub fn sine_noise_height(wx: i32, wz: i32) -> i32 {
//     let h = (((wx as f32 * 0.02).sin() * 3.0
//         + (wz as f32 * 0.015).cos() * 2.0
//         + ((wx + wz) as f32 * 0.01).sin() * 1.5)
//         .round() as i32
//         + 2)
//     .max(0);
//     h
// }

pub fn make_noise() -> FastNoiseLite {
    let mut n = FastNoiseLite::new();

    n.set_seed(Some(42)); // FIXME: change this later
    n.set_noise_type(Some(NoiseType::OpenSimplex2));
    n.set_frequency(Some(0.008));
    n.set_fractal_type(Some(FractalType::FBm));
    n.set_fractal_octaves(Some(4));
    n.set_fractal_lacunarity(Some(2.0));
    n.set_fractal_gain(Some(0.5));
    n
}

pub fn terrain_height(noise: &TerrainNoise, wx: i32, wz: i32) -> i32 {
    let raw = noise.0.get_noise_2d(wx as f32, wz as f32); // [-1, 1]
    (raw * 10.0).round() as i32
}
