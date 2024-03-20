#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>

#define VEC_INDEX_RED	2
#define VEC_INDEX_GREEN	1
#define VEC_INDEX_BLUE	0

typedef enum ErrorCode {
    kOk,
    kMissingArgument,
    kInvalidInput
} ErrorCode;

/* Primary functions. */
static void ContrastStretching(const cv::Mat&, cv::Mat&, const uchar, const uchar);
static void Solarization(const cv::Mat&, cv::Mat&);
static void Thresholding(const cv::Mat&, cv::Mat&, const uchar);

/* Additional functions. */
static std::pair<uchar, uchar> GetMinAndMaxBrightnessValues(const cv::Mat&);

int main(int argc, char** argv)
{
    int exit_value = kOk;

    if (argc != 2)
    {
        std::cout << " Usage: " << argv[0] << " ImageToLoadAndDisplay" << std::endl;

        exit_value = kMissingArgument;
        goto Exit;
    }
    else
    {
        const cv::Mat kImageOriginal = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

        if (kImageOriginal.empty())
        {
            std::cout << "Could not open or find the image" << std::endl;

            exit_value = kInvalidInput;
            goto Exit;
        }
        else
        {
            cv::Mat image_modified;

            /*const uchar kResultBrightnessMin = 0;
            const uchar kResultBrightnessMax = 255;
            ContrastStretching(kImageOriginal, image_modified,
                               kResultBrightnessMin, kResultBrightnessMax);*/

            /*const uchar kThreshhold = 128;
            Thresholding(kImageOriginal, image_modified, kThreshhold);*/

            Solarization(kImageOriginal, image_modified);

            cv::imshow("Original", kImageOriginal);
            cv::imshow("Modified", image_modified);

            cv::waitKey(0);
        }
    }

Exit:
    return exit_value;
}

static void ContrastStretching(const cv::Mat& kImageOriginal, cv::Mat& image_modified,
                               const uchar kOutMin, const uchar kOutMax)
{
    image_modified = kImageOriginal.clone();

    uchar in_min;
    uchar in_max;
    std::tie(in_min, in_max) = GetMinAndMaxBrightnessValues(image_modified);

    std::cout << "[DEBUG]: " <<
        static_cast<int>(in_min) << " " << static_cast<int>(in_max) <<
        std::endl;

    for (int row = 0; row < image_modified.rows; ++row)
    {
        for (int col = 0; col < image_modified.cols; ++col)
        {
            const double kCurrent =
                static_cast<double>(image_modified.at<uchar>(row, col));
            image_modified.at<uchar>(row, col) =
                (kCurrent - in_min) /
                (in_max - in_min) *
                (kOutMax - kOutMin) +
                kOutMin;
        }
    }
}

static void Solarization(const cv::Mat& kImageOriginal, cv::Mat& image_modified)
{
    image_modified = kImageOriginal.clone();

    /* Only `in_max` is required. */
    uchar in_min;
    uchar in_max;
    std::tie(in_min, in_max) = GetMinAndMaxBrightnessValues(image_modified);

    std::cout << "[DEBUG]: " <<
        static_cast<int>(in_min) << " " << static_cast<int>(in_max) <<
        std::endl;

    if (in_max != 0)
    {
        double x1 = 0;
        double y1 = 0;

        double x2 = in_max / 2.0;
        double y2 = (in_max * in_max) / 4.0;

        double x3 = in_max;
        double y3 = 0;

        double a = -1;
            /*(y3 - (x3 * (y2 - y1) + x2 * y1 - x1 * y2) / (x2 - x1)) /
            x3 * (x3 - x1 - x2) + x1 * x2;*/
        double b = in_max;
            // (y2 - y1) / (x2 - x1) - a * (x1 + x2);
        double c = 0;
            // (x2 * y1 - x1 * y2) / (x2 - x1) + (a * x1 * x2);

        for (int row = 0; row < image_modified.rows; ++row)
        {
            for (int col = 0; col < image_modified.cols; ++col)
            {
                const double kCurrent =
                    static_cast<double>(image_modified.at<uchar>(row, col));
                image_modified.at<uchar>(row, col) =
                    (a * kCurrent * kCurrent + b * kCurrent + c) * 4.0 / in_max;
            }
        }
    }
}

static void Thresholding(const cv::Mat& kImageOriginal, cv::Mat& image_modified,
                         const uchar kThreshold)
{
    image_modified = kImageOriginal.clone();

    for (int row = 0; row < image_modified.rows; ++row)
    {
        for (int col = 0; col < image_modified.cols; ++col)
        {
            if (image_modified.at<uchar>(row, col) < kThreshold)
            {
                image_modified.at<uchar>(row, col) = 0;
            }
            else
            {
                image_modified.at<uchar>(row, col) = 255;
            }
        }
    }
}

static std::pair<uchar, uchar> GetMinAndMaxBrightnessValues(const cv::Mat& image)
{
    uchar min = 255;
    uchar max = 0;

    for (int row = 0; row < image.rows; ++row)
    {
        for (int col = 0; col < image.cols; ++col)
        {
            const uchar kPixel = image.at<uchar>(row, col);
            if (kPixel > max) max = kPixel;
            if (kPixel < min) min = kPixel;
        }
    }

    return std::pair<uchar, uchar>(min, max);
}