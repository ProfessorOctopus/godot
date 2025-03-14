//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#include "jolt_convex_polygon_shape_3d.h"
#include "../jolt_project_settings.h"
#include "../misc/jolt_type_conversions.h"

#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"

JPH::ShapeRefC JoltConvexPolygonShape3D::_build() const {
	const int vertex_count = (int)vertices.size();

	if (unlikely(vertex_count == 0)) {
		return nullptr;
	}

	ERR_FAIL_COND_V_MSG(vertex_count < 3, nullptr, vformat("Failed to build Jolt Physics convex polygon shape with %s. It must have a vertex count of at least 3. This shape belongs to %s.", to_string(), _owners_to_string()));

	JPH::Array<JPH::Vec3> jolt_vertices;
	jolt_vertices.reserve((size_t)vertex_count);

	const Vector3 *vertices_begin = &vertices[0];
	const Vector3 *vertices_end = vertices_begin + vertex_count;

	for (const Vector3 *vertex = vertices_begin; vertex != vertices_end; ++vertex) {
		jolt_vertices.emplace_back((float)vertex->x, (float)vertex->y, (float)vertex->z);
	}

	const float min_half_extent = _calculate_aabb().get_shortest_axis_size() * 0.5f;
	const float actual_margin = MIN(margin, min_half_extent * JoltProjectSettings::collision_margin_fraction);

	const JPH::ConvexHullShapeSettings shape_settings(jolt_vertices, actual_margin);
	const JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
	ERR_FAIL_COND_V_MSG(shape_result.HasError(), nullptr, vformat("Failed to build Jolt Physics convex polygon shape with %s. It returned the following error: '%s'. This shape belongs to %s.", to_string(), to_godot(shape_result.GetError()), _owners_to_string()));

	return shape_result.Get();
}

AABB JoltConvexPolygonShape3D::_calculate_aabb() const {
	AABB result;

	for (int i = 0; i < vertices.size(); ++i) {
		if (i == 0) {
			result.position = vertices[i];
		} else {
			result.expand_to(vertices[i]);
		}
	}

	return result;
}

Variant JoltConvexPolygonShape3D::get_data() const {
	return vertices;
}

void JoltConvexPolygonShape3D::set_data(const Variant &p_data) {
	ERR_FAIL_COND(p_data.get_type() != Variant::PACKED_VECTOR3_ARRAY);

	vertices = p_data;

	aabb = _calculate_aabb();

	destroy();
}

void JoltConvexPolygonShape3D::set_margin(float p_margin) {
	if (unlikely(margin == p_margin)) {
		return;
	}

	margin = p_margin;

	destroy();
}

String JoltConvexPolygonShape3D::to_string() const {
	return vformat("{vertex_count=%d margin=%f}", vertices.size(), margin);
}
