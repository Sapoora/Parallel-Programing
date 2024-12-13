### **Project Description**

The project explores computationally intensive tasks through **parallel programming** using OpenMP. It includes three tasks:

1. **Mandelbrot Set Simulation:**
   - **Overview:** The Mandelbrot Set is a famous fractal, defined by iterating the equation ( z = z^2 + c ) (where  z  and  c  are complex numbers). The task involves:
     - Simulating this fractal by iterating the equation.
     - Visualizing zoom-in and zoom-out effects.
     - Testing convergence for different values of  z  and  c .
     - Applying parallel programming to improve performance.

2. **Julia Set Simulation:**
   - **Overview:** Similar to the Mandelbrot Set, the Julia Set uses a fixed  c  value while varying  z . The task involves:
     - Generating patterns for different  c  values.
     - Visualizing the results.
     - Improving performance using OpenMP.

3. **Estimating π Using the Monte Carlo Method:**
   - **Overview:** The Monte Carlo method involves generating random points in a square, then determining how many lie within a circle inscribed in the square. Using this ratio, \( \pi \) can be estimated. This task requires:
     - Efficient random point generation.
     - Calculating the ratio of points inside the circle to the total points.
     - Parallelizing the process for better efficiency.

---

### **Tools and Technologies**

1. **Programming Languages:**
   - **C/C++:** The tasks must be implemented in C or C++.

2. **Parallel Programming Framework:**
   - **OpenMP:** An API for multi-threaded parallel programming in C/C++ that allows splitting tasks into threads to enhance speed and performance.

3. **Operating Systems:**
   - Compatible with both **Linux** and **Windows**.

4. **Visualization Tools:**
   - For rendering the fractals (Mandelbrot and Julia Sets), libraries such as **OpenCV**, **SDL**, or even basic graphical libraries in C/C++ can be used.

5. **Mathematics:**
   - The Mandelbrot and Julia sets require knowledge of complex numbers.
   - Monte Carlo simulations need random number generation and statistical calculations.

6. **Compilers:**
   - Use compilers such as **gcc/g++** (on Linux) or **Visual Studio** (on Windows) with OpenMP support.

---

### **Implementation Notes**

- The **Mandelbrot and Julia Sets**:
  - Requires iterating over a grid of complex numbers and checking convergence for each point.
  - Results are rendered as pixel colors based on the number of iterations before divergence.
  - Use OpenMP to distribute grid computations across threads.

- **Monte Carlo Method for \( \pi \):**
  - Generate random points and check whether each lies inside the circle.
  - Use OpenMP to parallelize the point generation and checking process.

- **Performance Analysis:**
  - Measure and report **speedup** by comparing serial and parallel implementations.
  - Optimize the code for efficient parallel execution.
  - the performance speedup is a reasonably good speedup.
---
