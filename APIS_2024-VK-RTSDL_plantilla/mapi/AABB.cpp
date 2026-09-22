#include "AABB.h"
#include "Mesh.h"
#include <utility>

void AABBColls::addMesh(std::string name, Mesh* mesh, Material* mat)
{
	this->name = name;

	this->mat = mat;
	//isRoot = true;
	vertices = mesh->vertexList;
	transfVertices.resize(vertices.size());
	std::vector<unsigned int> &vIdx = mesh->getTriangleIdxList();

	//lista de triangulos originales (sin transformaciones)
	//guardamos una copia para transformaciones
	std::copy(vertices.begin(), vertices.end(), transfVertices.begin());
	for (auto i= vIdx.begin();i!=vIdx.end();)
	{
		triangleIdxs_t tr;
		tr.v0 = *i;
		i++;
		tr.v1 = *i;
		i++;
		tr.v2 = *i;
		i++;
		triangles.push_back(tr);
	}
	//lista de triángulos multiplicados por matrices de transformacion	
	// 
	AABB_t root=newAABB();
	root.id = 0;
	trianglesPerAABB[root.id].resize(triangles.size());
	int cont = 0;
	for (auto i = triangles.begin(); i != triangles.end();i++,cont++)
	{
		trianglesPerAABB[root.id][cont]=cont;
	}
	AABBs.push_back(root);
	resize(root.id);
	subdivide(root.id);

}

void AABBColls::resize(int boxId) {
	//check bounds
	AABB_t& box = AABBs[boxId];
	box.minB = glm::vec3(FLOAT_MAX, FLOAT_MAX, FLOAT_MAX);
	box.maxB = glm::vec3(FLOAT_MIN, FLOAT_MIN, FLOAT_MIN);
	for (auto i: trianglesPerAABB[box.id])
	{
		triangleIdxs_t t = triangles[i];
		for (int vt = 0; vt < 3; vt++)
		{
			glm::vec4 v = transfVertices[t.v[vt]].pos;
			if (v.x < box.minB.x)	box.minB.x = v.x;
			if (v.y < box.minB.y)	box.minB.y = v.y;
			if (v.z < box.minB.z)	box.minB.z = v.z;
			if (v.x > box.maxB.x)	box.maxB.x = v.x;
			if (v.y > box.maxB.y)	box.maxB.y = v.y;
			if (v.z > box.maxB.z)	box.maxB.z = v.z;
		}
	}
}


void AABBColls::resizeRec(int boxId) {
	AABB_t& box = AABBs[boxId];
	resize(boxId);
	for (int i = 0; i < box.nSons; i++)
	{
		resizeRec(box.sons[i]);
	}
}



void AABBColls::subdivide(int boxId)
{
	//AABB_t& box = AABBs[boxId];
	
	if (trianglesPerAABB[boxId].size() > 1)
	{
		AABB_t subAABB[8];
		std::vector<int> triIds[8];
		memset(subAABB, 0, sizeof(AABB_t) * 8);
		int cont = 0;
		glm::vec3 step = glm::vec3((AABBs[boxId].maxB.x - AABBs[boxId].minB.x) / 2.0f, (AABBs[boxId].maxB.y - AABBs[boxId].minB.y) / 2.0f, (AABBs[boxId].maxB.z - AABBs[boxId].minB.z) / 2.0f);

		glm::vec3 margin = step / 2.0f;
		if (step.x == 0) step.x = 0.0001f;
		if (step.y == 0) step.y = 0.0001f;
		if (step.z == 0) step.z = 0.0001f;
		for (int i = 0; i < 8; i++)
		{
			subAABB[i] = newAABB();
			//subAABB[i]->mat = mat;
		}
		//dlb
		subAABB[0].minB.x = AABBs[boxId].minB.x;
		subAABB[0].minB.y = AABBs[boxId].minB.y;
		subAABB[0].minB.z = AABBs[boxId].minB.z;
		subAABB[0].maxB.x = AABBs[boxId].minB.x + step.x;
		subAABB[0].maxB.y = AABBs[boxId].minB.y + step.y;
		subAABB[0].maxB.z = AABBs[boxId].minB.z + step.z;
		//drb
		subAABB[1].minB.x = AABBs[boxId].minB.x + step.x;
		subAABB[1].minB.y = AABBs[boxId].minB.y;
		subAABB[1].minB.z = AABBs[boxId].minB.z;
		subAABB[1].maxB.x = AABBs[boxId].maxB.x;
		subAABB[1].maxB.y = AABBs[boxId].minB.y + step.y;
		subAABB[1].maxB.z = AABBs[boxId].minB.z + step.z;
		//ulb
		subAABB[2].minB.x = AABBs[boxId].minB.x;
		subAABB[2].minB.y = AABBs[boxId].minB.y + step.y;
		subAABB[2].minB.z = AABBs[boxId].minB.z;
		subAABB[2].maxB.x = AABBs[boxId].minB.x + step.x;
		subAABB[2].maxB.y = AABBs[boxId].maxB.y;
		subAABB[2].maxB.z = AABBs[boxId].minB.z + step.z;
		//urb
		subAABB[3].minB.x = AABBs[boxId].minB.x + step.x;
		subAABB[3].minB.y = AABBs[boxId].minB.y + step.y;
		subAABB[3].minB.z = AABBs[boxId].minB.z;
		subAABB[3].maxB.x = AABBs[boxId].maxB.x;
		subAABB[3].maxB.y = AABBs[boxId].maxB.y;
		subAABB[3].maxB.z = AABBs[boxId].minB.z + step.z;
		//dlf
		subAABB[4].minB.x = AABBs[boxId].minB.x;
		subAABB[4].minB.y = AABBs[boxId].minB.y;
		subAABB[4].minB.z = AABBs[boxId].minB.z + step.z;
		subAABB[4].maxB.x = AABBs[boxId].minB.x + step.x;
		subAABB[4].maxB.y = AABBs[boxId].minB.y + step.y;
		subAABB[4].maxB.z = AABBs[boxId].maxB.z;
		//drf
		subAABB[5].minB.x = AABBs[boxId].minB.x + step.x;
		subAABB[5].minB.y = AABBs[boxId].minB.y;
		subAABB[5].minB.z = AABBs[boxId].minB.z + step.z;
		subAABB[5].maxB.x = AABBs[boxId].maxB.x;
		subAABB[5].maxB.y = AABBs[boxId].minB.y + step.y;
		subAABB[5].maxB.z = AABBs[boxId].maxB.z;
		//ulf
		subAABB[6].minB.x = AABBs[boxId].minB.x;
		subAABB[6].minB.y = AABBs[boxId].minB.y + step.y;
		subAABB[6].minB.z = AABBs[boxId].minB.z + step.z;
		subAABB[6].maxB.x = AABBs[boxId].minB.x + step.x;
		subAABB[6].maxB.y = AABBs[boxId].maxB.y;
		subAABB[6].maxB.z = AABBs[boxId].maxB.z;
		//urf
		subAABB[7].minB.x = AABBs[boxId].minB.x + step.x;
		subAABB[7].minB.y = AABBs[boxId].minB.y + step.y;
		subAABB[7].minB.z = AABBs[boxId].minB.z + step.z;
		subAABB[7].maxB.x = AABBs[boxId].maxB.x;
		subAABB[7].maxB.y = AABBs[boxId].maxB.y;
		subAABB[7].maxB.z = AABBs[boxId].maxB.z;

		//for (auto i = transfTriangles.begin(); i != transfTriangles.end(); i++)
		for (auto i : trianglesPerAABB[AABBs[boxId].id])
		{
			triangleIdxs_t t = triangles[i];

			glm::dvec3 p;//center of this triangle
			glm::dvec3 v0 = transfVertices[t.v[0]].pos;
			glm::dvec3 v1 = transfVertices[t.v[1]].pos;
			glm::dvec3 v2 = transfVertices[t.v[2]].pos;
			p.x = (v0.x + v1.x + v2.x) / 3.0;
			p.y = (v0.y + v1.y + v2.y) / 3.0;
			p.z = (v0.z + v1.z + v2.z) / 3.0;

			bool salir = false;
			int bb = 0;
			while (!salir && (bb < 8))
			{
				if (testCollisionPoint(subAABB[bb],p))
				{
					//subAABB[bb]->transfTriangles.push_back(*i);
					triIds[bb].push_back(i);
					salir = 1;
				}
				bb++;
			}
			if (!salir)
			{
				std::cout << "ERROR AABB 113\n";
			}
		}
		for (int i = 0; i < 8; i++)
		{
			if ((triIds[i].size() > 0) && (triIds[i].size() != trianglesPerAABB[boxId].size()))
			{
				subAABB[i].id =(int) AABBs.size();
				AABBs.push_back(subAABB[i]);
				trianglesPerAABB[subAABB[i].id] = triIds[i];
				AABBs[boxId].sons[AABBs[boxId].nSons] = subAABB[i].id;
				AABBs[boxId].nSons++;
				//subAABBs.push_back(subAABB[i]);
			}
		}

		for (auto i = 0;i<AABBs[boxId].nSons;i++)
		{
			resize(AABBs[boxId].sons[i]);
			subdivide(AABBs[boxId].sons[i]);
		}
	}
}

inline bool AABBColls::testCollisionPoint(AABB_t box,glm::vec3 p)
{
	return (p.x >= box.minB[0] && p.x <= box.maxB[0]) &&
		(p.y >= box.minB[1] && p.y <= box.maxB[1]) &&
		(p.z >= box.minB[2] && p.z <= box.maxB[2]);
}

void AABBColls::update(glm::mat4 matrixTransf)
{


	glm::mat4 IVM = glm::inverse(glm::transpose(matrixTransf));	
#pragma omp parallel for num_threads(4)
	for(int i=0;i<vertices.size();i++)
	{
		transfVertices[i].pos = matrixTransf * vertices[i].pos;
		transfVertices[i].normal = glm::vec4(glm::normalize(glm::vec3(IVM * vertices[i].normal)),0.0f);

	}

	resizeRec(0);
	//subdivide();

}


bool AABBColls::testCollisionAABB(int boxId,collisionRay_t r)
{

	AABB_t& box = AABBs[boxId];

	float tmin = (box.minB.x - r.origin.x) / r.dir.x;
	float tmax = (box.maxB.x - r.origin.x) / r.dir.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (box.minB.y - r.origin.y) / r.dir.y;
	float tymax = (box.maxB.y - r.origin.y) / r.dir.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (box.minB.z - r.origin.z) / r.dir.z;
	float tzmax = (box.maxB.z - r.origin.z) / r.dir.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;

}


collision_t AABBColls::getCollisions(collisionRay_t ray, int boxId = 0)
{
	AABB_t& box = AABBs[boxId];
	collision_t collisionOut;
	collisionOut.distance = FLOAT_MAX;
	bool coll = testCollisionAABB(boxId,ray);
	if (coll) {
		if (box.nSons > 0)
		{
			for (auto i =0;i< box.nSons;i++)
			{
				collision_t collision = getCollisions(ray, box.sons[i]);

				if (collision.distance < collisionOut.distance)
					collisionOut = (collision);
			}
		}
		else
		{
			collision_t collision;
			std::vector<int>& triangles = trianglesPerAABB[box.id];
			for (auto i = triangles.begin(); i != triangles.end(); i++)
			{
				if (triangleTestRay((*i), mat, ray, collision))
				{
					if((collision.distance>0)&& collision.distance < collisionOut.distance)
						collisionOut = (collision);
				}
			}
		}
	}
	return collisionOut;
}



bool AABBColls::triangleTestRay(int triangleId, Material* mat, collisionRay_t ray, collision_t& collision)
{

	constexpr float kEpsilon = 1e-8f;
	auto triangle = triangles[triangleId];

	vertex_t vt0 = transfVertices[triangle.v0];
	vertex_t vt1 = transfVertices[triangle.v1];
	vertex_t vt2 = transfVertices[triangle.v2];
	// compute plane's normal
	glm::vec3 v0 = vt0.pos;
	glm::vec3 v1 = vt1.pos;
	glm::vec3 v2 = vt2.pos;

	glm::vec3 v0v1 = v1 - v0;
	glm::vec3 v0v2 = v2 - v0;
	glm::vec3 N = glm::cross(v0v1, v0v2); // N
	glm::vec3 pvec = glm::cross(ray.dir, v0v2);
	float det = glm::dot(v0v1, pvec);

	// ray and triangle are parallel if det is close to 0
	if (fabs(det) < kEpsilon) return false;

	float invDet = 1 / det;

	glm::vec3 tvec = ray.origin - v0;
	float u = glm::dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) return false;

	glm::vec3 qvec = glm::cross(tvec, v0v1);
	float v = glm::dot(ray.dir, qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	float t = glm::dot(v0v2, qvec) * invDet;

	// check if the triangle is in behind the ray
	if (t < 0) return false; // the triangle is behind 

	glm::dvec3 P = glm::dvec3(ray.origin) + (double)t * glm::dvec3(ray.dir);

	collision.point.coordText = (1.0f - u - v) * vt0.coordText + u * vt1.coordText + v * vt2.coordText;

	if (mat->textures.size()>0&& mat->textures["textureColor"]!=nullptr)
	{
		//TODO: Implement linear mix filter
		Texture::resolution textSize = mat->textures["textureColor"]->res;
		int x = (int)(collision.point.coordText[0] * (textSize.w - 1));
		int y = (int)(collision.point.coordText[1] * (textSize.h - 1));
		while(x<0) x+= textSize.w;
		x = x % textSize.w;
		while (y < 0) y += textSize.h;
		y = y % textSize.h;
		collision.point.color = mat->textures["textureColor"]->getColorAtXY(0, x, y);
	}
	else
		collision.point.color = mat->color;
	collision.point.normal = (1.0f - u - v) * vt0.normal + u * vt1.normal + v * vt2.normal;
	collision.point.normal = glm::normalize(collision.point.normal);
	collision.point.pos = glm::vec4(P, 1.0f);
	collision.distance = (float)glm::length(P - glm::dvec3(ray.origin));
	collision.mat = mat;
	return true; // this ray hits the triangle
}