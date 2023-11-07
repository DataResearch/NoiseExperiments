#pragma once

#include "internal.hpp"

#include <array>
#include <functional>
#include <concepts>

#include <cstdint>
#include <cmath>
#include <cassert>

namespace noise::perlin
{
	template<std::floating_point real>
	struct perlin2d
	{
		constexpr static auto gradients =
			internal::make_array(
				// Note (@everyone): we need vectors of unit length - so we
				// provide 1 / 1.41421 as 1 / sqrt(2) approx for vectors where
				// both components are set -- CodingChris
				internal::vector2d<real>{.x = real{-(1 / 1.41421)}, .y = real{-(1 / 1.41421)}},
				internal::vector2d<real>{.x = real{-1}, .y = real{0}},
				internal::vector2d<real>{.x = real{-(1 / 1.41421)}, .y = real{(1 / 1.41421)}},
				internal::vector2d<real>{.x = real{0}, .y = real{1}},
				internal::vector2d<real>{.x = real{(1 / 1.41421)}, .y = real{(1 / 1.41421)}},
				internal::vector2d<real>{.x = real{1}, .y = real{0}},
				internal::vector2d<real>{.x = real{(1 / 1.41421)}, .y = real{-(1 / 1.41421)}},
				internal::vector2d<real>{.x = real{0}, .y = real{-1}});

		template<std::integral index>
		constexpr static auto get_gradient_at(index x, index y)
		{
			return gradients[(internal::permutations[(x + internal::permutations[y & 255]) & 255]) %
							 gradients.size()];
		}

		static auto perlin(real x, real y)
		{
			using index = int64_t;
			using integer = int64_t;

			// define the lower left corner of the cell for which we evaluate the gradients
			const auto floored_x = static_cast<integer>(x);
			const auto floored_y = static_cast<integer>(y);

			const auto fractional_x = x - floored_x;
			const auto fractional_y = y - floored_y;

			enum noise_cell_corner
			{
				LOWER_LEFT,
				LOWER_RIGHT,
				UPPER_LEFT,
				UPPER_RIGHT,
			};

			const auto selected_gradients = internal::make_array(
				get_gradient_at(floored_x, floored_y),
				get_gradient_at(floored_x + 1, floored_y),
				get_gradient_at(floored_x, floored_y + 1),
				get_gradient_at(floored_x + 1, floored_y + 1)
			);

			const auto noise_contributed_values = internal::make_array(
				selected_gradients[LOWER_LEFT] * internal::vector2d<real>{fractional_x, fractional_y},
				selected_gradients[LOWER_RIGHT] * internal::vector2d<real>{fractional_x - 1, fractional_y},
				selected_gradients[UPPER_LEFT] * internal::vector2d<real>{fractional_x, fractional_y - 1},
				selected_gradients[UPPER_RIGHT] * internal::vector2d<real>{fractional_x - 1, fractional_y - 1}
			);

			const auto lower = internal::lerp(noise_contributed_values[LOWER_LEFT],
											  noise_contributed_values[LOWER_RIGHT],
											  internal::fade(fractional_x));
			const auto upper = internal::lerp(noise_contributed_values[UPPER_LEFT],
											  noise_contributed_values[UPPER_RIGHT],
											  internal::fade(fractional_x));
			const auto vertical = internal::lerp(lower, upper, internal::fade(fractional_y));
			return vertical;
		}
	};

	template<std::floating_point real>
	struct perlin3d
	{
		constexpr static auto gradients =
			internal::make_array(
				internal::vector3d<real>{.x = real{0}, .y = real{-1}, .z = real{1}},
				internal::vector3d<real>{.x = real{1}, .y = real{-1}, .z = real{0}},
				internal::vector3d<real>{.x = real{0}, .y = real{-1}, .z = real{-1}},
				internal::vector3d<real>{.x = real{-1}, .y = real{-1}, .z = real{0}},
				internal::vector3d<real>{.x = real{1}, .y = real{0}, .z = real{1}},
				internal::vector3d<real>{.x = real{1}, .y = real{0}, .z = real{-1}},
				internal::vector3d<real>{.x = real{-1}, .y = real{0}, .z = real{1}},
				internal::vector3d<real>{.x = real{-1}, .y = real{0}, .z = real{-1}},
				internal::vector3d<real>{.x = real{0}, .y = real{1}, .z = real{1}},
				internal::vector3d<real>{.x = real{1}, .y = real{1}, .z = real{0}},
				internal::vector3d<real>{.x = real{0}, .y = real{1}, .z = real{-1}},
				internal::vector3d<real>{.x = real{-1}, .y = real{1}, .z = real{0}});

		template<std::integral index>
		constexpr static auto get_gradient_at(index x, index y, index z)
		{
			return gradients[
				(internal::permutations[(x + internal::permutations[(y + internal::permutations[z & 255]) & 255]) &
										255]) % gradients.size()];
		}

		static auto perlin(real x, real y, real z)
		{
			using index = int64_t;
			using integer = int64_t;

			// define the lower left corner of the cell for which we evaluate the gradients
			const auto floored_x = static_cast<integer>(x);
			const auto floored_y = static_cast<integer>(y);
			const auto floored_z = static_cast<integer>(z);

			const auto fractional_x = x - floored_x;
			const auto fractional_y = y - floored_y;
			const auto fractional_z = z - floored_z;

			// select gradients for each of the noise cells (cubes) corners
			const auto selected_gradients = internal::make_array(
				get_gradient_at(floored_x, floored_y, floored_z),
				get_gradient_at(floored_x + 1, floored_y, floored_z),
				get_gradient_at(floored_x, floored_y + 1, floored_z),
				get_gradient_at(floored_x, floored_y, floored_z + 1),
				get_gradient_at(floored_x + 1, floored_y + 1, floored_z),
				get_gradient_at(floored_x + 1, floored_y, floored_z + 1),
				get_gradient_at(floored_x, floored_y + 1, floored_z + 1),
				get_gradient_at(floored_x + 1, floored_y + 1, floored_z + 1)
			);

			const auto noise_contributed_values = internal::make_array(
				selected_gradients[0] * internal::vector3d<real>{fractional_x, fractional_y, fractional_z},
				selected_gradients[1] * internal::vector3d<real>{fractional_x - 1, fractional_y, fractional_z},
				selected_gradients[2] * internal::vector3d<real>{fractional_x, fractional_y - 1, fractional_z},
				selected_gradients[3] * internal::vector3d<real>{fractional_x, fractional_y, fractional_z - 1},
				selected_gradients[4] * internal::vector3d<real>{fractional_x - 1, fractional_y - 1, fractional_z},
				selected_gradients[5] * internal::vector3d<real>{fractional_x - 1, fractional_y, fractional_z - 1},
				selected_gradients[6] * internal::vector3d<real>{fractional_x, fractional_y - 1, fractional_z - 1},
				selected_gradients[7] * internal::vector3d<real>{fractional_x - 1, fractional_y - 1, fractional_z - 1}
			);

			// lerp on the x axis
			const auto x_blend = internal::fade(fractional_x);
			const auto axis_lerp_0 = internal::lerp(noise_contributed_values[3], noise_contributed_values[5], x_blend);
			const auto axis_lerp_1 = internal::lerp(noise_contributed_values[6], noise_contributed_values[7], x_blend);
			const auto axis_lerp_2 = internal::lerp(noise_contributed_values[0], noise_contributed_values[1], x_blend);
			const auto axis_lerp_3 = internal::lerp(noise_contributed_values[2], noise_contributed_values[4], x_blend);

			// lerp bilinear on the y plane
			const auto y_blend = internal::fade(fractional_y);
			const auto plane_lerp_0 = internal::lerp(axis_lerp_0, axis_lerp_1, y_blend);
			const auto plane_lerp_1 = internal::lerp(axis_lerp_2, axis_lerp_3, y_blend);

			// lerp trilinear on the z cube
			const auto z_blend = internal::fade(fractional_z);
			const auto cube_lerp_0 = internal::lerp(plane_lerp_0, plane_lerp_1, z_blend);

			return cube_lerp_0;
		}

		static real perlin2d(real x, real y)
		{
			constexpr static real DEFAULT_Z_2D_APPROX = ((real) 0.753274);
			const auto value = perlin(x, y, DEFAULT_Z_2D_APPROX);
			return value;
		}
	};

}
