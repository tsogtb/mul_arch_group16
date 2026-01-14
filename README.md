# Parallel Numerical Integration (OpenMP vs. Pthreads)

This project explores high-performance computing (HPC) techniques to optimize numerical and stochastic integration. Specifically, it implements **Monte Carlo Pi estimation** and **Numerical Integral approximation** using parallel processing to achieve significant speedup.

## 🚀 Performance Strategy

The implementation follows a three-phase execution model to minimize overhead and maximize thread utilization:

1.  **Phase 1: Serial Initialization** Argument parsing, environment setup, and thread count configuration.

2.  **Phase 2: Parallel Computation** * **Stochastic Independence:** Each thread maintains its own random number generator to avoid mutex contention and shared-seed bottlenecks.
    * **Workload Distribution:** Utilizes OpenMP's static scheduling for deterministic load balancing across available cores.
    * **Synchronization:** Employs `atomic` updates for global counters to prevent race conditions while maintaining high throughput compared to standard critical sections.

3.  **Phase 3: Serial Reduction** Final arithmetic operations to produce the estimation results from gathered thread data.



## 📊 Complexity Analysis

The execution time follows the ideal parallel scaling law (assuming $P$ processors are available for $T$ threads):

$$T(n, p) \approx O\left(\frac{\text{samples}}{\text{threads}}\right)$$

### Performance-Critical Operations:
* **Thread-local RNG:** Eliminates the bottleneck of shared memory access during random number generation.
* **Atomic Operations:** Minimizes the performance penalty during the "pcount" reduction phase.

## 🛠 Tech Stack
* **Language:** C
* **Parallelism:** OpenMP (Directives), Pthreads (Comparison)
* **Analysis:** LaTeX for performance reporting and mathematical verification

## 📈 OpenMP vs. Pthreads Analysis
As part of the study, we compared the high-level abstractions of OpenMP against the manual control provided by Pthreads:
* **OpenMP:** Highly efficient for homogeneous data operations and parallel loops due to its directive-based semantics.
* **Pthreads:** While requiring manual scheduling and synchronization primitives (mutexes), it offers finer control over thread attributes and heterogeneous task execution.

---
*Developed as part of the CS-214: Parallelism and Concurrency course at EPFL.*
