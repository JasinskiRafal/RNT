#include <iostream>
#include <image.hpp>
#include <thread>
#include <filesystem>

constexpr cv::BorderTypes kBorderType = cv::BorderTypes::BORDER_DEFAULT;

constexpr double kMaxValue = 255;

void simple_threshold_test(const rnt::img::File file);
void adaptive_threshold_test(const rnt::img::File file);

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Started the test application without an image path\n";
        return -1;
    }

    std::filesystem::path image_path{argv[1]};

    rnt::img::File test_image(image_path);

    if (!test_image.is_correct())
    {
        std::cerr << "Cannot load image\n";
        return -1;
    }

    simple_threshold_test(test_image);

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
        builder.blur(cv::Size(5, 5), cv::Point(-1, -1), kBorderType)
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

    std::filesystem::create_directory("output");
    std::filesystem::path output_path{"output/split_0.png"};

    for (int i = 0; i < split_images.size(); i++)
    {
        std::stringstream output_file;
        output_file << "split_" << i << ".png";

        output_path.replace_filename(output_file.str());

        cv::imwrite(output_path, split_images[i]);
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
    cv::imwrite("adaptive_threshold_test.png", adaptive_image);
}

