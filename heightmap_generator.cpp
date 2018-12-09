#include "heightmap_generator.hpp"

#include "noise/noise.h"
#include "noise/noiseutils.h"

HeightmapGenerator::HeightmapGenerator() {
    configure();
}
void HeightmapGenerator::configure() {
	baseFlatTerrain.SetFrequency (2.0);
	flatTerrain.SetSourceModule (0, baseFlatTerrain);
	flatTerrain.SetScale (0.125);
	flatTerrain.SetBias (-0.75);
	terrainType.SetFrequency (0.5);
	terrainType.SetPersistence (0.25);
	terrainType.SetOctaves(5);
	terrainType.SetFractal(true);
	finalTerrain.SetSourceModule (0, flatTerrain);
	finalTerrain.SetSourceModule (1, mountainTerrain);
	finalTerrain.SetControlModule (terrainType);
	finalTerrain.SetBounds (0.0, 1000.0);
	finalTerrain.SetEdgeFalloff (0.125);
	heightMapBuilder.SetSourceModule (finalTerrain);
	heightMapBuilder.SetDestNoiseMap (heightMap);
}

void HeightmapGenerator::set_bounds(int resolution, float min, float max) {
	heightMapBuilder.SetDestSize (resolution, resolution);
	heightMapBuilder.SetBounds (min, max, min, max);
	heightMapBuilder.Build ();
}

float HeightmapGenerator::compute_height(float x, float y) {
    return heightMap.GetValue(x, y);
}