#include "main.hpp"

#include <iostream>

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
        image_original = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

        if (image_original.empty())
        {
            std::cout << "Could not open or find the image." << std::endl;

            exit_value = kInvalidInput;
            goto Exit;
        }
        else
        {
            std::cout << "Choose your option " <<
                "(1 - thresholding, 2 - linear contrast stretching, 3 - solarisation): ";

            char option;
            std::cin >> option;

            if (option >= '1' && option <= '3')
            {
                image_modified = image_original.clone();

                cv::namedWindow(kNameWindowOriginal, cv::WINDOW_AUTOSIZE);
                cv::imshow(kNameWindowOriginal, image_original);
            }

            switch (option)
            {
            case OPTION_THRESHOLDING:
            {
                int threshold_value = 128;
                const int kThresholdValueMax = 255;

                cv::namedWindow(kNameWindowModified, cv::WINDOW_AUTOSIZE);
                cv::createTrackbar(kTrackbarNameThreshold, kNameWindowModified,
                    &threshold_value, kThresholdValueMax,
                    TrackbarCallback, (void*)OPTION_THRESHOLDING);
            } break;
            case OPTION_LINEAR_CONTRAST_STRETCHING:
            {                
                const int kValueMax = 255;

                cv::namedWindow(kNameWindowModified, cv::WINDOW_AUTOSIZE);

                cv::createTrackbar(kTrackbarNameConstastStretchingMin,
                    kNameWindowModified, &contrast_stretching_min, kValueMax,
                    TrackbarCallback, (void*)OPTION_LINEAR_CONTRAST_STRETCHING);
                cv::createTrackbar(kTrackbarNameConstastStretchingMax,
                    kNameWindowModified, &contrast_stretching_max, kValueMax,
                    TrackbarCallback, (void*)OPTION_LINEAR_CONTRAST_STRETCHING);
            } break;
            case OPTION_SOLARISING:
            {
                Solarisation();

                cv::namedWindow(kNameWindowModified, cv::WINDOW_AUTOSIZE);
                cv::imshow(kNameWindowModified, image_modified);
            } break;
            default:
                std::cout << "Invalid option." << std::endl;

                break;
            }

            if (option >= '1' && option <= '3')
            {
                cv::waitKey(0);
            }
        }
    }

Exit:
    return exit_value;
}

static void Thresholding(const uchar kThreshold)
{
    for (int row = 0; row < image_original.rows; ++row)
    {
        for (int col = 0; col < image_original.cols; ++col)
        {
            if (image_original.at<uchar>(row, col) < kThreshold)
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

static void ContrastStretching(const uchar kOutMin, const uchar kOutMax)
{
    uchar in_min;
    uchar in_max;
    std::tie(in_min, in_max) = GetMinAndMaxBrightnessValues(image_original);

    for (int row = 0; row < image_original.rows; ++row)
    {
        for (int col = 0; col < image_original.cols; ++col)
        {
            const double kCurrent =
                static_cast<double>(image_original.at<uchar>(row, col));
            image_modified.at<uchar>(row, col) =
                (kCurrent - in_min) /
                (in_max - in_min) *
                (kOutMax - kOutMin) +
                kOutMin;
        }
    }
}

static void Solarisation(void)
{
    /* Only `in_max` is required. */
    uchar in_min;
    uchar in_max;
    std::tie(in_min, in_max) = GetMinAndMaxBrightnessValues(image_original);

    std::cout << "[DEBUG]: " <<
        static_cast<int>(in_min) << " " << static_cast<int>(in_max) <<
        std::endl;

    if (in_max != 0)
    {
        double a = -1;
        double b = in_max;

        for (int row = 0; row < image_original.rows; ++row)
        {
            for (int col = 0; col < image_original.cols; ++col)
            {
                const uchar kCurrent = image_original.at<uchar>(row, col);
                image_modified.at<uchar>(row, col) =
                    (a * kCurrent * kCurrent + b * kCurrent) * 4.0 / in_max;
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
            if (kPixel > max) { max = kPixel; }
            if (kPixel < min) { min = kPixel; }
        }
    }

    return std::pair<uchar, uchar>(min, max);
}

static void TrackbarCallback(int value, void* userdata)
{
    if ((char)userdata == OPTION_THRESHOLDING)
    {
        Thresholding(static_cast<uchar>(value));
    }
    else
    {
        ContrastStretching(static_cast<uchar>(contrast_stretching_min),
                           static_cast<uchar>(contrast_stretching_max));
    }

    cv::imshow(kNameWindowModified, image_modified);
}