# Looking for Group Synchronizations (P2)

## Description
This program is a solution to the "Looking for Group" dungeon queuing problem in an MMORPG. It ensures that parties of players are properly synchronized to enter dungeon instances while preventing deadlocks and starvation. The program manages the queue of players and allocates them to available dungeon instances based on predefined constraints.

## Features
- Supports a maximum of **n** concurrent dungeon instances.
- Forms a party of **1 tank, 1 healer, and 3 DPS** players.
- Ensures deadlock-free and starvation-free execution.
- Randomly selects dungeon completion time between **t1** and **t2** seconds.
- Displays the status of dungeon instances (active/empty).
- Provides a summary of the total instances served and total time served at the end of execution.

## Input Parameters
The program accepts user input for the following parameters:
- **n** - Maximum number of concurrent instances.
- **t** - Number of tank players in the queue.
- **h** - Number of healer players in the queue.
- **d** - Number of DPS players in the queue.
- **t1** - Minimum time before an instance is finished.
- **t2** - Maximum time before an instance is finished (≤ 15 seconds).

## Output
- Real-time status updates of available instances:
  - "active" if a party is currently running in the instance.
  - "empty" if the instance is not in use.
- Final summary displaying:
  - Number of parties served.
  - Total time spent in dungeon instances.

## Compilation and Execution
### Compilation
Ensure you have **GCC** installed. Use the following command to compile the program:
```bash
 g++ -o main main.cpp -pthread
```

### Running the Program
Run the compiled program with:
```bash
 ./main
```
