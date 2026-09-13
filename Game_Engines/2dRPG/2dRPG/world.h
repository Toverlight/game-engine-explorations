#ifndef WORLD_H
#define WORLD_H

#include "vector2.h"
#include "perlin_noise.h"

#include <vector>
#include <string>
#include <stdlib.h>
#include <graphics.h>

using namespace PerlinNoise;

// 矿物类别
enum class Mineral
{
    Stone,
    Iron,
    Silver,
    Gold,
    Crystal,

    Num
};

// 世界块
struct WorldBlock
{
    float humidity;                             // 空气湿度
    float moisture;                             // 土壤水分
    float vegetation;                           // 植被覆盖度
    int herbivores;                             // 食草动物数量
    int carnivores;                             // 食肉动物数量
    float mineralContent[(int)Mineral::Num];    // 各种矿物含量

    float rainfall;                             // 单位时间降雨量
    float temperature;                          // 气温

    int population;                             // 人口数量
    float assarticDegree;                       // 开垦程度

    WorldBlock() : humidity(0), moisture(0), vegetation(0), herbivores(0), carnivores(0),
        mineralContent{0}, rainfall(0),temperature(0), population(0), assarticDegree(0) {}
    ~WorldBlock() = default;
};

class World
{
public:
    World() : elevations_p(nullptr) {}
    World(Vector2 size, std::string name) : size(size), name(name)
    {
        elevations_p = nullptr;
        blocks.resize(size.y);
        for (int i = 0; i < size.y; i++)
        {
            blocks[i].resize(size.x);
        }

    }
    ~World() = default;

    void draw()
    {

    }

    void set_elevations_p(std::vector<std::vector<float>>* elevations_p)
    {
        this->elevations_p = elevations_p;
    }
    auto get_elevations_p()
    {
        return elevations_p;
    }

private:
    const int gridLen = 20;
    const int blockLen = 10;

    Vector2 size;
    std::string name;
    std::vector<std::vector<WorldBlock>> blocks;
    std::vector<std::vector<float>>* elevations_p;


};

#endif // !WORLD_H