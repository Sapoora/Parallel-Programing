
### **Project Description**

This project is designed to help us practice and learn parallel programming techniques using real-world computational tasks. The tasks focus on image processing, numerical analysis, data compression, and motion detection in video frames. Each task combines theoretical concepts with practical coding implementations. Here's a breakdown:

1. **Image Blending**:
   - **Objective**: Blend a university logo into a given image using a predefined formula. This involves pixel-by-pixel manipulation to integrate the logo with the image.
   - **Challenges**: Efficient manipulation of large images and ensuring accurate positioning of the logo.

2. **Outlier Detection in Data Arrays**:
   - **Objective**: Identify outliers in an array of floating-point numbers using the z-score formula:
     \[
     z = \frac{x - \text{mean}}{\sigma}
     \]
     Any data point with \( z > 2.5 \) is classified as an outlier.
   - **Challenges**: Efficient computation of the mean and standard deviation, especially for large datasets.

3. **Run-Length Encoding (RLE) Compression**:
   - **Objective**: Implement RLE, a data compression technique that reduces the size of a string by replacing repeated characters with a single character followed by the count of repetitions.
   - **Challenges**: Handle edge cases (e.g., very short or long runs) and calculate the compression ratio effectively:
     \[
     \text{Compression Ratio} = \frac{\text{Uncompressed Size}}{\text{Compressed Size}}
     \]

4. **Motion Detection in Video Frames**:
   - **Objective**: Detect motion between consecutive video frames using the formula:
     \[
     \text{Motion Frame} = |\text{Current Frame} - \text{Previous Frame}|
     \]
   - **Challenges**: Process frames in real-time and manage computational load.

---

### **Tools and Technologies**

1. **Programming Languages**:
   - **C/C++**: Core languages for high-performance implementations.
   - Used for both serial and parallel versions of the tasks.

2. **Frameworks and Libraries**:
   - **OpenCV**:
     - For image processing and video manipulation.
     - Handles tasks like loading, editing, and analyzing images and video frames.
   - **Standard Template Library (STL)** in C++:
     - For data manipulation and mathematical operations.

3. **Parallel Programming Concepts**:
   - **SIMD Instructions**:
     - Use SIMD (Single Instruction, Multiple Data) techniques like SSE (Streaming SIMD Extensions) to optimize parallel processing.
   - **Parallel vs. Serial Execution**:
     - Compare performance by implementing both serial and parallel versions of the tasks.

4. **Development Environment**:
   - Compatible with **Windows** and **Linux** systems.
   - Use any IDE or text editor that supports C/C++ (e.g., Visual Studio, CLion, Code::Blocks).

5. **Execution Requirements**:
   - Implement both serial and parallel versions of the code.
   - Generate a detailed report explaining:
     - The achieved **speedup** in parallel execution.
     - Justifications for chosen optimizations.

---

### **Submission Requirements**

1. **Code**:
   - Fully functional implementations for all tasks.
   - Include a serial and optimized parallel version of each.

2. **Report**:
   - Discuss your approach, results, and conclusions.
   - Include speedup ratios, insights on performance, and lessons learned.

3. **Environment**:
   - Ensure your program compiles and runs on Linux or Windows.
   - Clearly document the compilation and execution steps.

4. **Collaboration**:
   - If working in a group, include the names and IDs of all contributors.

This project integrates theoretical knowledge with practical application, fostering a deeper understanding of parallel programming techniques while emphasizing performance optimization.
