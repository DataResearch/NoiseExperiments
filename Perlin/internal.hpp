#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "UnreachableCode"
#pragma once

#include <concepts>
#include <functional>
#include <type_traits>

namespace noise::internal
{
	/// Constructs an array from the provided parameters.
	/// \tparam Args
	/// \param args
	/// \return Compile time constructed array.
	template<typename ...Args>
	[[nodiscard]] constexpr auto make_array(Args... args)
	{
		const auto size = sizeof...(Args);
		using type_t = std::remove_cvref_t<decltype((args, ...))>;
		return std::array<type_t, size>{std::forward<Args>(args) ...};
	}

	/// Represents any 2d vector or point.
	/// \tparam real Any type that is a floating point type.
	template<std::floating_point real>
	struct vector2d
	{
		real x;
		real y;

		/// Implements the dot-product for vectors.
		/// \param rhs The right-hand-side of the multiplication operator.
		/// \return Dot-product of the two vectors.
		constexpr real operator*(const vector2d& rhs) const
		{
			return x * rhs.x + y * rhs.y;
		}
	};

	/// Represents any 3d vector or point.
	/// \tparam real Any type that is a floating point type.
	template<std::floating_point real>
	struct vector3d
	{
		real x;
		real y;
		real z;

		/// Implements the dot-product for vectors.
		/// \param rhs The right-hand-side of the multiplication operator.
		/// \return Dot-product of the two vectors.
		constexpr real operator*(const vector3d& rhs) const
		{
			return x * rhs.x + y * rhs.y + z * rhs.z;
		}
	};

	/// Performs a linear interpolation.
	/// \tparam real Any type that satisfies the std::floating_point concept.
	/// \param start The start value of the interpolation. (start is approx equal to lerped, when t = 0)
	/// \param end The end value of the interpolation. (end is approx equal to lerped, when t = 1)
	/// \param t The interpolating time value. Needs to be in the range [0, 1].
	/// \return The result of the linear interpolation.
	template<std::floating_point real>
	[[nodiscard]] real lerp(real start, real end, real t)
	{
		return start * (1 - t) + end * t;
	}

	/// Maps a linear range [0, 1] onto a non-linear range with the same limits.
	/// The resulting value can be used for a lerp in a smooth blending.
	/// \tparam real Any type that satisfies the std::floating_point concept.
	/// \param t A value in the range of [0, 1].
	/// \return A value whose range is non-linear, that can be used for blending or fading in a lerp.
	template<std::floating_point real>
	constexpr static real fade(real t)
	{
		// 6 * t^5 - 15 * t^4 + 10 * t^3
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	/// Provides a pseudo-random permutation for breaking up patterns and make them seem more random.
	/// This is a default permutations that are proposed by Ken Perlin.
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

#pragma clang diagnostic pop
