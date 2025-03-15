//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#include "register_types.h"
#include "fbx_document.h"
#ifdef TOOLS_ENABLED
#include "editor/editor_scene_importer_fbx2gltf.h"
#include "editor/editor_scene_importer_ufbx.h"
#include "core/config/project_settings.h"
#include "editor/editor_node.h"

static void _editor_init() {
	Ref<EditorSceneFormatImporterUFBX> import_fbx;
	import_fbx.instantiate();
	ResourceImporterScene::add_scene_importer(import_fbx);

	bool fbx2gltf_enabled = GLOBAL_GET("filesystem/import/fbx2gltf/enabled");
	if (fbx2gltf_enabled) {
		Ref<EditorSceneFormatImporterFBX2GLTF> importer;
		importer.instantiate();
		ResourceImporterScene::add_scene_importer(importer);
	}
}
#endif // TOOLS_ENABLED

void initialize_fbx_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(FBXDocument);
		GDREGISTER_CLASS(FBXState);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {

		GDREGISTER_CLASS(EditorSceneFormatImporterUFBX);

		GLOBAL_DEF_RST_BASIC("filesystem/import/fbx2gltf/enabled", true);
		GDREGISTER_CLASS(EditorSceneFormatImporterFBX2GLTF);
		GLOBAL_DEF_RST("filesystem/import/fbx2gltf/enabled.android", false);
		GLOBAL_DEF_RST("filesystem/import/fbx2gltf/enabled.web", false);

		EditorNode::add_init_callback(_editor_init);
	}
#endif // TOOLS_ENABLED
}

void uninitialize_fbx_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}// TODO: 20240118 // fire // FBXDocument::unregister_all_gltf_document_extensions();
}
