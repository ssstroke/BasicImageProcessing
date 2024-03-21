#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#define VEC_INDEX_RED	2
#define VEC_INDEX_GREEN	1
#define VEC_INDEX_BLUE	0

#define OPTION_THRESHOLDING                 '1'
#define OPTION_LINEAR_CONTRAST_STRETCHING   '2'
#define OPTION_SOLARISING                   '3'

typedef enum ErrorCode {
    kOk,
    kMissingArgument,
    kInvalidInput,
    kInvalidOption
} ErrorCode;

cv::Mat image_original;
cv::Mat image_modified;

const cv::String kNameWindowModified = cv::String("Modified");
const cv::String kNameWindowOriginal = cv::String("Original");

const cv::String kTrackbarNameThreshold = cv::String("Threshold");
const cv::String kTrackbarNameConstastStretchingMin = cv::String("Lower");
const cv::String kTrackbarNameConstastStretchingMax = cv::String("Upper");

int contrast_stretching_min = 0;
int contrast_stretching_max = 255;

/* Primary functions. */
static void Thresholding(const uchar);
static void ContrastStretching(const uchar, const uchar);
static void Solarisation(void);

/* Additional functions. */
static std::pair<uchar, uchar> GetMinAndMaxBrightnessValues(const cv::Mat&);
static uchar GetPixelBrigthnessRGB(const cv::Mat& image, const int row, const int col);

/* Necessary for OpenCV trackbar. */
static void TrackbarCallback(int, void*);

/* Code below was taken from: https://stackoverflow.com/a/13558570 */

// sRGB luminance(Y) values
const double rY = 0.212655;
const double gY = 0.715158;
const double bY = 0.072187;

// Inverse of sRGB "gamma" function. (approx 2.2)
double inv_gam_sRGB(int ic);

// sRGB "gamma" function (approx 2.2)
int gam_sRGB(double v);

// GRAY VALUE ("brightness")
int gray(uchar r, uchar g, uchar b);

#endif // !GRAPHICS_H_