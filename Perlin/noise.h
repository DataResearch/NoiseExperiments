#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <array>
#include <functional>
#include <concepts>

#include <cstdint>
#include <cmath>
#include <cassert>

namespace noise
{
	namespace internal
	{
		template<typename ...Args>
		[[nodiscard]] constexpr auto make_array(Args... args)
		{
			constexpr size_t size = sizeof...(Args);
			using type_t = std::remove_cvref_t<decltype((args, ...))>;
			return std::array<type_t, size>{std::forward<Args>(args) ...};
		}

		template<std::floating_point real>
		struct vector2d
		{
			real x;
			real y;

			constexpr real operator*(const vector2d& rhs) const
			{
				return x * rhs.x + y * rhs.y;
			}
		};

		template<std::floating_point real>
		struct vector3d
		{
			real x;
			real y;
			real z;

			constexpr real operator*(const vector3d& rhs) const
			{
				return x * rhs.x + y * rhs.y + z * rhs.z;
			}
		};

		template<std::floating_point real>
		[[nodiscard]] real lerp(real start, real end, real t)
		{
			return start * (1 - t) + end * t;
		}

		template<std::floating_point real>
		constexpr static real fade(real t)
		{
			// 6 * t^5 - 15 * t^4 + 10 * t^3
			return t * t * t * (t * (t * 6 - 15) + 10);
		}

		constinit auto permutations =
			internal::make_array(151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36,
								 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0,
								 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87,
								 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146,
								 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40,
								 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18,
								 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64,
								 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206,
								 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2,
								 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
								 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242,
								 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
								 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4,
								 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66,
								 215, 61, 156, 180);
	}

	namespace perlin
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
			// TODO (@CodingChris): implement perlin noise3d
		};

	}

	namespace simplex
	{
		template<std::floating_point real>
		struct simplex2d
		{
			// TODO (@CodingChris): implement simplex noise 2d
		};

		template<std::floating_point real>
		struct simplex3d
		{
			// TODO (@CodingChris): implement simplex noise 3d
		};

	}
}

#endif //PERLIN_NOISE_H
