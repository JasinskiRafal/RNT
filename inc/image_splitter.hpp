#pragma once

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

namespace rnt
{
    class Note
    {
        cv::Mat _image;

    public:
        Note(const cv::Mat &image) : _image{image} {}
        cv::Mat get() { return _image; }
    };

    namespace image
    {
        class Splitter
        {
            const double k_max_value = 255;
            double _threshold;
            double _contour_area_threshold;

        public:
            Splitter(double threshold = 125,
                     double contour_area_threshold = 0.8) : _threshold{threshold},
                                                            _contour_area_threshold{contour_area_threshold}
            {
            }

            std::vector<Note> split(const std::string &file_name) const;
            std::vector<Note> operator()(const std::string& file_name) const;
        };
    }
}
