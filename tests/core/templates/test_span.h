//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#pragma once
#include "core/templates/span.h"
#include "tests/test_macros.h"

namespace TestSpan {
TEST_CASE("[Span] Constexpr Validators") {
	constexpr Span<uint16_t> span_empty;
	static_assert(span_empty.ptr() == nullptr);
	static_assert(span_empty.size() == 0);
	static_assert(span_empty.is_empty());

	constexpr static uint16_t value = 5;
	constexpr Span<uint16_t> span_value(&value, 1);
	static_assert(span_value.ptr() == &value);
	static_assert(span_value.size() == 1);
	static_assert(!span_value.is_empty());

	constexpr static char32_t array[] = U"122345";
	constexpr Span<char32_t> span_array(array, strlen(array));
	static_assert(span_array.ptr() == &array[0]);
	static_assert(span_array.size() == 6);
	static_assert(!span_array.is_empty());
	static_assert(span_array[0] == U'1');
	static_assert(span_array[span_array.size() - 1] == U'5');

	int idx = 0;
	for (const char32_t &chr : span_array) {
		CHECK_EQ(chr, span_array[idx++]);
	}
}
} // namespace TestSpan
