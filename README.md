# Distributed Dataset Statistics using MPI

**Course:** Parallel and Grid Computing (PGC) – Lab Evaluation
**Theme:** 6 – Distributed Dataset Statistics
**Parallel Model:** MPI (Message Passing Interface)
**Team:** B1 Team 6
**Dataset size used:** N = 1000 (numbers 1 to 1000)

| Name | Roll No. |
|------|----------|
| Divya Kumari | 222 |
| Chaitanya M | 228 |
| Shridevi | 230 |
| Vineet K | 221 |

---

## 1. Objective

To calculate the **sum, average, maximum and minimum** of a dataset of **1000 numbers** using **4 MPI processes** on a cluster of one Master and three Worker VMs, and to compare the parallel result and workload with the sequential version.

## 2. Introduction (in simple words)

When a dataset is large, one computer takes a long time to process it. Instead, we can **divide the data into equal parts** and give each part to a different process. Every process works on its own part at the same time. At the end, the partial answers are **combined** into the final answer.

**MPI** is a library that lets many processes (on one or many computers) talk to each other by sending messages. Each process has a number called its **rank** (0, 1, 2, 3). Rank 0 is the **master**: it holds the data, sends parts to the others and collects the results. Every process has its **own memory**, so data must be sent explicitly.

## 3. Problem Definition

- **Input:** a dataset of N = 1000 integers (1, 2, 3, ..., 1000).
- **Output:** Sum, Average (= Sum / N), Maximum, Minimum.
- **Goal:** compute these using 4 MPI processes and check that the answer equals the sequential answer.

Expected answer (by formula): Sum = 1000 x 1001 / 2 = **500500**, Average = **500.50**, Max = **1000**, Min = **1**.

## 4. Algorithms

### 4.1 Sequential Algorithm
```
sum = 0; max = min = first element
for each number x in the dataset:
    sum = sum + x
    if x > max: max = x
    if x < min: min = x
average = sum / N
```
Time complexity: **O(N)**, all work done by one process.

### 4.2 Parallel Design (MPI)
1. **Initialise** MPI and get the rank and number of processes.
2. **Rank 0** creates the dataset (1 to 1000).
3. **Distribute:** `MPI_Scatter` gives each process an equal block of **1000 / 4 = 250** numbers.
4. **Local computation:** every process finds the sum, maximum and minimum of its own 250 numbers.
5. **Combine:** `MPI_Reduce` with `MPI_SUM`, `MPI_MAX` and `MPI_MIN` collects the partial results at rank 0.
6. **Final result:** rank 0 computes average = total sum / 1000, prints the statistics and the execution time.
7. **Finalise** MPI.

```
              Rank 0 holds the 1000 numbers
                      |  MPI_Scatter (250 numbers each)
     +----------+-----+-----+----------+
  Rank 0     Rank 1      Rank 2     Rank 3
  1-250     251-500     501-750    751-1000
 local calc  local calc  local calc local calc
     +----------+-----+-----+----------+
                      |  MPI_Reduce (SUM, MAX, MIN)
                   Rank 0 -> Sum, Average, Max, Min
```

### 4.3 MPI Functions Used

| Function | Purpose |
|----------|---------|
| `MPI_Init` / `MPI_Finalize` | Start and end the MPI environment |
| `MPI_Comm_rank` / `MPI_Comm_size` | Get the rank and the number of processes |
| `MPI_Scatter` | Divide the dataset equally among all processes |
| `MPI_Reduce` | Combine partial sum, max and min at rank 0 |
| `MPI_Wtime` | Measure execution time |

### 4.4 Work Done by Each Process

| Rank | Node | Numbers Received | Count | Local Sum |
|------|------|------------------|-------|-----------|
| 0 | master | 1 – 250 | 250 | 31375 |
| 1 | worker1 | 251 – 500 | 250 | 93875 |
| 2 | worker2 | 501 – 750 | 250 | 156375 |
| 3 | worker3 | 751 – 1000 | 250 | 218875 |
| **Combined** | | | **1000** | **500500** |

(Local sums are calculated from the block each rank receives. 31375 + 93875 + 156375 + 218875 = 500500.)

## 5. Environment / Cluster Setup

MPI runs many independent processes. Each process has its **own memory**, so data must be sent between processes explicitly. For this project, one Master VM and three Worker VMs are connected on the same virtual network.

### 5.1 Requirements
- VMware Workstation (or similar virtualization software)
- Four Ubuntu virtual machines (1 Master + 3 Workers) on the same virtual network
- OpenSSH Server and Open MPI installed on all nodes
- Passwordless SSH from Master to all Workers

### 5.2 Cluster Details

| Node | Hostname | IP Address | MPI Rank |
|------|----------|------------|----------|
| Master | master | 192.168.125.128 | 0 |
| Worker1 | worker1 | 192.168.125.129 | 1 |
| Worker2 | worker2 | 192.168.125.130 | 2 |
| Worker3 | worker3 | 192.168.125.131 | 3 |

| Item | Details |
|------|---------|
| OS | Ubuntu (VMware virtual machines) |
| MPI Library | Open MPI (`openmpi-bin`, `libopenmpi-dev`) |
| Compiler | `mpicc` (C language) |
| Processes | 4 (one per VM) |
| Hostfile | `hosts` |

### 5.3 Setup Steps

**Step 1 – Set a unique hostname (run on each VM, only its own name)**
```bash
sudo hostnamectl set-hostname master     # worker1 / worker2 / worker3 on the other VMs
```

**Step 2 – Find the IP address of each VM**
```bash
hostname -I
```

**Step 3 – Test network connectivity (on Master)**
```bash
ping -c 4 192.168.125.129
ping -c 4 192.168.125.130
ping -c 4 192.168.125.131
```
Expected: 4 packets sent, 4 received, 0% packet loss.

**Step 4 – Install SSH on every VM**
```bash
sudo apt update
sudo apt install openssh-server -y
sudo systemctl enable --now ssh
```

**Step 5 – Install Open MPI on every VM**
```bash
sudo apt update
sudo apt install openmpi-bin libopenmpi-dev -y
```

**Step 6 – Verify MPI on every VM**
```bash
mpicc --version
mpirun --version
```

**Step 7 – Create SSH key on Master**
```bash
ssh-keygen -t rsa
```

**Step 8 – Copy the key to the Workers (on Master)**
```bash
ssh-copy-id worker1
ssh-copy-id worker2
ssh-copy-id worker3
```

**Step 9 – Test passwordless SSH (on Master)**
```bash
ssh worker1 hostname
ssh worker2 hostname
ssh worker3 hostname
```
Expected output: `worker1`, `worker2`, `worker3` without asking for a password.

**Step 10 – Create the working directory and hostfile (on Master)**
```bash
mkdir -p ~/parallel_lab/mpi
cd ~/parallel_lab/mpi
nano hosts
```
Contents of `hosts`:
```
master slots=1
worker1 slots=1
worker2 slots=1
worker3 slots=1
```
The hostfile tells `mpirun` which machines take part in the run. `slots=1` means one process per machine.

## 6. Repository Structure

```
lab-evaluation-parallel-computing/
|-- README.md
|-- src/
|   |-- dataset_stats_sequential.c        # Sequential version (n = 1000)
|   |-- dataset_stats_sequential_timed.c  # Same, with timing (optional)
|   `-- dataset_stats_parallel_mpi.c      # MPI parallel version
|-- data/                                 # Dataset (numbers 1..1000 generated in code)
|-- results/                              # Output text and terminal screenshots
|-- graphs/                               # Comparison graphs
|-- report/                               # Optional PDF report
`-- presentation/                         # The single PPT
```

## 7. How to Build and Run

All commands are run on the **Master VM** inside `~/parallel_lab/mpi`.

### 7.1 Compile
```bash
mpicc -O2 src/dataset_stats_parallel_mpi.c -o dataset_stats
```

### 7.2 Copy the executable to all Workers
Every Worker runs the same program, so each one needs a copy.
```bash
scp dataset_stats worker1:~/dataset_stats
scp dataset_stats worker2:~/dataset_stats
scp dataset_stats worker3:~/dataset_stats
```

### 7.3 Run on the cluster (4 processes)
```bash
mpirun -np 4 --hostfile hosts sh -c '$HOME/dataset_stats'
```
This starts 4 MPI ranks: rank 0 on the Master and ranks 1, 2, 3 on Worker1, Worker2, Worker3.

### 7.4 Sequential version (single process)
```bash
gcc src/dataset_stats_sequential.c -o dataset_stats_sequential
./dataset_stats_sequential
```

### 7.5 Run MPI on a single machine (for testing)
```bash
mpirun -np 4 ./dataset_stats
```

> If Open MPI refuses to run as root, run as a normal user. Do not disable the safety checks.

## 8. Source Code

### 8.1 Sequential (`src/dataset_stats_sequential.c`)
```c
#include <stdio.h>
int main() {
    int n = 1000;
    long long sum = 0;
    int max = 1, min = 1;
    for (int i = 1; i <= n; i++) {
        sum += i;
        if (i > max) max = i;
        if (i < min) min = i;
    }
    double average = (double)sum / n;
    printf("Sequential Dataset Statistics\n");
    printf("Dataset Size = %d\n", n);
    printf("Sum = %lld\n", sum);
    printf("Average = %.2f\n", average);
    printf("Maximum = %d\n", max);
    printf("Minimum = %d\n", min);
    return 0;
}
```

### 8.2 Parallel MPI (`src/dataset_stats_parallel_mpi.c`)
The MPI program follows the design in Section 4.2 (`MPI_Scatter`, local computation, three `MPI_Reduce` calls, `MPI_Wtime` for timing). See the source file in `src/`.

## 9. Output

### 9.1 Sequential (run on WSL)
```
Sequential Dataset Statistics
Dataset Size = 1000
Sum = 500500
Average = 500.50
Maximum = 1000
Minimum = 1
```
Screenshot: `<img width="1600" height="906" alt="WhatsApp Image 2026-10-07 at 9 37 29 PM" src="https://github.com/user-attachments/assets/78c5baf0-62e0-4d71-840a-7344c67e1db9" />`

### 9.2 MPI – 4 processes on the Master + 3 Worker cluster
```
===== Distributed Dataset Statistics =====
Dataset Size : 1000
MPI Processes: 4
Sum          : 500500
Average      : 500.50
Maximum      : 1000
Minimum      : 1
Execution Time: 0.0003 seconds        (Run 2; Run 1 gave 0.0009 seconds)
==========================================
```
Screenshots: `<img width="1496" height="1051" alt="WhatsApp Image 2026-10-07 at 9 40 19 PM" src="https://github.com/user-attachments/assets/597dd400-a38b-46b2-92d1-017e7a85d195" />`

## 10. Results and Comparison

### 10.1 Correctness
| Quantity | Formula / Expected | Sequential | MPI (4 processes) |
|----------|--------------------|------------|-------------------|
| Sum | 1000 x 1001 / 2 | 500500 | 500500 |
| Average | 500500 / 1000 | 500.50 | 500.50 |
| Maximum | N | 1000 | 1000 |
| Minimum | 1 | 1 | 1 |

The parallel output is **identical** to the sequential output and to the formula, so the MPI program is **correct**.

### 10.2 MPI Execution Time (N = 1000, 4 processes)

| Run | Execution Time |
|-----|----------------|
| Run 1 | 0.0009 s (0.9 ms) |
| Run 2 | 0.0003 s (0.3 ms) |

![MPI execution time](
)

### 10.3 Workload Comparison

| Version | Processes | Numbers handled by each process |
|---------|-----------|---------------------------------|
| Sequential | 1 | 1000 |
| MPI parallel | 4 | 250 |

Each MPI process does **4 times less work** than the sequential process.

![Work per process](<img width="1300" height="800" alt="image" src="https://github.com/user-attachments/assets/79c6a465-2c2a-43f4-b3ec-19517c1b8e6b" />
)

### 10.4 Speedup and Efficiency
Speedup = T(sequential) / T(parallel) and Efficiency = Speedup / 4.
The sequential program in the lab did not print its execution time, and it was run on a different system (Windows WSL) from the MPI cluster (Ubuntu VMs). So a fair speedup value is **not calculated here**. To obtain it, run `src/dataset_stats_sequential_timed.c` on the Master VM and put the time in this table:

| T(sequential) | T(parallel, 4 processes) | Speedup | Efficiency |
|---------------|--------------------------|---------|------------|
| [measure on master VM] | 0.0003 s (best run) | [ ] | [ ] |

## 11. Analysis and Discussion

- The 1000 numbers are split equally, so every process has the **same workload** (250 numbers) and the load is balanced.
- Each process works only on its own memory. Data is moved only by `MPI_Scatter` and `MPI_Reduce`, and only three small values (sum, max, min) are sent back by each process.
- The MPI time changed between runs (0.9 ms and 0.3 ms). This shows that the time at this size is mostly **communication and network overhead** between the VMs, which varies from run to run.
- For N = 1000 the actual calculation takes only a few microseconds, so the communication time is larger than the computation time. Therefore a big speedup is **not expected** at this size. Parallel processing becomes useful for **much larger datasets**, where computing time dominates.
- `MPI_Reduce` is efficient because rank 0 combines only a few partial answers instead of receiving every number.



## 12. Known Messages / Troubleshooting

- **"Authorization required, but no authorization protocol specified"** appears many times in the terminal during the run. It is a display (X11 / GUI) authorization warning from the VM and does **not** affect the MPI computation. The final output is correct.
- The program prints `Executon Time` in the screenshot (spelling mistake in the print statement). Correct it to `Execution Time` in the source before final submission.
- If `mpirun` cannot reach workers, check passwordless SSH and the names in `hosts`.
- If the executable is not found on a worker, copy it again using `scp`.

## 13. Checkpoint Mapping

| Checkpoint | Work | Where in this repo |
|-----------|------|--------------------|
| 1 | Problem definition, sequential algorithm, parallel design | Sections 3 and 4 |
| 2 | Working parallel implementation (MPI) | `src/`, Sections 5, 7, 8 |
| 3 | Run and collect results | Sections 9 and 10, `results/` |
| 4 | Graphs and analysis | `graphs/`, Sections 10 and 11 |
| 5 | Final demonstration and viva | `presentation/` |

## 14. Conclusion

The program computes the sum, average, maximum and minimum of 1000 numbers using 4 MPI processes on one Master and three Worker VMs. The data was divided with `MPI_Scatter` (250 numbers per process) and combined with `MPI_Reduce`. The result (Sum = 500500, Average = 500.50, Max = 1000, Min = 1) matches the sequential program and the formula. Each process does four times less work than the sequential version. At this small size, communication overhead dominates the execution time (0.3 ms to 0.9 ms), so larger datasets are needed to show real speedup.

