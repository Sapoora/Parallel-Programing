#include <iostream>
#include <string>
#include <smmintrin.h>  // For SSE4.1
#include <emmintrin.h>  // For SSE2 intrinsics
#include <chrono>        // For chrono

using namespace std;


// Serial RLE Compression
string rle_compress_serial(const string& input) {
    string result = "";
    int n = input.length();
    for (int i = 0; i < n; i++) {
        int count = 1;
        while (i + 1 < n && input[i] == input[i + 1]) { // Avoid out-of-bounds
            count++;
            i++;
        }
        result += input[i];
        result += to_string(count);  // Concatenate count properly as a string
    }
    return result;
}

string rle_compress_simd(const string& input) {
    string result = "";
    int n = input.length();
    int i = 0;

    // Process input in chunks of 16 characters (128-bit blocks)
    for (i; i <= n - 16; i += 16) {
        // Load 16 bytes (characters) into an SSE register
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(input.c_str() + i));

        // Compare each byte with the previous byte
        __m128i cmp = _mm_cmpeq_epi8(chunk, _mm_srli_si128(chunk, 1)); // Compare each byte with the one next to it

        int count = 1;  // count is the length of the current run
        char last_char = input[i];  // the character being counted
        
        for (int j = 1; j < 16; j++) {
            // Extract the comparison result into a bitmask
            int mask = _mm_movemask_epi8(cmp);
            
            // Check the specific bit corresponding to the current position
            if ((mask & (1 << j)) == 0) {
                // A new character or different run, record the previous run
                result += last_char;
                result += to_string(count);  // Use to_string for count
                
                // Start counting the new run
                last_char = input[i + j];
                count = 1;
            } else {
                count++;
            }
        }

        // After finishing the loop, add the last run (16th character)
        result += last_char;
        result += to_string(count);
    }

    // Handle remaining characters (less than 16 characters left)
    for (; i < n; i++) {
        int count = 1;
        while (i + 1 < n && input[i] == input[i + 1]) {  // Avoid out-of-bounds
            count++;
            i++;
        }
        result += input[i];
        result += to_string(count);  // Properly append the count as a string
    }

    return result;
}

// Calculate compression ratio
double calculate_compression_ratio(const string& original, const string& compressed) {
    double original_size = original.size();
    double compressed_size = compressed.size();
    return original_size / compressed_size;
}

int main() {
    string input;
    cout << "Enter the string to compress: ";
    cin >> input;

   // Measure time for serial compression
    auto start_serial = chrono::high_resolution_clock::now();
    string compressed_serial = rle_compress_serial(input);
    auto end_serial = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_serial = end_serial - start_serial;

    // Measure time for parallel compression
    auto start_parallel = chrono::high_resolution_clock::now();
    string compressed_parallel = rle_compress_simd(input);
    auto end_parallel = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_parallel = end_parallel - start_parallel;

    // Calculate compression ratio
    double ratio_serial = calculate_compression_ratio(input, compressed_serial);
    double ratio_parallel = calculate_compression_ratio(input, compressed_parallel);

    // Calculate speedup
    double speedup = duration_serial.count() / duration_parallel.count();

    // Output results
    cout << "\nSerial Compression:\n";
    cout << "Compressed string: " << compressed_serial << endl;
    cout << "Compression ratio: " << ratio_serial << endl;
    cout << "Time taken: " << duration_serial.count() << " seconds\n";

    cout << "\nParallel Compression:\n";
    cout << "Compressed string: " << compressed_parallel << endl;
    cout << "Compression ratio: " << ratio_parallel << endl;
    cout << "Time taken: " << duration_parallel.count() << " seconds\n";

    cout << "\nSpeedup: " << speedup << "\n";

    return 0;
}
