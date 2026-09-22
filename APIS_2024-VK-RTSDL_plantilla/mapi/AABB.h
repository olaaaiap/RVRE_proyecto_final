#pragma once
#include "common.h"
#include "vertex.h"
#include "Material.h"
class Mesh;
/// <summary>
/// Estructura que almacena información de un punto en el que ha colisionado un rayo
/// </summary>
typedef struct collision_t
{
	vertex_t point; //almacena posición 3D(coordenadas de cámara), color,coordenadas de textura y normal del punto en el que ha intersecado
	//no almacena la tangente
	float distance; //distancia del punto al origen del rayo
	Material* mat; //propiedades del material al que pertenece este punto (transparencia, reflexión, refracción...)
	Entity* entity; //referencia al objeto con el que colisionó
	Mesh* mesh;		//referencia a la malla del objeto con la que colisionó 
}collision_t;


//
//Estructura para almacenar datos de un triángulo (3 vértices)
//
typedef struct triangle_t
{
	union {
		struct {
			vertex_t* v0; //punteros a los vértices. La estructura solo tiene referencias
			vertex_t* v1; //a vértices para poder ahorrar memoria
			vertex_t* v2;
		};
		vertex_t* v[3];
	};
	
}triangle_t;


#define FLOAT_MIN -std::numeric_limits<float>::max()
#define FLOAT_MAX  std::numeric_limits<float>::max()

//
//Clase que gestiona las colisiones entre mallas y rayos en el raytracer
//Funciona como un octree, por cada nivel se subdivide en 8 cajas alineadas con los ejes
//Cada subnivel almacena una referencia (punteros) a los triángulos que le pertenecen
//



class AABBColls {
	typedef struct AABB_t
	{
		glm::vec3 minB, maxB;
		int nSons;
		int id;
		int sons[8];
	}AABB_t;

	typedef struct triangleIdxs_t
	{
		union {
			struct {
				int v0; //punteros a los vértices. La estructura solo tiene referencias
				int v1; //a vértices para poder ahorrar memoria
				int v2;
			};
			int v[3];
		};

	}triangleIdxs_t;


	Material* mat=nullptr;
	//original
	//MVP*vertices
	std::vector<vertex_t>& vertices;

	std::vector<vertex_t> transfVertices;
	std::vector<triangleIdxs_t> triangles;
	std::vector<AABB_t> AABBs;
	std::map<int,std::vector<int>> trianglesPerAABB;
	std::string name;
	
public:
	AABBColls(std::vector<vertex_t>& vertices):vertices(vertices), mat(nullptr) { };

	void addMesh(std::string name,Mesh* mesh, Material* mat);
	//
	//Método para actualizar la jerarquía de cajas y triángulos almacenados. En cada step, si se ha movido/cambiado de posición
	//la cámara o el objeto, se deben actualizar todas las posiciones de los triángulos y recalcular la jerarquía de cajas
	//
	void update(glm::mat4 matrixTransf);

	bool testCollisionAABB(int boxId, collisionRay_t r);

	collision_t getCollisions(collisionRay_t ray, int boxId);

	bool triangleTestRay(int triangleId, Material* mat, collisionRay_t ray, collision_t& collision);

	//
	//Método para crear el siguiente nivel de cajas AABB. Dada la caja actual, se crean 8 cajas hijas dividiendo por la mitad en los 
	//4 ejes 3D. Como resultado, hay un máximo de 8 cajas hijas. En caso de no haber triángulos suficientes, se eliminan las cajas vacías
	//
	void subdivide(int boxId);
	//
	//Método para ajustar la caja al tamaño de los triángulos que almacena
	//
	void resize(int boxId);
	void resizeRec(int boxId);

	static  AABB_t newAABB() {
		AABB_t n;
		memset(&n, 0, sizeof(AABB_t));
		n.minB = glm::vec3(FLOAT_MAX, FLOAT_MAX, FLOAT_MAX);
		n.maxB = glm::vec3(FLOAT_MIN, FLOAT_MIN, FLOAT_MIN);
		return n;
	}

	bool testCollisionPoint(AABB_t box, glm::vec3 p);

};

