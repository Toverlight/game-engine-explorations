# Principle of World-gen

## Preface

To make a terrain vivid, various noises are fundamental, instead of any single one.

For natural terrain, noises appliable: 

1. **Basic noises**
2. **Fractal addition**
3. **Coordinate distortion**
4. **Nonlinear mapping**

What natures a "noise" should possess:

1. **Continuity**: Output smooth changes at adjacent sampling points
2. **Multi-scale structure**: There are both slow changes (continents, climate zones) and fast changes (small hills, rock walls, caves)
3. **Controllable spectrum**: You can control "roughness" by tweaking frequency and amplitude
4. **Reproducibility**: A seed decides an output

## A. Basic Noises

### a. Value noise

**Idea**: 

Put random scalar values on integer grid points, then do smooth interpolation between grid points.

**Steps**:

1. Sampling point `p=(x,y,z)` falls in a grid, finds `2^D` grid values around itself
2. Interpolate along each axis (linear or smoother curves)
3. Get the continuous output

**Advantages**:

1. Simple.
2. Suitable for "slowly changing parameter field" (2D field: temperature, humidity, continentalness and etc.)

**Disadvantages**:

1. Directionality/lattice feeling is more obvious

### b. Gradient noise

#### b1. Perlin

**Idea**: 

Place a "pseudo-random gradient vector" on a grid point. The value of the points inside the grid is determined by the product of the "displacement vector from point to grid" and the "gradient vector," then interpolate.

**Steps**:

1. For each point `g`: gradient `G(g)` (select one from a fixed small collection, using hash/replacement table)
2. For sampling point `p`: compute `d = p - g`
3. Contribution value: `dot(G(g), d)` (the closer to the grid point, the less contribution it makes; direction determines the sign (positive or negative))
4. Perform smooth interpolation on the contributions from 2^D grid points (the fade curve is crucial).

**Advantages**:

1. Continuous, visually natural; very common for terrain detail.

**Disadvantages**:

1. Classic 3D Perlin is relatively slower and prone to slight deviations in certain directions (implementation/gradient collection issues).

#### b2. Simplex / OpenSimplex

**Idea**: 

Divide the space into simple shapes (2D triangles, 3D tetrahedrons) instead of cubic grids, reducing the number of corner points that need to be mixed.

**Steps**:

TODO

**Advantages**:

1. Performance improves at higher dimensions, with fewer lattice artifacts.
2. 3D/4D versions are generally more user-friendly than the classic Perlin implementation.

**Disadvantages**:

1. Math/Implementation is more complex.

### c. Worley/Cellular

**Idea**:

Scatter “feature points” throughout the space, and for each sampled point, calculate the distance to the nearest or second-nearest feature point (or a combination of both).

- Common output values: F1 (nearest distance), F2 (second-nearest distance), F2 - F1, etc.
- Visually like "cells/cracks/pebbles"

**Steps**:

TODO

**Advantages**:

1. “Block-like zoning” at the boundaries of biological communities
2. Texture and friability of the rock layers
3. Mixing with gradient noise to create richer surface noise

## B. Combination technologies

### a. Fractional Noise

#### a1. fBm (Fractal Brownian Motion)

#### a2. Turbulence / Abs noise

#### a3. Ridged multifractal

### b. Domain Wrapping

## C. Nonlinear Mapping - Six Major Parameter Spaces

### Continentalness

### Erosion

### Peaks/Valleys

### Temperature

### Humidity

### Weirdness
