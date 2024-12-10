#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <chrono> // Include chrono for timing

#define ARRAY_SIZE (1 << 20) // 2^20 (1,048,576) elements
#define THRESHOLD 1.5

// Serial implementation to calculate mean and standard deviation
void meanAndSTD_Serial(float *array, int size, float *mean, float *STDev)
{
    float temp, temp_mean, square, subb;
    temp = 0.0f;
    for (int i = 0; i < size; i++)
        temp += array[i];

    temp_mean = temp / size;
    *mean = temp_mean;
    temp = 0.0f;

    for (int i = 0; i < size; i++)
    {
        subb = array[i] - temp_mean;
        square = subb * subb;
        temp += square;
    }

    *STDev = sqrt(temp / size);
}

// Serial implementation to count outliers based on Z-Score
int countOutliers_Serial(float *array, int size, float mean, float stddev)
{
    int outliers = 0;
    for (int i = 0; i < size; i++)
    {
        float zScore = fabs((array[i] - mean) / stddev);
        if (zScore > THRESHOLD)
        {
            outliers++;
        }
    }
    return outliers;
}
// Parallel implementation to calculate mean and standard deviation using SSE
void meanAndSTD_Parallel(float *array, int size, float *mean, float *STDev)
{
    __m128 temp_mean;
    __m128 vec, A, B;
    __m128 temp = _mm_set1_ps(0.0f);
    for (int i = 0; i < size; i += 4)
    {
        vec = _mm_loadu_ps(&array[i]);
        temp = _mm_add_ps(temp, vec);
    }
    temp = _mm_hadd_ps(temp, temp);
    temp = _mm_hadd_ps(temp, temp);
    *mean = _mm_cvtss_f32(temp) / size;
    temp_mean = _mm_set1_ps(*mean);

    temp = _mm_set1_ps(0.0f);
    for (int i = 0; i < size; i += 4)
    {
        vec = _mm_loadu_ps(&array[i]);
        A = _mm_sub_ps(vec, temp_mean);
        B = _mm_mul_ps(A, A);
        temp = _mm_add_ps(temp, B);
    }
    temp = _mm_hadd_ps(temp, temp);
    temp = _mm_hadd_ps(temp, temp);
    *STDev = sqrt(_mm_cvtss_f32(temp) / size);
}

// Custom function for absolute value (SSE)
// Corrected custom function for absolute value (SSE)
__m128 _mm_abs_ps_custom(__m128 x)
{
    __m128 signMask = _mm_set1_ps(-0.0f); // Set a mask to isolate the sign bit
    return _mm_andnot_ps(signMask, x);    // Mask out the sign bit
}

// Parallel implementation to count outliers based on Z-Score using SSE
int countOutliers_Parallel(float *array, int size, float mean, float stddev)
{
    __m128 meanVec = _mm_set1_ps(mean);     // Set mean value for all elements
    __m128 stddevVec = _mm_set1_ps(stddev); // Set stddev value for all elements
    int outliers = 0;
    int i = 0;

    // Process 4 elements at a time using SSE
    for (i; i + 3 < size; i += 4)
    {
        __m128 data = _mm_loadu_ps(&array[i]); // Load 4 values from the array

        // Subtract mean and divide by stddev (Z-Score)
        __m128 zScores = _mm_sub_ps(data, meanVec);
        zScores = _mm_div_ps(zScores, stddevVec);

        // Use custom absolute value function
        __m128 absZScores = _mm_abs_ps_custom(zScores);

        // Debugging: Print Z-scores
        float temp[4];
        _mm_storeu_ps(temp, absZScores);
        // printf("Z-Scores: %f %f %f %f\n", temp[0], temp[1], temp[2], temp[3]);

        // Compare Z-Score with 2.5 and count outliers
        __m128 threshold = _mm_set1_ps(THRESHOLD);
        __m128 cmp = _mm_cmpgt_ps(absZScores, threshold); // Compare abs(Z) > 2.5

        // Count outliers by summing the results
        int mask = _mm_movemask_ps(cmp);
        outliers += __builtin_popcount(mask); // Count the number of 1's in the mask
    }

    // Handle the remaining elements (less than 4 elements)
    for (; i < size; ++i)
    {
        float zScore = fabs((array[i] - mean) / stddev);
        if (zScore > 2.5)
        {
            ++outliers;
        }
    }

    return outliers;
}

int main(void)
{
    using namespace std::chrono; // Use std::chrono for time measurements

    float mean_ser, sigma_ser;
    float mean_par, sigma_par;

    // Allocate a large array of floats
    float *array = (float *)malloc(ARRAY_SIZE * sizeof(float));

    // Initialize the array with random values (for demonstration)
    srand(time(NULL));

    // Random float from a truly unrestricted range
    // for (int i = 0; i < ARRAY_SIZE; i++)
    // {
    //     array[i] = (float)rand() / (float)RAND_MAX * RAND_MAX; // Unbounded random float
    // }

    // Generate random float values between -1,000,000 and 1,000,000 for more spread
for (int i = 0; i < ARRAY_SIZE; i++) {
    array[i] = (float)(rand() % 2000001) - 1000000.0f; // Random values between -1,000,000 and 1,000,000
}


    // Serial implementation for mean, stddev, and outliers count
    auto start = high_resolution_clock::now();
    meanAndSTD_Serial(array, ARRAY_SIZE, &mean_ser, &sigma_ser);
    int outliersSerial = countOutliers_Serial(array, ARRAY_SIZE, mean_ser, sigma_ser);
    auto end = high_resolution_clock::now();
    auto timeSerial = duration_cast<microseconds>(end - start).count();

    // Parallel implementation for mean, stddev, and outliers count
    start = high_resolution_clock::now();
    meanAndSTD_Parallel(array, ARRAY_SIZE, &mean_par, &sigma_par);
    int outliersParallel = countOutliers_Parallel(array, ARRAY_SIZE, mean_par, sigma_par);
    end = high_resolution_clock::now();
    auto timeParallel = duration_cast<microseconds>(end - start).count();

    printf("\nSerial Result  : \n    mean = %f \n    sigma = %f\n", mean_ser, sigma_ser);
    printf("Outliers (Serial): %d\n", outliersSerial);

    printf("\nParallel Result: \n    mean = %f \n    sigma = %f\n", mean_par, sigma_par);
    printf("Outliers (Parallel): %d\n", outliersParallel);

    printf("\nSerial Run time = %ld microseconds\n", timeSerial);
    printf("Parallel Run time = %ld microseconds\n", timeParallel);
    printf("\tSpeedup = %f\n\n", (float)(timeSerial) / (float)timeParallel);

    // Clean up
    free(array);

    return 0;
}
