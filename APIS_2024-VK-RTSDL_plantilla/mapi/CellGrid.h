#pragma once
#include "common.h"
#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <string_view>

typedef struct cell {
	int init;
	int end;
}cell;


template <typename T>
class CellGrid {

private:
	typedef struct  {
		int cellId;
		glm::vec3 pos;
		T object;
	}cellObject;
	std::map<int,cell> cells; //inicio/final de celdas
	std::vector<cellObject> objects; //lista de objetos, ordenados por ID de celda
	glm::vec3 cellSize; //tamaño de celda
	glm::ivec3 cellListSize; //número de celdas en X,Y,Z
	glm::vec3 min; //coordenadas mínimas
	glm::vec3 max; //coordenadas máximas
public:
	CellGrid() {};
	CellGrid(glm::vec3 min, glm::vec3 max, glm::vec3 cellSize);;

	void init(glm::vec3 min, glm::vec3 max, glm::vec3 cellSize, int maxObjects);;


	int computeCellId(glm::vec3 pos);
	void addObject(T object, glm::vec3 pos);
	void update();
	std::vector<T> getNNearestObjects(glm::vec3 pos, int n, float dist);
};


template<typename T>
inline CellGrid<T>::CellGrid(glm::vec3 min, glm::vec3 max, glm::vec3 cellSize) :min(min), max(max), cellSize(cellSize)
{
	glm::vec3 size = max - min;
	cellListSize = glm::ceil(size / cellSize);
	cellListSize += glm::ivec3(1);
	cells.resize(cellListSize.x * cellListSize.y * cellListSize.z);
}

template<typename T>
inline void CellGrid<T>::init(glm::vec3 min, glm::vec3 max, glm::vec3 cellSize, int maxObjects)
{
	this->min = min; this->max = max; this->cellSize = cellSize;
	glm::vec3 size = max - min;
	cellListSize = glm::ceil(size / cellSize);
	cellListSize += glm::ivec3(1);
	//cells.resize(cellListSize.x * cellListSize.y * cellListSize.z);
	objects.reserve(maxObjects);
	objects.resize(0);
}

template<typename T>
inline int CellGrid<T>::computeCellId(glm::vec3 pos)
{
	glm::ivec3 cellPos = (pos - min) / cellSize;


	if (((cellPos.x >= cellListSize.x) || cellPos.x  < 0) ||
		((cellPos.y >= cellListSize.y) || cellPos.y  < 0) ||
		((cellPos.z >= cellListSize.z) || cellPos.z  < 0))
		std::cout << "ERROR cellList\n";
	return (cellPos.z * cellListSize.y + cellPos.y) * cellListSize.x + cellPos.x;
}

template<typename T>
inline void CellGrid<T>::addObject(T object, glm::vec3 pos)
{
	cellObject obj;
	obj.cellId = computeCellId(pos);
	obj.object = object;
	obj.pos = pos;
	objects.push_back(obj);
}

template<typename T>
inline void CellGrid<T>::update()
{
	struct
	{
		bool operator()(cellObject a, cellObject b) const { return a.cellId < b.cellId; }
	}customLess;

	std::sort(objects.begin(), objects.end(), customLess);

	//memset(cells.data(), 0, sizeof(cell)* cells.size());
	cells.clear();
	int size = objects.size();
	//cells[0].init = 0;
	//cells[cells.size()-1].end = size;

	int postCellId = 0;
	int cellId = objects[0].cellId;
	cells[cellId].init = 0;

	for (int i = 0; i<size - 1; i++)
	{
		postCellId = objects[i + 1].cellId;
		cellId = objects[i].cellId;

		if (postCellId != cellId)
		{
			cells[postCellId].init = i + 1;
			cells[cellId].end = i;
		}
	}
	cellId = objects[size - 1].cellId;
	cells[cellId].end = size;
}

template<typename T>
inline std::vector<T> CellGrid<T>::getNNearestObjects(glm::vec3 pos, int n, float dist)
{
	glm::ivec3 cellPos = (pos - min) / cellSize;
	std::map<float, T> dists;
	std::vector<T> res;


	glm::ivec3 cellMax = (pos + glm::vec3(dist) - min) / cellSize;
	glm::ivec3 cellMin = (pos - glm::vec3(dist) - min) / cellSize;

	cellMax.x = cellMax.x < 0 ? 0 : cellMax.x;
	cellMax.y = cellMax.y < 0 ? 0 : cellMax.y;
	cellMax.z = cellMax.z < 0 ? 0 : cellMax.z;
	cellMin.x = cellMin.x < 0 ? 0 : cellMin.x;
	cellMin.y = cellMin.y < 0 ? 0 : cellMin.y;
	cellMin.z = cellMin.z < 0 ? 0 : cellMin.z;

	cellMax.x = cellMax.x > cellListSize.x ? cellListSize.x : cellMax.x;
	cellMax.y = cellMax.y > cellListSize.y ? cellListSize.y : cellMax.y;
	cellMax.z = cellMax.z > cellListSize.z ? cellListSize.z : cellMax.z;
	cellMin.x = cellMin.x > cellListSize.x ? cellListSize.x : cellMin.x;
	cellMin.y = cellMin.y > cellListSize.y ? cellListSize.y : cellMin.y;
	cellMin.z = cellMin.z > cellListSize.z ? cellListSize.z : cellMin.z;

	for (int x = cellMin.x; x <= cellMax.x; x++)
		for (int y = cellMin.y; y <= cellMax.y; y++)
			for (int z = cellMin.z; z <= cellMax.z; z++)
			{
				if (((x >= cellListSize.x) || x < 0) ||
					((y >= cellListSize.y) || y < 0) ||
					((z >= cellListSize.z) || z < 0))
					continue;

				int cellId = (z * cellListSize.y + y) * cellListSize.x + x;
				auto cellFound = cells.find(cellId);
				if (cellFound != cells.end())
				{
					cell c = cellFound->second;

					for (int i = c.init; i < c.end; i++)
					{
						T obj = objects[i].object;
						float objDist = glm::distance(obj.pos, pos);
						if (objDist <= dist)
							dists[objDist] = (obj);
					}
				}
			}
	int count = 0;
	for (auto it = dists.begin(); it != dists.end() && count < n; it++)
	{
		res.push_back(it->second);
		count++;
	}
	return res;
}
