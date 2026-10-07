# Distributed Dataset Statistics using MPI

**Course:** Parallel and Grid Computing (PGC) – Lab Evaluation
**Theme:** 6 – Distributed Dataset Statistics
**Parallel Model:** MPI (Message Passing Interface)
**Team:** B1 Team 6

| Name | Roll No. |
|------|----------|
| [Member 1] | [Roll No.] |
| [Member 2] | [Roll No.] |
| [Member 3] | [Roll No.] |
| [Member 4] | [Roll No.] |

---

## 1. Objective

To calculate the **sum, average, maximum and minimum** of a numerical dataset using multiple **MPI processes**, and to study how splitting the work across processes (and machines) affects execution time, speedup and efficiency.

## 2. Introduction (in simple words)

When a dataset is very large, one computer takes a long time to process it. Instead, we can **divide the data into equal parts** and give each part to a different process. Every process works on its own part at the same time. At the end, the partial answers are **combined** into the final answer.

**MPI** is a standard library that lets many processes (running on one or many computers) talk to each other by sending messages. Each process has a number called its **rank** (0, 1, 2, ...). Rank 0 is normally the **master** that distributes data and collects results.

## 3. Problem Definition

**Input:** A dataset of `N` numbers.
**Output:** Sum, Average (= Sum / N), Maximum, Minimum.
**Goal:** Compute these in parallel using `P` MPI processes and compare with the sequential version.

## 4. Algorithm

### 4.1 Sequential Algorithm
1. Read or generate the dataset.
2. Loop through all `N` elements once.
3. Keep a running sum, and update max and min.
4. Average = sum / N.

Time complexity: **O(N)**.

### 4.2 Parallel Design (MPI)
1. **Initialise** MPI (`MPI_Init`) and get the rank and number of processes (`MPI_Comm_rank`, `MPI_Comm_size`).
2. **Check:** the program needs exactly **4 processes**. If not, rank 0 prints a message and all processes exit.
3. The dataset of **16 integers** (10, 20, ..., 160) is stored in the program.
4. **Distribute:** `MPI_Scatter` sends **4 elements to each process** (16 / 4 = 4).
5. **Local computation:** every process finds the sum, maximum and minimum of its own 4 elements.
6. **Combine:** three `MPI_Reduce` calls collect the partial results at rank 0:
   - `MPI_SUM` for the total sum
   - `MPI_MAX` for the global maximum
   - `MPI_MIN` for the global minimum
7. **Final result:** rank 0 calculates average = total sum / 16 and prints all statistics.
8. **Finalise** MPI (`MPI_Finalize`).

```
              Rank 0 holds data[16]
                      |  MPI_Scatter (4 elements each)
     +----------+-----+-----+----------+
     |          |           |          |
  Rank 0     Rank 1      Rank 2     Rank 3
 10..40      50..80      90..120    130..160
 local calc  local calc  local calc local calc
     |          |           |          |
     +----------+-----+-----+----------+
                      |  MPI_Reduce (SUM, MAX, MIN)
                   Rank 0
        Final Sum, Average, Max, Min
```

### 4.3 MPI Functions Used

| Function | Purpose |
|----------|---------|
| `MPI_Init` / `MPI_Finalize` | Start and end the MPI environment |
| `MPI_Comm_rank` | Get the process number (rank) |
| `MPI_Comm_size` | Get the total number of processes |
| `MPI_Scatter` | Divide the dataset equally among all processes |
| `MPI_Reduce` | Combine partial results (sum, max, min) at rank 0 |

### 4.4 Work Done by Each Process

| Rank | Node | Elements Received | Local Sum | Local Max | Local Min |
|------|------|-------------------|-----------|-----------|-----------|
| 0 | master | 10, 20, 30, 40 | 100 | 40 | 10 |
| 1 | worker1 | 50, 60, 70, 80 | 260 | 80 | 50 |
| 2 | worker2 | 90, 100, 110, 120 | 420 | 120 | 90 |
| 3 | worker3 | 130, 140, 150, 160 | 580 | 160 | 130 |
| **Combined** | | | **1360** (SUM) | **160** (MAX) | **10** (MIN) |

Average = 1360 / 16 = **85.00**

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
|-- README.md                          # This report
|-- src/
|   |-- dataset_stats_sequential.c     # Sequential baseline
|   `-- dataset_stats_parallel_mpi.c   # MPI parallel version
|-- data/                              # Dataset (also built into the code)
|-- results/                           # Terminal output / screenshots
|-- graphs/                            # Plots (if timing runs are done)
|-- report/                            # Optional PDF report
`-- presentation/                      # The single PPT (lab evaluation only)
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

### 7.4 Run on a single machine (for testing)
```bash
mpirun -np 4 ./dataset_stats
```

> If Open MPI refuses to run as root, run as a normal user. Do not disable the safety checks.

## 8. Source Code

File: `src/dataset_stats_parallel_mpi.c`

```c
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;

    // Dataset
    int data[16] = {
        10, 20, 30, 40,
        50, 60, 70, 80,
        90, 100, 110, 120,
        130, 140, 150, 160
    };

    int local_data[4];

    int local_sum = 0;
    int local_max;
    int local_min;

    int total_sum;
    int global_max;
    int global_min;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check that exactly 4 processes are used
    if (size != 4)
    {
        if (rank == 0)
        {
            printf("Please run the program using 4 MPI processes.\n");
        }

        MPI_Finalize();
        return 0;
    }

    // Distribute 4 elements to each process
    MPI_Scatter(data, 4, MPI_INT, local_data, 4, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate local statistics
    local_sum = 0;
    local_max = local_data[0];
    local_min = local_data[0];

    for (int i = 0; i < 4; i++)
    {
        local_sum += local_data[i];

        if (local_data[i] > local_max)
            local_max = local_data[i];

        if (local_data[i] < local_min)
            local_min = local_data[i];
    }

    // Combine results from all processes
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    // Display results only from Master
    if (rank == 0)
    {
        double average = (double)total_sum / 16;

        printf("\n===== Distributed Dataset Statistics =====\n");
        printf("Dataset Size : 16\n");
        printf("MPI Processes: %d\n", size);
        printf("Sum          : %d\n", total_sum);
        printf("Average      : %.2f\n", average);
        printf("Maximum      : %d\n", global_max);
        printf("Minimum      : %d\n", global_min);
        printf("==========================================\n");
    }

    MPI_Finalize();

    return 0;
}
```

## 9. Output

Run on the cluster (Master + 3 Workers) with 4 MPI processes:

```
===== Distributed Dataset Statistics =====
Dataset Size : 16
MPI Processes: 4
Sum          : 1360
Average      : 85.00
Maximum      : 160
Minimum      : 10
==========================================
```

*(Screenshot of the terminal output is saved in `results/`.)*

### Verification
The dataset is 10, 20, 30, ..., 160.
- Sum = 10 + 20 + ... + 160 = **1360** (matches)
- Average = 1360 / 16 = **85.00** (matches)
- Maximum = **160** (matches)
- Minimum = **10** (matches)

The MPI output is the same as the manually calculated values, so the program is **correct**.

## 10. Results and Analysis

### 10.1 Performance
The program is designed to prove **correct distributed processing** on a small dataset of 16 elements. It does not measure execution time.

| Dataset Size | Processes | Elements per Process | Result |
|--------------|-----------|----------------------|--------|
| 16 | 4 | 4 | Correct |

[If you run extra experiments with larger data, add a table here: dataset size, processes, time (s), speedup, efficiency.]

**Formulas**
- Speedup = T(sequential) / T(parallel)
- Efficiency = Speedup / number of processes

### 10.2 Discussion
- The 16 elements are split equally, so every process does the **same amount of work** (good load balance).
- Each process works only on its own memory. Data is moved only by `MPI_Scatter` and `MPI_Reduce`.
- Only **three small values** (sum, max, min) per process are sent back, so communication after computation is very small.
- For only 16 numbers, the computation is tiny. The time to start MPI, connect over SSH and send messages is much larger than the calculation itself. So parallel execution gives **no speedup** here.
- For a **very large dataset**, each process would handle many more elements and the parallel version would become faster than the sequential one.
- Using `MPI_Reduce` is better than sending every number to rank 0, because rank 0 only combines a few partial answers.

### 10.3 Limitations
- The dataset size (16) and process count (4) are fixed in the code.
- The program works only when exactly 4 processes are used.
- No timing is measured.

## 11. Known Messages / Troubleshooting

- **"Authorization required, but no authorization protocol specified"** appears many times in the terminal during the run. This is a display (X11 / GUI authorization) warning from the VM environment. It does **not** affect the MPI computation, and the final output is still correct.
- **"Please run the program using 4 MPI processes."** means `-np` was not 4. Run with `-np 4`.
- If `mpirun` cannot reach workers, check passwordless SSH and the names in `hosts`.
- If the executable is not found on a worker, copy it again using `scp` to `~/dataset_stats`.

## 12. Checkpoint Mapping

| Checkpoint | Work | Where in this repo |
|-----------|------|--------------------|
| 1 | Problem definition, sequential algorithm, parallel design | Sections 3 and 4 |
| 2 | Working MPI implementation | `src/dataset_stats_parallel_mpi.c`, Section 8 |
| 3 | Runs and results | Sections 9 and 10, `results/` |
| 4 | Analysis (time, speedup, efficiency) | Section 10, `graphs/` |
| 5 | Final demonstration and viva | `presentation/` |

## 13. Conclusion

The program computes the sum, average, maximum and minimum of a dataset using MPI on a cluster of one Master and three Worker VMs. The data was divided with `MPI_Scatter`, each process calculated its own partial results, and `MPI_Reduce` combined them at rank 0. The output (Sum = 1360, Average = 85.00, Max = 160, Min = 10) matches the manual calculation. The experiment shows how data is shared between processes with separate memory, and why parallel computing is useful mainly for large datasets.

## 14. Future Work
- Use a large dataset (for example, millions of random numbers).
- Measure time with `MPI_Wtime()` and plot speedup and efficiency for different dataset sizes.
- Support any number of processes and uneven sizes with `MPI_Scatterv`.
- Compare with an OpenMP version of the same task.

## 15. References
- MPI Forum: https://www.mpi-forum.org
- Open MPI Documentation: https://www.open-mpi.org/doc/
- Course lab manual and notes
