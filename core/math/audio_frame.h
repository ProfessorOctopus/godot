//========= /*This file is part of : Godot Engine(see LICENSE.txt)*/ ============//
#pragma once
#include "core/math/vector2.h"
#include "core/typedefs.h"

static inline float undenormalize(volatile float f) {
	union {
		uint32_t i;
		float f;
	} v;
	v.f = f;

	// original: return (v.i & 0x7f800000) == 0 ? 0.0f : f;
	// version from Tim Blechmann:
	return (v.i & 0x7f800000) < 0x08000000 ? 0.0f : f;
}

static const float AUDIO_PEAK_OFFSET = 0.0000000001f;
static const float AUDIO_MIN_PEAK_DB = -200.0f; // linear_to_db(AUDIO_PEAK_OFFSET)

struct AudioFrame {
	// Left and right samples.
	union {
		struct {
			float left;
			float right;
		};
#ifndef DISABLE_DEPRECATED
		struct {
			float l;
			float r;
		};
#endif
		float levels[2] = { 0.0 };
	};

	_ALWAYS_INLINE_ const float &operator[](int p_idx) const {
		DEV_ASSERT((unsigned int)p_idx < 2);
		return levels[p_idx];
	}
	_ALWAYS_INLINE_ float &operator[](int p_idx) {
		DEV_ASSERT((unsigned int)p_idx < 2);
		return levels[p_idx];
	}

	_ALWAYS_INLINE_ AudioFrame operator+(const AudioFrame &p_frame) const { return AudioFrame(left + p_frame.left, right + p_frame.right); }
	_ALWAYS_INLINE_ AudioFrame operator-(const AudioFrame &p_frame) const { return AudioFrame(left - p_frame.left, right - p_frame.right); }
	_ALWAYS_INLINE_ AudioFrame operator*(const AudioFrame &p_frame) const { return AudioFrame(left * p_frame.left, right * p_frame.right); }
	_ALWAYS_INLINE_ AudioFrame operator/(const AudioFrame &p_frame) const { return AudioFrame(left / p_frame.left, right / p_frame.right); }
	_ALWAYS_INLINE_ AudioFrame operator+(float p_sample) const { return AudioFrame(left + p_sample, right + p_sample); }
	_ALWAYS_INLINE_ AudioFrame operator-(float p_sample) const { return AudioFrame(left - p_sample, right - p_sample); }
	_ALWAYS_INLINE_ AudioFrame operator*(float p_sample) const { return AudioFrame(left * p_sample, right * p_sample); }
	_ALWAYS_INLINE_ AudioFrame operator/(float p_sample) const { return AudioFrame(left / p_sample, right / p_sample); }

	_ALWAYS_INLINE_ void operator+=(const AudioFrame &p_frame) {
		left += p_frame.left;
		right += p_frame.right;
	}
	_ALWAYS_INLINE_ void operator-=(const AudioFrame &p_frame) {
		left -= p_frame.left;
		right -= p_frame.right;
	}
	_ALWAYS_INLINE_ void operator*=(const AudioFrame &p_frame) {
		left *= p_frame.left;
		right *= p_frame.right;
	}
	_ALWAYS_INLINE_ void operator/=(const AudioFrame &p_frame) {
		left /= p_frame.left;
		right /= p_frame.right;
	}

	_ALWAYS_INLINE_ void operator+=(float p_sample) {
		left += p_sample;
		right += p_sample;
	}
	_ALWAYS_INLINE_ void operator-=(float p_sample) {
		left -= p_sample;
		right -= p_sample;
	}
	_ALWAYS_INLINE_ void operator*=(float p_sample) {
		left *= p_sample;
		right *= p_sample;
	}
	_ALWAYS_INLINE_ void operator/=(float p_sample) {
		left /= p_sample;
		right /= p_sample;
	}

	_ALWAYS_INLINE_ void undenormalize() {
		left = ::undenormalize(left);
		right = ::undenormalize(right);
	}

	_FORCE_INLINE_ AudioFrame lerp(const AudioFrame &p_b, float p_t) const {
		AudioFrame res = *this;

		res.left += (p_t * (p_b.left - left));
		res.right += (p_t * (p_b.right - right));

		return res;
	}

	_ALWAYS_INLINE_ AudioFrame(float p_left, float p_right) {
		left = p_left;
		right = p_right;
	}
	_ALWAYS_INLINE_ AudioFrame(const AudioFrame &p_frame) {
		left = p_frame.left;
		right = p_frame.right;
	}

	_ALWAYS_INLINE_ void operator=(const AudioFrame &p_frame) {
		left = p_frame.left;
		right = p_frame.right;
	}

	_ALWAYS_INLINE_ operator Vector2() const {
		return Vector2(left, right);
	}

	_ALWAYS_INLINE_ AudioFrame(const Vector2 &p_v2) {
		left = p_v2.x;
		right = p_v2.y;
	}
	_ALWAYS_INLINE_ AudioFrame() {}
};

_ALWAYS_INLINE_ AudioFrame operator*(float p_scalar, const AudioFrame &p_frame) {
	return AudioFrame(p_frame.left * p_scalar, p_frame.right * p_scalar);
}

_ALWAYS_INLINE_ AudioFrame operator*(int32_t p_scalar, const AudioFrame &p_frame) {
	return AudioFrame(p_frame.left * p_scalar, p_frame.right * p_scalar);
}

_ALWAYS_INLINE_ AudioFrame operator*(int64_t p_scalar, const AudioFrame &p_frame) {
	return AudioFrame(p_frame.left * p_scalar, p_frame.right * p_scalar);
}

template <>
struct is_zero_constructible<AudioFrame> : std::true_type {};
