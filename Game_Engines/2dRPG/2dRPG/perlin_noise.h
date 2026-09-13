#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include "vector2.h"

#include <vector>
#include <cmath>

namespace PerlinNoise
{
	// 平滑插值函数，Perlin噪声常用
	// (ease curve, 定义域: [0, 1], 值域: [0, 1])
	inline float fade(float t) {
		return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3
	}

	// 线性插值
	inline float lerp(float t, float a, float b) {
		return a + t * (b - a);
	}

	// 点积计算，简单的向量点积
	inline float dotGridGradient(int ix, int iy, int sideX, int sideY, float x, float y,
		std::vector<std::vector<Vector2>>& gradient) {
		// 从格点到当前点的向量
		float dx = x - (float)sideX;
		float dy = y - (float)sideY;
		// 格点的梯度向量
		float gradX = gradient[iy][ix].x;
		float gradY = gradient[iy][ix].y;
		// 计算点积
		return (dx * gradX + dy * gradY);
	}

	// 初始化格点梯度向量（随机生成每个格点的梯度向量）
	std::vector<std::vector<Vector2>> generateGradient(float x_num, float y_num)
	{
		std::vector<std::vector<Vector2>> gradient(y_num, std::vector<Vector2>(x_num));

		for (int i = 0; i < y_num; ++i) {
			for (int j = 0; j < x_num; ++j) {
				float angle = (float(rand()) / RAND_MAX) * 2.0f * 3.14159f; // 随机方向
				gradient[i][j] = Vector2(cos(angle), sin(angle)); // 单位向量
			}
		}

		return gradient;
	}

	// Perlin噪声生成器(生成值∈(-1， 1))
	inline float perlinNoise(float x, float y, int gridLen, std::vector<std::vector<Vector2>>& gradient)
	{
		// 找到包含(x, y)的网格单元的四个角点
		int x0i = (int)floor(x / gridLen);
		int x1i = x0i + 1;
		int y0i = (int)floor(y / gridLen);
		int y1i = y0i + 1;
		int x0 = x0i * gridLen;
		int x1 = x1i * gridLen;
		int y0 = y0i * gridLen;
		int y1 = y1i * gridLen;

		// 从四个角点分别计算梯度点积
		float xf = (x - (float)x0) / gridLen;
		float yf = (y - (float)y0) / gridLen;
		float sx = fade(xf); // 插值参数，x方向
		float sy = fade(yf); // 插值参数，y方向

		float n0, n1, ix0, ix1, value;
		n0 = dotGridGradient(x0i, y0i, x0, y0, x, y, gradient);
		n1 = dotGridGradient(x1i, y0i, x1, y0, x, y, gradient);
		ix0 = lerp(sx, n0, n1);

		n0 = dotGridGradient(x0i, y1i, x0, y1, x, y, gradient);
		n1 = dotGridGradient(x1i, y1i, x1, y1, x, y, gradient);
		ix1 = lerp(sx, n0, n1);

		value = lerp(sy, ix0, ix1) * sqrt(2.0f) / gridLen;		// 最终插值结果

		return value;
	}
	// 倍频柏林噪声生成器(生成值∈(-1， 1))
	float octavePerlinNoise(float x, float y, int gridLen, std::vector<std::vector<Vector2>>& gradient, 
		int octaves, float persistence)
	{
		float total = 0.0f;
		float frequency = 1.0f;
		float amplitude = 1.0f;
		float maxValue = 0.0f;

		for (int i = 0; i < octaves; ++i) {
			total += perlinNoise(x * frequency, y * frequency, gridLen, gradient) * amplitude;
			maxValue += amplitude;
			frequency *= 2.0f;
			amplitude *= persistence;
		}

		return total / maxValue;
	}

	// 侵蚀函数
	// iterations为侵蚀条数
	// erosionFactor取值在 0 到 0.5 之间正常
	void erodeTerrain(std::vector<std::vector<float>>& elevations, int iterations, float erosionFactor, 
		int srcElevationLowest, int srcElevationHighest)
	{
		//float diagErosionFactor = (1.0f / sqrtf(2.0f)) * erosionFactor;	// 用于减小对四角的侵蚀影响

		int numY = elevations.size();
		int numX = elevations[0].size();

		int x = -1, y = -1;
		int drawLimit = 100;			// 如果一直抽不到（抽取次数达到上限）满足高度范围的起始点，则跳过该条侵蚀

		for (int i = 0; i < iterations; ++i)
		{
			bool isDrawSuccessful = false;
			// 随机选择一个点作为水流起点
			for (int k = 0; k < drawLimit; ++k)
			{
				int tempX = rand() % numX;
				int tempY = rand() % numY;
				if (elevations[tempY][tempX] >= srcElevationLowest &&
					elevations[tempY][tempX] <= srcElevationHighest)
				{
					x = tempX;
					y = tempY;
					isDrawSuccessful = true;
					break;
				}
			}
			if (!isDrawSuccessful) continue;

			// 获取当前点的海拔高度
			float currentElevation = elevations[y][x];
			// 一条河流的侵蚀
			while (currentElevation >= 0)
			{
				// 定义邻居方向（上下左右、四角，共八方向）
				int dx[] = { 0, 1, 0, -1, -1, 1, 1, -1 };
				int dy[] = { -1, 0, 1, 0, -1, -1, 1, 1 };

				int lastUpdatedIndex = -1;
				// 找到最低邻居并流向它
				int lowestNeighborX = x;
				int lowestNeighborY = y;
				float lowestNeighborElevation = currentElevation;

				for (int dir = 0; dir < 8; ++dir)
				{
					int nx = x + dx[dir];
					int ny = y + dy[dir];

					// 确保邻居在边界内
					if (nx >= 0 && nx < numX && ny >= 0 && ny < numY)
					{
						if (elevations[ny][nx] < lowestNeighborElevation)
						{
							lowestNeighborElevation = elevations[ny][nx];
							lowestNeighborX = nx;
							lowestNeighborY = ny;

							lastUpdatedIndex = dir;
						}
					}
				}
				if (lastUpdatedIndex == -1)		// 极端情况：自己就是周围一圈中的最低点
				{
					break;
				}

				// 进行侵蚀：将一部分高度从当前点转移到最低邻居
				//float transferAmount = (lastUpdatedIndex < 4 ?
				//	(currentElevation - lowestNeighborElevation) * erosionFactor :
				//	(currentElevation - lowestNeighborElevation) * erosionFactor
				//	);
				float transferAmount = (currentElevation - lowestNeighborElevation) * erosionFactor;

				// 更新海拔值
				elevations[y][x] -= transferAmount;
				elevations[lowestNeighborY][lowestNeighborX] += transferAmount;

				// 将最低邻居的坐标更新到 x, y，继续新一轮循环，直至河流末端到达最低点或低于 0 海拔
				x = lowestNeighborX;
				y = lowestNeighborY;

				currentElevation = elevations[y][x];
			}
		}
	}

}

#endif // !PERLIN_NOISE_H

