#ifndef TILE_MESH_GENERATOR_H
#define TILE_MESH_GENERATOR_H

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_int64_array.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/vector3.hpp"

namespace godot {

struct Face {
	PackedVector3Array vertices;
	PackedInt32Array indices;
	PackedVector2Array uvs;
	PackedVector3Array normals;
};

class TileMeshGenerator : public MeshInstance3D {
	GDCLASS(TileMeshGenerator, MeshInstance3D);

private:
	Vector<Face> m_faces;
	float m_inner_size;
	float m_outer_size;
	float m_height;
	bool m_is_flat_topped;
	Array m_surface_arrays;

	void _DrawFaces();
	void _CombineFaces();
	Face _CreateFace(float inner_radius, float outer_radius, float height_a, float height_b, int point, bool reverse = false);
	Vector3 _GetPoint(float size, float height, int index);

public:
	TileMeshGenerator();
	TileMeshGenerator(float inner_size, float outer_size, float height, bool is_flat_topped);
	~TileMeshGenerator();

	void DrawMesh();

protected:
	static void _bind_methods();
};
} //namespace godot

#endif
