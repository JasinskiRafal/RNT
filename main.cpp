#include <iostream>
#include <image.hpp>
#include <thread>

constexpr cv::BorderTypes kBorderType = cv::BorderTypes::BORDER_DEFAULT;

constexpr double kMaxValue = 255;

void simple_threshold_test(const rnt::img::File file);
void adaptive_threshold_test(const rnt::img::File file);

int main(int argc, char **argv)
{
    // TODO: Change these hardcoded paths to CLI passed arguments
    rnt::img::File test_image("/home/rafalj/RapidNodeTaker/assets/input/notes.png");

    if (!test_image.is_correct())
    {
        std::cerr << "Cannot load image\n";
        return -1;
    }

    simple_threshold_test(test_image);
    adaptive_threshold_test(test_image);

    return 0;
}

void simple_threshold_test(rnt::img::File file)
{
    constexpr double kThresholdValue = 230;
    constexpr cv::ThresholdTypes kThresholdType = cv::THRESH_BINARY_INV;

    rnt::img::operations::Sequence initial_sequence;
    rnt::img::operations::OperationVisitor first_visitor(file.get_image().value());

    rnt::img::operations::SequenceBuilder builder;
    initial_sequence =
        builder.blur(cv::Size(4, 4), cv::Point(-1, -1), kBorderType)
            .convert_colour(cv::COLOR_BGR2GRAY)
            .simple_threshold(kThresholdValue, kMaxValue, kThresholdType)
            .build();

    rnt::Image first_process_image = rnt::img::process(first_visitor,
                                                       initial_sequence);
    rnt::img::Contours first_process_contours = rnt::img::make_contours(first_process_image,
                                                                        cv::RetrievalModes::RETR_EXTERNAL,
                                                                        cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
    rnt::img::Rectangles bounding_rectangles = rnt::img::make_bounding_rectangles(first_process_contours);
    rnt::Images split_images = rnt::img::split_image(file.get_image().value(), bounding_rectangles);

    for (int i = 0; i < split_images.size(); i++)
    {
        std::stringstream output_file_name;
        output_file_name << "/home/rafalj/RapidNodeTaker/assets/split_photos/split " << i << ".png";
        cv::imwrite(output_file_name.str(), split_images[i]);
    }
}

void adaptive_threshold_test(rnt::img::File file)
{
    constexpr int kBlockSize = 7;
    constexpr double kCalculationConstant = 1;
    constexpr cv::AdaptiveThresholdTypes kAdaptiveThresholdType = cv::AdaptiveThresholdTypes::ADAPTIVE_THRESH_MEAN_C;
    constexpr cv::ThresholdTypes kThresholdType = cv::THRESH_BINARY;

    rnt::img::operations::Sequence adaptive_threshold_sequence;
    rnt::img::operations::OperationVisitor adaptive_visitor(file.get_image().value());
    rnt::img::operations::SequenceBuilder builder;

    // Adaptive threshold seems to work incredibely poorly with my current test photo.
    // TODO: Investigate if changing parameters fixes the issue
    adaptive_threshold_sequence =
        builder.convert_colour(cv::COLOR_BGR2GRAY)
            .blur(cv::Size(100, 100), cv::Point(-1, -1), kBorderType)
            .adaptive_threshold(kMaxValue, kThresholdType, kAdaptiveThresholdType, kBlockSize, kCalculationConstant)
            .build();

    rnt::Image adaptive_image = rnt::img::process(adaptive_visitor, adaptive_threshold_sequence);
    cv::imwrite("/home/rafalj/RapidNodeTaker/assets/adaptive_threshold_test.png", adaptive_image);
}
