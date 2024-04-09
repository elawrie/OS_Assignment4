# OS_Assignment4

Alejandro Rea
237852
area@chapman.edu

Evelyn Lawrie
2364909
lawrie@chapman.edu
 
CPSC380 Section 01

# Alejandro and Evelyn worked together

# This is an implementation of the bounded buffer problem using semaphores

# Files:
buffer.c
prodcon.c
Checksum.c
sampleOutput.txt
README.md

# Running instructions:
gcc -o prodcon prodcon.c buffer.c -Wall
./prodcon 5 10 10 

this will run the program with 5 seconds of sleep time before terminating, 10 producer threads, and 10 consumer threads
```