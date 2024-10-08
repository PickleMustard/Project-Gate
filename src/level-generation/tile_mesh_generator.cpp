#include "tile_mesh_generator.h"
#include "godot_cpp/classes/array_mesh.hpp"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/multi_mesh.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/rendering_server.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_int64_array.hpp"
#include "godot_cpp/variant/packed_vector2_array.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/rid.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include <cstdint>

godot::TileMeshGenerator::TileMeshGenerator() {
	m_inner_size = 0.0f;
	m_outer_size = 1.0f;
	m_height = 1.0f;
	m_is_flat_topped = false;
	m_surface_arrays.resize(RenderingServer::ARRAY_MAX);
}

godot::TileMeshGenerator::TileMeshGenerator(float inner_size, float outer_size, float height, bool is_flat_topped) {
	m_inner_size = inner_size;
	m_outer_size = outer_size;
	m_height = height;
	m_is_flat_topped = is_flat_topped;
	m_surface_arrays.resize(RenderingServer::ARRAY_MAX);
}

godot::TileMeshGenerator::~TileMeshGenerator() {
}

void godot::TileMeshGenerator::_DrawFaces(uint16_t type) {
	m_faces = {};
	/* Top Plane */
	for (int point = 0; point < 6; point++) {
		m_faces.push_back(_CreateFace(m_inner_size, m_outer_size, m_height / 2.0f, m_height / 2.0f, point));
	}
	/* Bottom Plane */
	for (int point = 0; point < 6; point++) {
		m_faces.push_back(_CreateFace(m_inner_size, m_outer_size, -m_height / 2.0f, -m_height / 2.0f, point));
	}
	/*Outside Plane */
	UtilityFunctions::print(vformat("Type: %X", type));
	type = type >> 8;
	if (type & 0x80) {
		switch (type) {
			case 0x81:
				m_faces.push_back(_CreateFace(m_outer_size, m_outer_size, m_height, -m_height / 2.0f, 3));
				for (int point = 0; point < 6; point++) {
					if (point != 3) {
						m_faces.push_back(_CreateFace(m_outer_size, m_outer_size, m_height / 2.0f, -m_height / 2.0f, point));
					}
				}
				break;
			case 0x82:
				m_faces.push_back(_CreateFace(m_outer_size, m_outer_size, m_height, -m_height / 2.0f, 0));
				for (int point = 0; point < 6; point++) {
					if (point != 0) {
						m_faces.push_back(_CreateFace(m_outer_size, m_outer_size, m_height / 2.0f, -m_height / 2.0f, point));
					}
				}
				break;
			case 0x83:
				m_faces.push_back(_CreateFace(m_outer_size, m_outer_size, m_height, -m_height / 2.0f, 0));
				for (int point = 0; point < 6; point++) {
					if (point != 0) {
						m_faces.push_back(_CreateFace(m_outer_size, m_outer_size, m_height + m_height * point, -m_height / 2.0f, point));
					}
				}
				break;
		}
	} else {
		for (int point = 0; point < 6; point++) {
			m_faces.push_back(_CreateFace(m_outer_size, m_outer_size, m_height / 2.0f, -m_height / 2.0f, point));
		}
	}
	/*Inside Plane */
	for (int point = 0; point < 6; point++) {
		m_faces.push_back(_CreateFace(m_inner_size, m_inner_size, -m_height / 2.0f, m_height / 2.0f, point));
	}
}

void godot::TileMeshGenerator::_CombineFaces() {
	Array arr;
	arr.resize(RenderingServer::ARRAY_MAX);
	PackedVector3Array vertices;
	PackedInt32Array indices;
	PackedVector2Array uvs;
	PackedVector3Array normals;

	for (int i = 0; i < m_faces.size(); i++) {
		vertices.append_array(m_faces[i].vertices);
		normals.append_array(m_faces[i].normals);
		uvs.append_array(m_faces[i].uvs);

		int offset = (4 * i);
		for (int index : m_faces[i].indices) {
			indices.push_back(index + offset);
		}
	}

	m_surface_arrays[Mesh::ARRAY_NORMAL] = normals;
	m_surface_arrays[Mesh::ARRAY_VERTEX] = vertices;
	m_surface_arrays[Mesh::ARRAY_TEX_UV] = uvs;
	m_surface_arrays[Mesh::ARRAY_INDEX] = indices;
}

godot::Face godot::TileMeshGenerator::_CreateFace(float inner_radius, float outer_radius, float height_a, float height_b, int point, bool reverse) {
	Vector3 point_a = _GetPoint(inner_radius, height_a, point);
	Vector3 point_b = _GetPoint(inner_radius, height_a, (point < 5) ? point + 1 : 0);
	Vector3 point_c = _GetPoint(outer_radius, height_b, (point < 5) ? point + 1 : 0);
	Vector3 point_d = _GetPoint(outer_radius, height_b, point);

	PackedVector3Array vertices{ point_a, point_b, point_c, point_d };
	PackedVector3Array normals{ point_a.normalized(), point_b.normalized(), point_c.normalized(), point_d.normalized() };
	PackedInt32Array indices{ 2, 1, 0, 0, 3, 2 };
	PackedVector2Array uvs{ Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) };

	if (reverse) {
		vertices.reverse();
	}

	return Face{ vertices, indices, uvs, normals };
}

godot::Vector3 godot::TileMeshGenerator::_GetPoint(float size, float height, int index) {
	float angle_deg = m_is_flat_topped ? 60 * index : 60 * index - 30;
	float angle_rad = Math_PI / 180.0f * angle_deg;
	return Vector3((size * Math::cos(angle_rad)), height, size * Math::sin(angle_rad));
}

godot::Ref<godot::ArrayMesh> godot::TileMeshGenerator::DrawMesh(uint16_t type) {
	_DrawFaces(type);
	_CombineFaces();

	Ref<ArrayMesh> arr_mes = Ref<ArrayMesh>(memnew(ArrayMesh));
	arr_mes->add_surface_from_arrays(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES, m_surface_arrays);
	this->set_mesh(arr_mes);
	return arr_mes;
}

void godot::TileMeshGenerator::_bind_methods() {
}
