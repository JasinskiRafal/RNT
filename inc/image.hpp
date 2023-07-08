#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <optional>
#include <variant>

namespace rnt
{
    using Image = cv::Mat;
    using Images = std::vector<Image>;

    namespace img
    {
        using Contour = std::vector<cv::Point>;
        using Contours = std::vector<Contour>;

        using Rectangle = cv::Rect;
        using Rectangles = std::vector<Rectangle>;

        class File
        {
            Image _file_matrix;

        public:
            File(std::string file_name);
            bool is_correct() const;
            std::optional<Image> get_image() const;
        };
        namespace operations
        {
            class ColourConverter;
            class Blurer;
            class Thresholder;
            class Masker;

            using Operation = std::variant<ColourConverter, Blurer, Thresholder, Masker>;
            using Sequence = std::vector<Operation>;

            class ColourConverter
            {
                cv::ColorConversionCodes _conversion_code;

            public:
                ColourConverter(cv::ColorConversionCodes code);
                Image convert(const Image &matrix) const;
            };

            class Blurer
            {
                cv::Size _window_size;
                cv::Point _anchor_point;
                cv::BorderTypes _border_type;

            public:
                Blurer(cv::Size window_size, cv::Point anchor_point, cv::BorderTypes border_type);
                Image blur(const Image &matrix) const;
            };

            class Thresholder
            {
                double _threshold;

            protected:
                double _max_value;
                cv::ThresholdTypes _type;

            public:
                Thresholder(double threshold, double max_value, cv::ThresholdTypes type);
                virtual ~Thresholder() = default;
                virtual Image threshold(const Image &image) const;
            };

            class AdaptiveThresholder : public Thresholder
            {
                cv::AdaptiveThresholdTypes _adaptive_type;
                int _block_size;
                double _calculation_constant;

            public:
                AdaptiveThresholder(double max_value, cv::ThresholdTypes type, cv::AdaptiveThresholdTypes adaptive_type, int block_size, double calculation_constant);
                Image threshold(const Image &image) const override;
            };

            class Masker
            {
                Image _mask;

            public:
                Masker(const Image &mask);
                Image mask(const Image &image) const;
            };

            class SequenceBuilder
            {
                Sequence _sequence;

            public:
                SequenceBuilder &convert_colour(cv::ColorConversionCodes code);
                SequenceBuilder &blur(cv::Size window_size, cv::Point anchor_point, cv::BorderTypes border_type);
                SequenceBuilder &simple_threshold(double threshold, double max_value, cv::ThresholdTypes type);
                SequenceBuilder &adaptive_threshold(double max_value, cv::ThresholdTypes type, cv::AdaptiveThresholdTypes adaptive_type, int block_size, double calculation_constant);
                SequenceBuilder &mask(const Image &mask);
                Sequence build();
            };

            class OperationVisitor
            {
                Image _image;

            public:
                OperationVisitor(const Image &image);
                void operator()(const ColourConverter &operation);
                void operator()(const Blurer &operaton);
                void operator()(const Thresholder &operation);
                void operator()(const Masker &operation);
                Image get() { return _image; }
            };
        }

        Contours make_contours(const Image &image, cv::RetrievalModes retrieval_mode, cv::ContourApproximationModes approx_type);
        Rectangles make_bounding_rectangles(const Contours &contours);
        Images split_image(const Image &image, const Rectangles &boudning_rectangles);

        Image process(operations::OperationVisitor &visitor, operations::Sequence &sequence);
    }
}
