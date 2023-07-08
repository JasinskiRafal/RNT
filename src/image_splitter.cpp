#include <algorithm>

#include <image_splitter.hpp>
#include <opencv2/imgproc.hpp>

namespace rnt_singleton
{
    namespace image
    {
        const cv::Scalar kWhite(255, 255, 255);
        const cv::Scalar kBlack(0, 0, 0);
        const double k_minimal_note_size = 500;

        std::mutex _file_access_lock;

        std::vector<std::vector<cv::Point>> remove_small_contours(const std::vector<std::vector<cv::Point>> &contours, double threshold)
        {
            std::vector<std::vector<cv::Point>> ret{contours};
            auto compare_areas = [](const std::vector<cv::Point> &left, const std::vector<cv::Point> &right)
            { return cv::contourArea(left) < cv::contourArea(right); };
            double area_threshold_value = cv::contourArea(*std::max_element(ret.begin(),
                                                                            ret.end(),
                                                                            compare_areas)) *
                                          threshold;

            ret.erase(std::remove_if(ret.begin(),
                                     ret.end(),
                                     [&area_threshold_value](const auto &contour)
                                     { return cv::contourArea(contour) < area_threshold_value; }),
                      ret.end());

            return ret;
        }

        std::vector<Note> Splitter::split(const std::string &file_name) const
        {

            std::vector<Note> notes;
            cv::Mat image_file;
            {
                std::lock_guard<std::mutex> lock(_file_access_lock);
                cv::Mat image_file = cv::imread(file_name);
            }
            if (image_file.empty())
            {
                std::cerr << file_name << " file not found\n";
                return notes;
            }

            cv::Mat original_image{std::ref(image_file)};

            cv::cvtColor(image_file, image_file, cv::ColorConversionCodes::COLOR_BGR2GRAY);
            cv::threshold(image_file, image_file, _threshold, k_max_value, cv::THRESH_BINARY);

            std::vector<std::vector<cv::Point>> note_contours;
            cv::findContours(image_file, note_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

            cv::Mat notes_masks(image_file.size(), CV_8U, kWhite);
            cv::fillPoly(notes_masks, note_contours, kBlack, cv::LINE_AA);

            cv::Mat masked_image;
            cv::copyTo(original_image, masked_image, notes_masks);

            std::vector<cv::Rect> bounding_rectangles;
            for (auto contour : note_contours)
            {
                bounding_rectangles.push_back(cv::boundingRect(contour));
            }

            bounding_rectangles.erase(std::remove_if(bounding_rectangles.begin(), bounding_rectangles.end(),
                                                     [](const auto &rectangle)
                                                     {
                                                         return rectangle.area() < k_minimal_note_size;
                                                     }),
                                      bounding_rectangles.end());

            for (auto &rectangle : bounding_rectangles)
            {
                notes.push_back(Note(masked_image(rectangle)));
            }

            return notes;
        }
    }
}
