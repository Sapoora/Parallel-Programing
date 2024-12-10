#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <emmintrin.h>
#include <x86intrin.h>
#include <chrono>

// SSE-based absolute difference
void absDiff_SIMD(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst)
{
    if (src1.size() != src2.size() || src1.type() != CV_8U || src2.type() != CV_8U)
    {
        std::cerr << "Input matrices must have the same size and be of type CV_8U." << std::endl;
        return;
    }
    dst.create(src1.rows, src1.cols, CV_8U);

    for (int row = 0; row < src1.rows; ++row)
    {
        const uchar *ptrSrc1 = src1.ptr<uchar>(row);
        const uchar *ptrSrc2 = src2.ptr<uchar>(row);
        uchar *ptrDst = dst.ptr<uchar>(row);

        for (int col = 0; col < src1.cols; col += 16)
        {
            __m128i xmm1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(ptrSrc1));
            __m128i xmm2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(ptrSrc2));
            __m128i xmmDiff = _mm_sub_epi8(xmm1, xmm2);
            xmmDiff = _mm_abs_epi8(xmmDiff);
            _mm_storeu_si128(reinterpret_cast<__m128i *>(ptrDst), xmmDiff);

            ptrSrc1 += 16;
            ptrSrc2 += 16;
            ptrDst += 16;
        }
    }
}

// Serial absolute difference for comparison
void absDiff_Serial(const cv::Mat &src1, const cv::Mat &src2, cv::Mat &dst)
{
    if (src1.size() != src2.size() || src1.type() != CV_8U || src2.type() != CV_8U)
    {
        std::cerr << "Input matrices must have the same size and be of type CV_8U." << std::endl;
        return;
    }
    dst.create(src1.rows, src1.cols, CV_8U);

    for (int row = 0; row < src1.rows; ++row)
    {
        const uchar *ptrSrc1 = src1.ptr<uchar>(row);
        const uchar *ptrSrc2 = src2.ptr<uchar>(row);
        uchar *ptrDst = dst.ptr<uchar>(row);

        for (int col = 0; col < src1.cols; ++col)
        {
            uchar pixel1 = ptrSrc1[col];
            uchar pixel2 = ptrSrc2[col];
            ptrDst[col] = std::abs(static_cast<int>(pixel1) - static_cast<int>(pixel2));
        }
    }
}

int main()
{
    // Open video file
    cv::VideoCapture cap("/home/atefeh/PP/PP-CA1-Fall03/assets/Q4/Q4.mp4");
    if (!cap.isOpened())
    {
        std::cerr << "Error opening video file" << std::endl;
        return -1;
    }

    cv::Mat frame, grayFrame, prevGrayFrame, motionFrame;
    bool isFirstFrame = true;

    // Create VideoWriter object to save the processed video
    int fourcc = cv::VideoWriter::fourcc('a', 'v', 'c', '1'); // H264 codec for MP4
    double fps = cap.get(cv::CAP_PROP_FPS); // Get the original FPS of the video
    cv::Size frameSize(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT)); // Get frame size
    cv::VideoWriter SIMDVideo("simd_output.mp4", fourcc, fps, frameSize, false); // 'false' for grayscale video
    cv::VideoWriter SerialVideo("simd_output.mp4", fourcc, fps, frameSize, false); // 'false' for grayscale video

    // Measure time for SIMD
    auto startSIMD = std::chrono::high_resolution_clock::now();

    // Process and show SIMD motion frames first
    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // Convert to grayscale for processing
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        if (!isFirstFrame)
        {
            // Compute absolute difference between current and previous frames (SIMD)
            absDiff_SIMD(grayFrame, prevGrayFrame, motionFrame);

            // Resize the motion frame to fit the window size (640x480)
            cv::resize(motionFrame, motionFrame, cv::Size(640, 480));

            // Write the frame to the output video
            SIMDVideo.write(motionFrame);

            // Show the motion frame for SIMD
            cv::imshow("Motion Frame - SIMD", motionFrame);
            if (cv::waitKey(30) >= 0)
                break;
        }

        // Update the previous frame
        grayFrame.copyTo(prevGrayFrame);
        isFirstFrame = false;
    }

    // Measure time after SIMD
    auto endSIMD = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationSIMD = endSIMD - startSIMD;
    std::cout << "Time for SIMD processing: " << durationSIMD.count() << " seconds" << std::endl;

    // Reset for the serial computation and display
    cap.set(cv::CAP_PROP_POS_FRAMES, 0);
    isFirstFrame = true;

    // Measure time for serial
    auto startSerial = std::chrono::high_resolution_clock::now();

    // Process and show serial motion frames
    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // Convert to grayscale for processing
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        if (!isFirstFrame)
        {
            // Compute absolute difference between current and previous frames (serial)
            absDiff_Serial(grayFrame, prevGrayFrame, motionFrame);

            // Resize the motion frame to fit the window size (640x480)
            cv::resize(motionFrame, motionFrame, cv::Size(640, 480));

            // Write the frame to the output video
            SerialVideo.write(motionFrame); // Optionally write serial motion frames too

            // Show the motion frame for serial
            cv::imshow("Motion Frame - Serial", motionFrame);
            if (cv::waitKey(30) >= 0)
                break;
        }

        // Update the previous frame
        grayFrame.copyTo(prevGrayFrame);
        isFirstFrame = false;
    }

    // Measure time after serial
    auto endSerial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationSerial = endSerial - startSerial;
    std::cout << "Time for serial processing: " << durationSerial.count() << " seconds" << std::endl;

    // Calculate and display speedup
    double speedup = durationSerial.count() / durationSIMD.count();
    std::cout << "Speedup (Serial / SIMD): " << speedup << "x" << std::endl;

    // Release resources
    cap.release();
    SIMDVideo.release();
    SerialVideo.release();
    cv::destroyAllWindows();

    return 0;
}
