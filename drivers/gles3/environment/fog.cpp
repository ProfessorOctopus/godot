//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#ifdef GLES3_ENABLED

#include "fog.h"

using namespace GLES3;

/* FOG */
RID Fog::fog_volume_allocate() {
	return RID();
}

void Fog::fog_volume_initialize(RID p_rid) {}

void Fog::fog_volume_free(RID p_rid) {}

void Fog::fog_volume_set_shape(RID p_fog_volume, RS::FogVolumeShape p_shape) {}

void Fog::fog_volume_set_size(RID p_fog_volume, const Vector3 &p_size) {}

void Fog::fog_volume_set_material(RID p_fog_volume, RID p_material) {}

AABB Fog::fog_volume_get_aabb(RID p_fog_volume) const {
	return AABB();
}

RS::FogVolumeShape Fog::fog_volume_get_shape(RID p_fog_volume) const {
	return RS::FOG_VOLUME_SHAPE_BOX;
}

#endif // GLES3_ENABLED
