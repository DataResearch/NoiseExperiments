#ifndef PERLIN_PERLIN2D_H
#define PERLIN_PERLIN2D_H

#include <array>
#include <functional>

#include <cstdint>
#include <cmath>
#include <cassert>

namespace noise::perlin
{
	namespace internal
	{
		template<typename ...Args>
		constexpr auto make_array(Args... args)
		{
			constexpr size_t size = sizeof...(Args);
			using type_t = std::remove_cvref_t<decltype((args, ...))>;
			return std::array<type_t, size>{std::forward<Args>(args) ...};
		}
	}

	template<typename real_t>
	struct point
	{
		real_t x;
		real_t y;
	};

	// implement the dot product for the point-vector type
	template<typename real_t>
	constexpr auto operator*(const point<real_t>& lhs, const point<real_t>& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	template<typename real_t = float, template<typename hash_value> class hash_fn = std::hash>
	class perlin2d
	{
	private:

		constexpr static auto gradients =
			internal::make_array(
				point<real_t>{.x = real_t{-1}, .y = real_t{-1}},
				point<real_t>{.x = real_t{-1}, .y = real_t{0}},
				point<real_t>{.x = real_t{-1}, .y = real_t{1}},
				point<real_t>{.x = real_t{0}, .y = real_t{1}},
				point<real_t>{.x = real_t{1}, .y = real_t{1}},
				point<real_t>{.x = real_t{1}, .y = real_t{0}},
				point<real_t>{.x = real_t{1}, .y = real_t{-1}},
				point<real_t>{.x = real_t{0}, .y = real_t{-1}});

		constexpr static real_t blending(real_t t)
		{
			// we utilize multiple multiplication, as this avoids the need
			// for cmath functions, that are not yet constexpr! (as of C++20)
			// 6 * t^5 - 15 * t^4 + 10 * t^3
			return
				6 * t * t * t * t * t -
				15 * t * t * t * t +
				10 * t * t * t;
		}

		template<typename index_type>
		static auto get_gradient_at(point<index_type> index)
		{
			// TODO (@CodingChris): this could be constexpr if we use another default hash
			hash_fn<index_type> hash;
			return gradients[(hash(index.x) + hash(index.y)) % gradients.size()];
		}

	public:
		static auto perlin(point<real_t> sample_point)
		{
			// TODO (@CodingChris): we want to use the systems native signed integer type - i.e. int32_t on 32 bit
			// the integer type for internal usage
			using integer_t = int64_t;

			// for the sample point we must determine the next integer coordinates
			// this is done by flooring to the next integer down - a cast should do this without issues but might be
			// causing issues with defined ness down the road. Especially negative values.
			// So we are casting std::floor(ed) values. This will probably yield better results.
			// Sadly std::hash as default hash and cmath functions will prevent this method from working as constexpr.
			const auto floored = point<int64_t>{
				.x = static_cast<int64_t>(std::floor(sample_point.x)),
				.y = static_cast<int64_t>(std::floor(sample_point.y))
			};
			assert(floored.x <= sample_point.x);
			assert((floored.x + 1) >= sample_point.x);
			assert(floored.y <= sample_point.y);
			assert((floored.y + 1) >= sample_point.y);

			// the distance in fractional values, from the nearest floored integer
			// this is used to compute the weight a gradient has for a certain direction
			// in the paper this x value is refered to as u
			// and the y value as v
			const auto floored_distance = point<real_t>{
				.x = sample_point.x - floored.x,
				.y = sample_point.y - floored.y
			};

			// for each of the corners of a square (defined between the floor and ceil points) we need to
			// determine the dot product with a gradient vector and the vector from the corner to the sample_point.
			// we encode this as:
			// 00 - lower left corner, (i, j)
			// 10 - lower right corner, (i + 1, j)
			// 01 - upper left corner, (i, j + 1)
			// 11 - upper right corner, (i + 1, j + 1)
			// where i and j are x and y of the floored values
			// and the +1 versions are the ceiled values
			const auto gradient_00 = get_gradient_at(point<integer_t>{floored.x, floored.y});
			const auto gradient_10 = get_gradient_at(point<integer_t>{floored.x + 1, floored.y});
			const auto gradient_01 = get_gradient_at(point<integer_t>{floored.x, floored.y + 1});
			const auto gradient_11 = get_gradient_at(point<integer_t>{floored.x + 1, floored.y + 1});

			// the floored distance with "- 1" us equal to the ceiled distance
			const auto noise_00 = gradient_00 * point<real_t>{floored_distance.x, floored_distance.y};
			const auto noise_10 = gradient_10 * point<real_t>{floored_distance.x - 1, floored_distance.y};
			const auto noise_01 = gradient_01 * point<real_t>{floored_distance.x, floored_distance.y - 1};
			const auto noise_11 = gradient_11 * point<real_t>{floored_distance.x - 1, floored_distance.y - 1};

			// since we have four points that we need to interpolate between,
			// we interpolate the lower x values and the upper x values - and then
			// we interpolate on the x-axis first - and then interpolate the y values
			// this is similar to a bi-linear filter
			const auto lerp = [](const auto first, const auto second, const auto slide) {
				return first * slide + second * (1 - slide);
			};

			const auto lower_x = lerp(noise_00, noise_10, blending(floored_distance.x));
			const auto upper_x = lerp(noise_01, noise_11, blending(floored_distance.x));
			const auto blended_y = lerp(lower_x, upper_x, blending(floored_distance.y));

			return blended_y;
		}
	};

}

#endif //PERLIN_PERLIN2D_H
