#include <image.hpp>

namespace rnt
{
    namespace img
    {
        File::File(std::string file_name) : _file_matrix(cv::imread(file_name)) {}

        bool File::is_correct() const { return !_file_matrix.empty(); }

        std::optional<Image> File::get_image() const
        {
            if (!is_correct())
            {
                return std::nullopt;
            }
            return {_file_matrix};
        }

        namespace operations
        {
            ColourConverter::ColourConverter(cv::ColorConversionCodes code) : _conversion_code{code}
            {
            }

            Image ColourConverter::convert(const Image &matrix) const
            {
                Image ret;
                cv::cvtColor(matrix, ret, _conversion_code);
                return ret;
            }

            Blurer::Blurer(cv::Size window_size, cv::Point anchor_point, cv::BorderTypes border_type) : _window_size{window_size},
                                                                                                        _anchor_point{anchor_point},
                                                                                                        _border_type{border_type}
            {
            }
            Image Blurer::blur(const Image &matrix) const
            {
                Image ret;
                cv::blur(matrix, ret, _window_size, _anchor_point, _border_type);
                return ret;
            }

            Thresholder::Thresholder(double threshold, double max_value, cv::ThresholdTypes type) : _threshold{threshold},
                                                                                                    _max_value{max_value},
                                                                                                    _type{type}
            {
            }

            Image Thresholder::threshold(const Image &image) const
            {
                Image ret;
                cv::threshold(image, ret, _threshold, _max_value, _type);
                return ret;
            }

            AdaptiveThresholder::AdaptiveThresholder(double max_value,
                                                     cv::ThresholdTypes type,
                                                     cv::AdaptiveThresholdTypes adaptive_type,
                                                     int block_size,
                                                     double calculation_constant) : Thresholder{0, max_value, type},
                                                                                    _adaptive_type{adaptive_type},
                                                                                    _block_size{block_size},
                                                                                    _calculation_constant{calculation_constant}
            {
            }

            Image AdaptiveThresholder::threshold(const Image &image) const
            {
                Image ret;
                cv::adaptiveThreshold(image, ret, _max_value, _adaptive_type, _type, _block_size, _calculation_constant);
                return ret;
            }

            Masker::Masker(const Image &mask) : _mask{mask}
            {
            }

            Image Masker::mask(const Image &image) const
            {
                Image ret;
                cv::copyTo(image, ret, _mask);
                return ret;
            }

            SequenceBuilder &SequenceBuilder::convert_colour(cv::ColorConversionCodes code)
            {
                _sequence.push_back(ColourConverter{code});
                return *this;
            }

            SequenceBuilder &SequenceBuilder::blur(cv::Size window_size, cv::Point anchor_point, cv::BorderTypes border_type)
            {
                _sequence.push_back(Blurer(window_size, anchor_point, border_type));
                return *this;
            }

            SequenceBuilder &SequenceBuilder::simple_threshold(double threshold, double max_value, cv::ThresholdTypes type)
            {
                _sequence.push_back(Thresholder{threshold, max_value, type});
                return *this;
            }

            SequenceBuilder &SequenceBuilder::adaptive_threshold(double max_value, cv::ThresholdTypes type, cv::AdaptiveThresholdTypes adaptive_type, int block_size, double calculation_constant) {
                _sequence.push_back(AdaptiveThresholder{max_value, type, adaptive_type, block_size, calculation_constant});
                return *this;
            }

            SequenceBuilder &SequenceBuilder::mask(const Image &mask)
            {
                _sequence.push_back(Masker{mask});
                return *this;
            }

            Sequence SequenceBuilder::build()
            {
                return _sequence;
            }

            OperationVisitor::OperationVisitor(const Image &image) : _image{image}
            {
            }

            void OperationVisitor::operator()(const ColourConverter &operation)
            {
                _image = operation.convert(_image);
            }

            void OperationVisitor::operator()(const Blurer &operaton)
            {
                _image = operaton.blur(_image);
            }

            void OperationVisitor::operator()(const Thresholder &operation)
            {
                _image = operation.threshold(_image);
            }

            void OperationVisitor::operator()(const Masker &operation) {}
        } // namespace operations

        Contours make_contours(const Image &image, cv::RetrievalModes retrieval_mode, cv::ContourApproximationModes approx_type)
        {
            Contours ret;
            cv::findContours(image, ret, retrieval_mode, approx_type);
            return ret;
        }

        Rectangles make_bounding_rectangles(const Contours &contours)
        {
            Rectangles ret;
            for (auto contour : contours)
            {
                ret.push_back(cv::boundingRect(contour));
            }
            return ret;
        }

        Images split_image(const Image &image, const Rectangles &boudning_rectangles)
        {
            Images ret;
            for (auto rectangle : boudning_rectangles)
            {
                ret.push_back({image(rectangle)});
            }
            return ret;
        }

        Image process(operations::OperationVisitor &visitor, operations::Sequence &sequence)
        {
            for (auto operation : sequence)
            {
                std::visit(visitor, operation);
            }
            return visitor.get();
        }
    } // namespace img

} // namespace rnt
