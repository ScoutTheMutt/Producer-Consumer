# Producer Consumer Problem With Semaphores
## <u>Overview</u>                

This project implements the producer consumer problem using threads and semaphores. The program is written in C and compiled locally using gcc. 

## <u>Usage</u>

### Building the Executable

While in the `Assignment2` folder run;
```bash
$ make
```

An executable file called `ProducerConsumer` will be made. This can be run manually using;
```
$ ./ProducerConsumer
```

This will run the program using it's default values

Additionally you may change the default seed using the parameter below;
- `-s <number>`: used for changing the pseudo random number generation algorithm. 

**Example:**
```bash
$ ./ProducerConsumer -s 25
```

## <u>Testing</u>

A requirement for this project was to have a test script that will run the program using a certain seed. 

This test script is located in the main directory and can be run using;
```
$ ./TestScript
```

This will run the program with the parameters of N = 200 and s = 42. This will produce random numbers where the number generation is replicatable.

### Log Files
all log files are stored under the `./logs` directory.

There is an aditional shell script in the `./logs` folder used to delete the log files. This script can be run using the command below;
```bash
$ ./DeleteLogs
```

## <u>Pre-submission Run</u>
Log files from a pre-submission run are located in `./logs/PreSubmissionTest_Seed53`. This run was done with a seed of `53`. The log files in that directory are run using the command;
```bash
$ ./ProducerConsumerProblem -s 53
```

## <u>File Structure</u>

#### C Files (./files/)
- `ProducerConsumer.c`: Contains main function and producer and consumer logic.
- `LinkedList.c`: Contains the functions for the linked list. 

#### Header Files (./headers/)
- `LinkedList.h`: Header file used to link both C files together.

#### Log Files (./logs/)
*Note that these files will only appear after the program has been run.*
- `consumer_even_output.txt`: Contains the output for the consumerEven thread.
- `consumer_odd_output.txt`: Contains output for the consumerOdd thread.
- `producer_even_output.txt`: Contains the output for the producerEven thread.
- `producer_odd_output.txt`: Contains output for the producerOdd thread. 

#### Documents (./documents/)
- `Assignment 2.pdf`: Contains the grading instructions for the program.

## <u>Troubleshooting</u>
- Verify that the file structure is identical to the structure above. 
- This implementation will only work on unix based systems with gcc.

## <u>Build Design </u>
This program needed to be designed in a way that would allow the producers and consumers to run along side each other. In order to do this I needed to syncronize the threads together. This was done by using a semaphore lock. 
This implementation allows syncromization by running a thread on each function. Then securing the data inside those functions with mutexs and semaphores.

Deadlock is avoided in my program by the use of two semaphores. `semEmpty` and `semFull`. Initialization of these below. 
```C
    sem_init(&semEmpty, 0, MAX_BUFFER_SIZE);
    sem_init(&semFull, 0, 0);
```
both of these semaphores are initialized with the starting value of `0`. The max value of `semEmpty` is set to 50 which is our max buffer size. The max size of `semFull` is set to `0`

Implementation of these semaphores in both the producer consumer functions is outlined below. 
```C
// Producer
sem_wait(&semEmpty);    // Wait for empty slot
// ... add item to buffer ...
sem_post(&semFull);     // Signal new item available

// Consumer  
sem_wait(&semFull);     // Wait for item
// ... remove item from buffer ...
sem_post(&semEmpty);    // Signal empty slot available
```
The reason they are implemented this way is to stop the program from producing more than 50 items while also not allowing the consumers to endlessly waste CPU cycles looking for a value.

This method would prohibit deadlock becuase the critical section in both will always run and then return the semaphore back to either the producer or consumer. The producers will always check to see if it has produced enough to stop before taking the semaphore. 

For terminating the program. In the producer section once they have produced 200 items total and 100 each, they will each increment a value. The consumers will check the that value and if it `= 2`; and the head of the list is null, the thread will exit. 