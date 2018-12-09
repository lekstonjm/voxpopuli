#pragma once
#include "noise/noise.h"
#include "noise/noiseutils.h"


struct HeightmapGenerator {
    HeightmapGenerator();
    void configure();
    void set_bounds(int resolution, float min, float max);
    float compute_height(float x, float y);
	noise::module::RidgedMulti mountainTerrain;
	noise::module::Billow baseFlatTerrain;
	noise::module::ScaleBias flatTerrain;
	noise::module::Simplex terrainType;
	noise::module::Select finalTerrain;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
};
