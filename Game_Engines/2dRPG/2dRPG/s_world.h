#ifndef S_WORLD_H
#define S_WORLD_H

#include <vector>

struct SWorldBlock
{
	int animalNum;				// 动物数量
	int plantNum;				// 植物数量
	int cHumanNum;				// 文明人数量
	int rHumanNum;				// 野蛮人数量
	int waterVolume;			// 水量
	int mineralVolume;			// 矿量

	SWorldBlock() : animalNum(0), plantNum(0), cHumanNum(0), rHumanNum(0), waterVolume(0), 
		mineralVolume(0) {}
	SWorldBlock(int a, int p, int c, int r, int w, int m):
		animalNum(a), plantNum(p), cHumanNum(c), rHumanNum(r), waterVolume(w),
		mineralVolume(m) {}
};

class SWorld
{
public:
	SWorld(std::vector<std::vector<float>>& elevations)
	{
		swbs.resize(elevations.size());
		for (int i = 0; i < elevations.size(); ++i)
		{
			swbs[i].resize(elevations[0].size());
		}
	}

	void draw()
	{

	}

	void init(std::vector<std::vector<float>>& elevations)
	{
		for (int i = 0; i < elevations.size(); ++i)
		{
			for (int j = 0; j < elevations[0].size(); ++j)
			{
				if (elevations[i][j] <= 0)
				{
					swbs[i][j].waterVolume = 100;
					swbs[i][j].mineralVolume = (int)(elevations[i][j] * (-0.1f));
				}
				else if (elevations[i][j] <= 1000)
				{
					swbs[i][j].waterVolume = ((rand() / RAND_MAX) * 30);	// 0,30
					swbs[i][j].mineralVolume = (int)((elevations[i][j] * 0.05f) +
						(rand() / RAND_MAX) * 5);								// 0,30
					swbs[i][j].plantNum = 10 + swbs[i][j].waterVolume * 5 +
						swbs[i][j].mineralVolume * 2;		// 10,215
					swbs[i][j].animalNum = 1 + (int)roundf(swbs[i][j].plantNum * 0.1f);// 2,16
					swbs[i][j].cHumanNum = 0;
					swbs[i][j].rHumanNum = 2 + (int)roundf(swbs[i][j].plantNum * 0.05f +
						swbs[i][j].animalNum * 0.2f);								
				}
			}
		}
	}

	void clear()
	{
		SWorldBlock swb_empty;
		for (int i = 0; i < swbs.size(); ++i)
		{
			for (int j = 0; j < swbs[0].size(); ++j)
			{
				swbs[i][j] = swb_empty;
			}
		}
	}

private:
	std::vector<std::vector<SWorldBlock>> swbs;
};

#endif // !S_WORLD_H