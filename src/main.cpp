#include "main.hpp"

#include <iostream>
#include <math.h>

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
        image_original = cv::imread(argv[1], cv::IMREAD_COLOR);

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
                Solarization();

                cv::namedWindow(kNameWindowModified, cv::WINDOW_AUTOSIZE);
                cv::imshow(kNameWindowModified, image_modified);
            } break;
            default:
                std::cout << "Invalid option." << std::endl;

                exit_value = kInvalidOption;
                goto Exit;
            }

            if (option >= '1' && option <= '3')
            {
                cv::namedWindow(kNameWindowOriginal, cv::WINDOW_AUTOSIZE);
                cv::imshow(kNameWindowOriginal, image_original);
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
            if (GetPixelBrigthnessRGB(image_original, row, col) < kThreshold)
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

/*
* REVIEW: Something weird happens on the mouth of the duck (turns purple).
*/
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
                static_cast<double>(image_original.at<cv::Vec3b>(row, col)[1]);
            image_modified.at<cv::Vec3b>(row, col)[1] = round(
                (kCurrent - in_min) /
                (in_max - in_min) *
                (kOutMax - kOutMin) +
                kOutMin);
        }
    }
}

static void Solarization(void)
{
    /* Only `in_max` is required. */
    uchar in_min;
    uchar in_max;

    std::tie(in_min, in_max) = GetMinAndMaxBrightnessValues(image_original);

    cv::cvtColor(image_original, image_modified, cv::COLOR_BGR2GRAY);
    image_original = image_modified.clone();

    if (in_max != 0)
    {
        double a = -1;
        double b = in_max;

        for (int row = 0; row < image_modified.rows; ++row)
        {
            for (int col = 0; col < image_modified.cols; ++col)
            {
                const double kCurrent = image_modified.at<uchar>(row, col);
                image_modified.at<uchar>(row, col) = round(
                    (a * kCurrent * kCurrent + b * kCurrent) * 4.0 / in_max);
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
            const uchar kPixel = GetPixelBrigthnessRGB(image, row, col);

            if (kPixel > max) { max = kPixel; }
            if (kPixel < min) { min = kPixel; }
        }
    }

    return std::pair<uchar, uchar>(min, max);
}

static uchar GetPixelBrigthnessRGB(const cv::Mat& image, const int row, const int col)
{
    const uchar r = image.at<cv::Vec3b>(row, col)[VEC_INDEX_RED];
    const uchar g = image.at<cv::Vec3b>(row, col)[VEC_INDEX_GREEN];
    const uchar b = image.at<cv::Vec3b>(row, col)[VEC_INDEX_BLUE];

    return static_cast<uchar>(gray(r, g, b));
}

static void TrackbarCallback(int value, void* userdata)
{
    if ((char)userdata == OPTION_THRESHOLDING)
    {
        image_modified = cv::Mat(image_original.rows, image_original.cols, CV_8UC1);
        cv::cvtColor(image_original, image_modified, cv::COLOR_BGR2GRAY);

        Thresholding(static_cast<uchar>(value));
    }
    else // OPTION_LINEAR_CONTRAST_STRETCHING
    {
        image_modified = cv::Mat(image_original.rows, image_original.cols, CV_8UC3);
        cv::cvtColor(image_original, image_modified, cv::COLOR_BGR2HLS);

        ContrastStretching(static_cast<uchar>(contrast_stretching_min),
                           static_cast<uchar>(contrast_stretching_max));

        cv::cvtColor(image_modified, image_modified, cv::COLOR_HLS2BGR);
    }

    cv::imshow(kNameWindowModified, image_modified);
}

/* Code below was taken from https://stackoverflow.com/a/13558570 */

// Inverse of sRGB "gamma" function. (approx 2.2)
double inv_gam_sRGB(int ic) {
    double c = ic / 255.0;

    if (c <= 0.04045) {
        return c / 12.92;
    }
    else
    {
        return pow(((c + 0.055) / (1.055)), 2.4);
    }    
}

// sRGB "gamma" function (approx 2.2)
int gam_sRGB(double v)
{
    if (v <= 0.0031308)
    {
        v *= 12.92;
    }   
    else
    {
        v = 1.055 * pow(v, 1.0 / 2.4) - 0.055;
    }

    // This is correct in C++. Other languages may not require +0.5.
    return int(v * 255 + 0.5);
}

// GRAY VALUE ("brightness")
int gray(uchar r, uchar g, uchar b)
{
    return gam_sRGB(
        rY * inv_gam_sRGB(r) +
        gY * inv_gam_sRGB(g) +
        bY * inv_gam_sRGB(b)
    );
}