//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#include "vulkan_hooks.h"

VulkanHooks *VulkanHooks::singleton = nullptr;

VulkanHooks::VulkanHooks() {
	if (singleton == nullptr) {
		singleton = this;
	}
}

VulkanHooks::~VulkanHooks() {
	if (singleton == this) {
		singleton = nullptr;
	}
}
