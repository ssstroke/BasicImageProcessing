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

static void ContrastStretching(const cv::Mat& , cv::Mat&, const uchar, const uchar);
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
		cv::Mat image_original = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

		if (image_original.empty())
		{
			std::cout << "Could not open or find the image" << std::endl;

			exit_value = kInvalidInput;
			goto Exit;
		}
		else
		{
			cv::Mat image_modified;

			const uchar kResultBrightnessMin = 0;
			const uchar kResultBrightnessMax = 255;
			ContrastStretching(image_original, image_modified,
				               kResultBrightnessMin, kResultBrightnessMax);

			cv::imshow("Original", image_original);
			cv::imshow("Modified", image_modified);

			cv::waitKey(0);
		}
	}

Exit:
	return exit_value;
}

static void ContrastStretching(const cv::Mat& image_original, cv::Mat& image_modified,
	                           const uchar kOutMin, const uchar kOutMax)
{
	image_modified = image_original.clone();

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