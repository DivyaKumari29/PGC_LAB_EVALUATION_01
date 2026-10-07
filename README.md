# Distributed Dataset Statistics using MPI

**Course:** Parallel and Grid Computing (PGC) – Lab Evaluation
**Theme:** 6 – Distributed Dataset Statistics
**Parallel Model:** MPI (Message Passing Interface)
**Team:** B1 Team 6

| Name | Roll No. |
|------|----------|
| Divya Kumari | 222 |
| Chaitanya M | 228 |
| Shridevi | 230 |
| Vineet K | 221 |

---

## 1. Objective

To calculate the **sum, average, maximum and minimum** of a numerical dataset using multiple **MPI processes**, and to study how splitting the work across processes (and machines) affects execution time, speedup and efficiency.

## 2. Introduction 

When a dataset is very large, one computer takes a long time to process it. Instead, we can **divide the data into equal parts** and give each part to a different process. Every process works on its own part at the same time. At the end, the partial answers are **combined** into the final answer.

**MPI** is a standard library that lets many processes (running on one or many computers) talk to each other by sending messages. Each process has a number called its **rank** (0, 1, 2, ...). Rank 0 is normally the **master** that distributes data and collects results.

## 3. Problem Definition

**Input:** A dataset of `N` numbers.
**Output:** Sum, Average (= Sum / N), Maximum, Minimum.
**Goal:** Compute these in parallel using `P` MPI processes 

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
 1..250      251..500      501..750    751..1000
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

| Rank         | Node    | Elements Received | Local Sum         | Local Max      | Local Min   |
| ------------ | ------- | ----------------- | ----------------- | -------------- | ----------- |
| 0            | master  | 1–250             | 31,375            | 250            | 1           |
| 1            | worker1 | 251–500           | 93,875            | 500            | 251         |
| 2            | worker2 | 501–750           | 156,375           | 750            | 501         |
| 3            | worker3 | 751–1000          | 218,875           | 1000           | 751         |
| **Combined** |         | **1000 elements** | **500,500 (SUM)** | **1000 (MAX)** | **1 (MIN)** |


Average = 500,500 / 1000 = 500.50
31,375 + 93,875 + 156,375 + 218,875 = 500,500

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
/* REFERENCE VERSION - replace this file with your final submitted code.
   MPI parallel Sum, Average, Max, Min of N numbers (1..N). */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1000

int main(int argc, char *argv[])
{
    int rank, size;
    int *data = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0)
    {
        if (rank == 0)
            printf("Dataset size must be divisible by the number of processes.\n");
        MPI_Finalize();
        return 0;
    }

    int chunk = N / size;
    int *local_data = (int *)malloc(chunk * sizeof(int));

    if (rank == 0)
    {
        data = (int *)malloc(N * sizeof(int));
        for (int i = 0; i < N; i++)
            data[i] = i + 1;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    MPI_Scatter(data, chunk, MPI_INT, local_data, chunk, MPI_INT, 0, MPI_COMM_WORLD);

    long long local_sum = 0;
    int local_max = local_data[0], local_min = local_data[0];
    for (int i = 0; i < chunk; i++)
    {
        local_sum += local_data[i];
        if (local_data[i] > local_max) local_max = local_data[i];
        if (local_data[i] < local_min) local_min = local_data[i];
    }

    long long total_sum;
    int global_max, global_min;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (rank == 0)
    {
        printf("\n===== Distributed Dataset Statistics =====\n");
        printf("Dataset Size : %d\n", N);
        printf("MPI Processes: %d\n", size);
        printf("Sum          : %lld\n", total_sum);
        printf("Average      : %.2f\n", (double)total_sum / N);
        printf("Maximum      : %d\n", global_max);
        printf("Minimum      : %d\n", global_min);
        printf("Execution Time: %.4f seconds\n", end - start);
        printf("==========================================\n");
        free(data);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
```

## 9. Output

Run on the cluster (Master + 3 Workers) with 4 MPI processes:

<img width="1496" height="1051" alt="image" src="https://github.com/user-attachments/assets/ea548429-a3c5-44e9-b609-87ed1094109e" />


### Verification
The dataset contains 1000 values ranging from 1 to 1000.
Sum = 1 + 2 + ... + 1000 = 500500 (matches)
Average = 500500 / 1000 = 500.50 (matches)
Maximum = 1000 (matches)
Minimum = 1 (matches)

The MPI output is the same as the manually calculated values, so the program is **correct**.

## 10. Results and Analysis

### 10.1 Performance
The program is designed to prove **correct distributed processing** on a small dataset of 16 elements. It does not measure execution time.

| Dataset Size | Processes | Elements per Process | Result |
|--------------|-----------|----------------------|--------|
| 1000 | 4 | 250 | Correct |




### 10.2 Discussion
- The 1000 elements are split equally, so every process does the **same amount of work** (good load balance).
- Each process works only on its own memory. Data is moved only by `MPI_Scatter` and `MPI_Reduce`.
- Only **three small values** (sum, max, min) per process are sent back, so communication after computation is very small.
- For a **very large dataset**, each process would handle many more elements and the parallel version would become faster than the sequential one.
- Using `MPI_Reduce` is better than sending every number to rank 0, because rank 0 only combines a few partial answers.


## 11. Conclusion

The program computes the sum, average, maximum and minimum of a dataset using MPI on a cluster of one Master and three Worker VMs. The data was divided with `MPI_Scatter`, each process calculated its own partial results, and `MPI_Reduce` combined them at rank 0. The output (Sum = 1360, Average = 85.00, Max = 160, Min = 10) matches the manual calculation. The experiment shows how data is shared between processes with separate memory, and why parallel computing is useful mainly for large datasets.



