#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <emmintrin.h>
#include <x86intrin.h>
#include <chrono> // Include chrono header for timing

using namespace cv;
using namespace std;


void mergePhotosWeighted_SIMD(Mat &src1, Mat &src2, Mat &dst, float alpha)
{
    // Ensure input matrices have the same size and are of type CV_8UC3
    if (src1.size() != src2.size() || src1.type() != CV_8UC3 || src2.type() != CV_8UC3)
    {
        std::cerr << "Input matrices must have the same size and be of type CV_8UC3." << std::endl;
        return;
    }

    dst.create(src1.rows, src1.cols, CV_8UC3);

    __m128 alphaVec = _mm_set1_ps(alpha);  // Set the alpha value for scaling

    for (int row = 0; row < src1.rows; ++row)
    {
        const uchar *ptrSrc1 = src1.ptr<uchar>(row);
        const uchar *ptrSrc2 = src2.ptr<uchar>(row);
        uchar *ptrDst = dst.ptr<uchar>(row);

        for (int col = 0; col < src1.cols; col += 4)
        {
            // Load 4 pixels (16 bytes, 3 channels each pixel) from the source images
            __m128i xmm1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(ptrSrc1 + col * 3));
            __m128i xmm2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(ptrSrc2 + col * 3));

            // Separate the 16-byte values (4 pixels * 3 channels = 12 values)
            __m128i src1_r = _mm_and_si128(xmm1, _mm_set1_epi32(0xFF));  // Red channel
            __m128i src1_g = _mm_and_si128(_mm_srli_si128(xmm1, 1), _mm_set1_epi32(0xFF));  // Green channel
            __m128i src1_b = _mm_and_si128(_mm_srli_si128(xmm1, 2), _mm_set1_epi32(0xFF));  // Blue channel

            __m128i src2_r = _mm_and_si128(xmm2, _mm_set1_epi32(0xFF));  // Red channel
            __m128i src2_g = _mm_and_si128(_mm_srli_si128(xmm2, 1), _mm_set1_epi32(0xFF));  // Green channel
            __m128i src2_b = _mm_and_si128(_mm_srli_si128(xmm2, 2), _mm_set1_epi32(0xFF));  // Blue channel

            // Convert to float and apply alpha scaling
            __m128 src1_r_f = _mm_cvtepi32_ps(src1_r);
            __m128 src1_g_f = _mm_cvtepi32_ps(src1_g);
            __m128 src1_b_f = _mm_cvtepi32_ps(src1_b);

            __m128 src2_r_f = _mm_cvtepi32_ps(src2_r);
            __m128 src2_g_f = _mm_cvtepi32_ps(src2_g);
            __m128 src2_b_f = _mm_cvtepi32_ps(src2_b);

            src2_r_f = _mm_mul_ps(src2_r_f, alphaVec);
            src2_g_f = _mm_mul_ps(src2_g_f, alphaVec);
            src2_b_f = _mm_mul_ps(src2_b_f, alphaVec);

            // Add the scaled values and convert back to int
            __m128i result_r = _mm_cvtps_epi32(_mm_add_ps(src1_r_f, src2_r_f));
            __m128i result_g = _mm_cvtps_epi32(_mm_add_ps(src1_g_f, src2_g_f));
            __m128i result_b = _mm_cvtps_epi32(_mm_add_ps(src1_b_f, src2_b_f));

            // Clamp the values to the 0-255 range using _mm_min_epu8
            result_r = _mm_min_epu8(result_r, _mm_set1_epi32(255));
            result_g = _mm_min_epu8(result_g, _mm_set1_epi32(255));
            result_b = _mm_min_epu8(result_b, _mm_set1_epi32(255));

            // Pack the results back into a single __m128i and store
            __m128i merged = _mm_or_si128(
                _mm_or_si128(
                    _mm_slli_si128(result_b, 2),  // Shift blue to the right position
                    _mm_slli_si128(result_g, 1)   // Shift green to the right position
                ),
                result_r                       // Red stays in the same position
            );

            _mm_storeu_si128(reinterpret_cast<__m128i *>(ptrDst + col * 3), merged);
        }
    }
}


void mergePhotosWeighted_Serial(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst, float alpha)
{
    // Ensure input matrices have the same size and are of type CV_8U (color images)
    if (src1.size() != src2.size() || src1.type() != CV_8UC3 || src2.type() != CV_8UC3)
    {
        std::cerr << "Input matrices must have the same size and be of type CV_8UC3." << std::endl;
        return;
    }

    dst.create(src1.rows, src1.cols, CV_8UC3);

    for (int row = 0; row < src1.rows; ++row)
    {
        const uchar *ptrSrc1 = src1.ptr<uchar>(row);
        const uchar *ptrSrc2 = src2.ptr<uchar>(row);
        uchar *ptrDst = dst.ptr<uchar>(row);

        for (int col = 0; col < src1.cols; ++col)
        {
            for (int channel = 0; channel < 3; ++channel)
            {
                int pixel1 = static_cast<int>(ptrSrc1[col * 3 + channel]);
                int pixel2 = static_cast<int>(ptrSrc2[col * 3 + channel]);

                // Multiply pixel2 by alpha (scaling the second image)
                pixel2 = static_cast<int>(pixel2 * alpha);

                // Perform addition with saturation
                int merged = pixel1 + pixel2;
                if (merged > 255)
                {
                    merged = 255; // Saturate to 8-bit maximum
                }

                // Store the merged pixel value in the output image
                ptrDst[col * 3 + channel] = static_cast<uchar>(merged);
            }
        }
    }
}

int main()
{
    using namespace std::chrono; // For time tracking

    // Load the two input images (color images)
    Mat image1 = imread("/home/atefeh/PP/PP-CA1-Fall03/assets/Q1/front.png", IMREAD_COLOR);
    Mat image2 = imread("/home/atefeh/PP/PP-CA1-Fall03/assets/Q1/logo.png", IMREAD_COLOR); // Ensure this path is correct
    Mat merged_par;
    Mat merged_ser;

    // Create a new image 'c' with the same size as 'a'
    Mat image2_scaled(image1.rows, image1.cols, image1.type(), Scalar(0, 0, 0)); // Initialize with empty (black) pixels

    // Copy the content of 'image2' into 'image2_scaled' at the appropriate position
    image2.copyTo(image2_scaled(Rect(0, 0, image2.cols, image2.rows)));

    // Multiply the second image's pixel values by 0.625 (weighting factor)
    float alpha = 0.625;
    for (int row = 0; row < image2_scaled.rows; ++row)
    {
        uchar *ptr = image2_scaled.ptr<uchar>(row);
        for (int col = 0; col < image2_scaled.cols; ++col)
        {
            for (int channel = 0; channel < 3; ++channel)
            {
                ptr[col * 3 + channel] = static_cast<uchar>(ptr[col * 3 + channel] * alpha);
            }
        }
    }

    // Start the timer for parallel execution
    auto start = high_resolution_clock::now();
    // Merge the two photos using SIMD instructions, with a weight of 0.625 for the second image
    mergePhotosWeighted_SIMD(image1, image2_scaled, merged_par, 0.625); // Set alpha to 1 as the scaling is done already
    // End the timer for parallel execution
    auto end = high_resolution_clock::now();
    auto timeParallel = duration_cast<microseconds>(end - start).count();

    // Start the timer for serial execution
    start = high_resolution_clock::now();
    // Merge the two photos using serial code, with a weight of 1 for the second image
    mergePhotosWeighted_Serial(image1, image2_scaled, merged_ser, 0.625); // Set alpha to 1 as the scaling is done already
    // End the timer for serial execution
    end = high_resolution_clock::now();
    auto timeSerial = duration_cast<microseconds>(end - start).count();

    // Saving output images
    imwrite("out_par.png", merged_par);
    imwrite("out_ser.png", merged_ser);

    printf("\nSerial Run time = %ld microseconds\n", timeSerial);
    printf("\nParallel Run time = %ld microseconds\n", timeParallel);
    printf("\tSpeedup = %f\n\n", (float)(timeSerial) / (float)(timeParallel));

    return 0;
}
