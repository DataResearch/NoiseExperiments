
#include "perlin.hpp"
#include <fstream>
#include <cstring>

void write_ppm_image(int32_t width, int32_t height, const unsigned char* pixels)
{
	std::ofstream image;
	image.open("test.ppm");

	// header
	image << "P3" << "\n" << width << " " << height << "\n" << "255" << "\n";

	// pixel data
	for (int64_t i = 0; i < (width * height * 3); ++i)
	{
		int value = static_cast<int>(pixels[i]);
		image << value << " ";
	}

}

int main()
{
	constexpr int32_t width = 3200;
	constexpr int32_t height = 3200;
	// row major
	unsigned char* pixel_buffer = new unsigned char[width * height * 3];
	std::memset(pixel_buffer, 0, width * height * 3);
	//unsigned char pixel_buffer[width * height * 3]{}; // times 3 for color channels - rgb

	for (int32_t row = 0; row < height; ++row)
	{
		for (int32_t col = 0; col < width; ++col)
		{
			const auto value = noise::perlin::perlin2d<float>::perlin(245.f + ((float) col) / 50.f,
																	  324.f + ((float) row) / 50.f);

			const auto grey = (value >= 1.0 ? 255 : (value <= 0.0 ? 0 : (int) std::floor(value * 256.0)));
			const auto pixel_index = row * width + col;
			pixel_buffer[pixel_index * 3 + 0] = grey;
			pixel_buffer[pixel_index * 3 + 1] = grey;
			pixel_buffer[pixel_index * 3 + 2] = grey;

			if (col % 50 == 0 || row % 50 == 0)
			{
				pixel_buffer[pixel_index * 3 + 0] = 255;
				pixel_buffer[pixel_index * 3 + 1] = 0;
				pixel_buffer[pixel_index * 3 + 2] = 0;
			}
		}
	}

	write_ppm_image(width, height, pixel_buffer);
	delete[] pixel_buffer;

	return 0;
}