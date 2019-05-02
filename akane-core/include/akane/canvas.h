#pragma once
#include "akane/spectrum.h"
#include "akane/external/svpng.inc"
#include <vector>
#include <string>
#include <cassert>
#include <tuple>
#include <algorithm>

namespace akane
{
    struct CanvasPixel
    {
        double r;
        double g;
        double b;
    };

    // film where a scene is rendered
    class Canvas
    {
    public:
        using SharedPtr = std::shared_ptr<Canvas>;

        Canvas(int width, int height) : width_(width), height_(height)
        {
            assert(width > 0 && height > 0);
            data_.resize(width * height * kCanvasChannel, 0.);
        }


		void Set(const Canvas& other) {
			std::copy(other.data_.begin(), other.data_.end(), data_.begin());
		}
		void Append(const Canvas& other)
		{
			for (int i = 0; i < data_.size(); ++i)
			{
				data_[i] += other.data_[i];
			}
		}
        void Append(double rr, double gg, double bb, int x, int y)
        {
            auto index = y * width_ * kCanvasChannel + x * kCanvasChannel;
            data_[index] += rr;
            data_[index + 1] += gg;
            data_[index + 2] += bb;
        }
        CanvasPixel At(int x, int y)
        {
            assert(x >= 0 && x < width_);
            assert(y >= 0 && y < height_);

            auto index = y * width_ * kCanvasChannel + x * kCanvasChannel;
            return CanvasPixel{data_[index], data_[index + 1], data_[index + 2]};
        }
        Spectrum GetSpectrum(int x, int y, akFloat scalar)
        {
            auto [rr, gg, bb] = At(x, y);
            return Spectrum{static_cast<akFloat>(rr * scalar), static_cast<akFloat>(gg * scalar),
                            static_cast<akFloat>(bb * scalar)};
        }

        void Clear()
        {
            for (auto& pixel : data_)
            {
                pixel = 0.;
            }
        }

        void SaveRaw(const std::string& filename, akFloat scalar = 1.f)
        {
            constexpr uint32_t magic = 0x41414141;

            uint32_t width  = width_;
            uint32_t height = height_;

            auto file = fopen(filename.c_str(), "wb");
            fwrite(&magic, 1, 4, file);
            fwrite(&width, 1, 4, file);
            fwrite(&height, 1, 4, file);
            for (auto value : data_)
            {
                float x = value * scalar;
                fwrite(&x, 1, 4, file);
            }
            fclose(file);
        }

        void SaveImage(const std::string& filename, akFloat scalar = 1.f, akFloat gamma = 2.4f)
        {
            std::vector<uint8_t> graph;
            for (int y = 0; y < height_; ++y)
            {
                for (int x = 0; x < width_; ++x)
                {
                    auto spectrum = GetSpectrum(x, y, scalar);
                    auto color    = SpectrumToRGB(GammaCorrect(ToneMap_Aces(spectrum), gamma));

                    for (int i = 0; i < 3; ++i)
                    {
                        graph.push_back(static_cast<uint8_t>(color[i]));
                    }
                }
            }

            auto file = fopen(filename.c_str(), "wb");
            svpng(file, width_, height_, graph.data(), 0);
            fclose(file);
        }

    private:
        static constexpr int kCanvasChannel = 3;

        int width_;
        int height_;
        std::vector<double> data_;
    };
} // namespace akane