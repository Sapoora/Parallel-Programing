#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;


const float constant_real = 0.355;
const float constant_imag = 0.355;
const int MAX_ITERATIONS = 1000;

void apply_color(int iteration, int max_iteration, unsigned char &r, unsigned char &g, unsigned char &b)
{
    if (iteration == max_iteration)
    {
        r = g = b = 0;
    }
    else
    {
        float t = (float)iteration / max_iteration;
        r = (unsigned char)(9 * (1 - t) * t * t * t * 255);
        g = (unsigned char)(15 * (1 - t) * (1 - t) * t * t * 255);
        b = (unsigned char)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
    }
}

int julia(int width, int height, float x_min, float x_max, float y_min, float y_max, int x, int y)
{
    float real_part, imag_part;
    float temp, real_coord, imag_coord;

    real_coord = ((float)(width - x - 1) * x_min + (float)(x)*x_max) / (float)(width - 1);
    imag_coord = ((float)(height - y - 1) * y_min + (float)(y)*y_max) / (float)(height - 1);

    real_part = real_coord;
    imag_part = imag_coord;

    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++)
    {
        if (real_part * real_part + imag_part * imag_part > 4)
        {
            return iteration;
        }
        temp = real_part * real_part - imag_part * imag_part + constant_real;
        imag_part = 2 * real_part * imag_part + constant_imag;
        real_part = temp;
    }

    return MAX_ITERATIONS;
}

void generate_julia_set_parallel(int width, int height, float x_min, float x_max, float y_min, float y_max, vector<unsigned char> &rgb)
{
    int julia_value;
    int k;

#pragma omp parallel for private(julia_value, k) schedule(dynamic)
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            julia_value = julia(width, height, x_min, x_max, y_min, y_max, x, y);

            unsigned char r, g, b;
            apply_color(julia_value, MAX_ITERATIONS, r, g, b);

            k = 3 * (y * width + x);
            rgb[k] = r;
            rgb[k + 1] = g;
            rgb[k + 2] = b;
        }
    }
}

void generate_julia_set_serial(int width, int height, float x_min, float x_max, float y_min, float y_max, vector<unsigned char> &rgb)
{
    int julia_value;
    int k;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            julia_value = julia(width, height, x_min, x_max, y_min, y_max, x, y);

            unsigned char r, g, b;
            apply_color(julia_value, MAX_ITERATIONS, r, g, b);

            k = 3 * (y * width + x);
            rgb[k] = r;
            rgb[k + 1] = g;
            rgb[k + 2] = b;
        }
    }
}

void write_ppm_image(int width, int height, const vector<unsigned char> &rgb, const string &filename)
{
    FILE *file_unit = fopen(filename.c_str(), "wb");

    if (!file_unit)
    {
        cerr << "Error opening file " << filename << " for writing.\n";
        return;
    }

    fprintf(file_unit, "P6\n");
    fprintf(file_unit, "%d %d\n", width, height);
    fprintf(file_unit, "255\n");

    fwrite(rgb.data(), sizeof(unsigned char), 3 * width * height, file_unit);
    fclose(file_unit);
}

int main()
{
    int height = 800;
    int width = 800;
    double start_time, end_time, time_serial, time_parallel;
    float x_min = -2;
    float x_max = 2;
    float y_min = -2;
    float y_max = 2;

    cout << "Plot a version of the Julia set for Z(k+1) = Z(k)^2 "
         << (constant_real >= 0 ? "+ " : "- ") << abs(constant_real)
         << " + " << constant_imag << "i\n";

    vector<unsigned char> rgb(width * height * 3);

    start_time = omp_get_wtime();
    generate_julia_set_serial(width, height, x_min, x_max, y_min, y_max, rgb);
    end_time = omp_get_wtime();
    time_serial = end_time - start_time;
    cout << "Serial execution time: " << time_serial << " seconds\n";
    write_ppm_image(width, height, rgb, "julia_serial.ppm");

    start_time = omp_get_wtime();
    generate_julia_set_parallel(width, height, x_min, x_max, y_min, y_max, rgb);
    end_time = omp_get_wtime();
    time_parallel = end_time - start_time;
    cout << "Parallel execution time (OpenMP): " << time_parallel << " seconds\n";
    write_ppm_image(width, height, rgb, "julia_openmp.ppm");

    double speedup = time_serial / time_parallel;
    cout << "Speedup (Serial / Parallel): " << speedup << endl;

    return 0;
}
