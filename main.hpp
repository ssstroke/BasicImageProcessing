#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#define VEC_INDEX_RED	2
#define VEC_INDEX_GREEN	1
#define VEC_INDEX_BLUE	0

#define OPTION_THRESHOLDING                 '1'
#define OPTION_LINEAR_CONTRAST_STRETCHING   '2'
#define OPTION_SOLARISING                   '3'

typedef enum ErrorCode {
    kOk,
    kMissingArgument,
    kInvalidInput
} ErrorCode;

/* Primary functions. */
static void Thresholding(const uchar);
static void ContrastStretching(const uchar, const uchar);
static void Solarisation(void);

/* Additional functions. */
static std::pair<uchar, uchar> GetMinAndMaxBrightnessValues(const cv::Mat&);

/* Necessary for OpenCV trackbar. */
static void TrackbarCallback(int, void*);

cv::Mat image_original;
cv::Mat image_modified;

const cv::String kNameWindowModified = cv::String("Modified");
const cv::String kNameWindowOriginal = cv::String("Original");

const cv::String kTrackbarNameThreshold = cv::String("Threshold");
const cv::String kTrackbarNameConstastStretchingMin = cv::String("Lower");
const cv::String kTrackbarNameConstastStretchingMax = cv::String("Upper");

int contrast_stretching_min = 0;
int contrast_stretching_max = 255;

#endif // !GRAPHICS_H_