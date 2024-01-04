# MIT Lecture 10: Measurement and Timing

- machine is changing the clock frequency: laptop is getting hot
- heating up, slows down to save power
- running times for sorting
- compare A and B:
  - run A before B - A might be slower but since it ran first, it heats up the
    processor then clock frequency is reduced - B runs slower
- DVFS: Dynamic Frequency and Volatage Scaling. Works as follows:
  - reduce operating frequency if chip is too hot or otherwise conserve
    (especially battery) power.
  - reduce voltage if frequency is reduced
  - technique to reduce power by adjusting the clock frequency and supply
    voltage to transistors.
  - reduce the voltage if frequency lowers
- how can one reliably measure the performance
- shutting DVFS off:
- outline:
  - quiscing systems: making systems quiet enough
  - tools for measuring software performance
  - performance modeling

## Quiescing Systems

- Genichi Taguchi & Quality Control
- When we're producing a product, let's first reliably produce what we're
  producing so as to get the same every time.
- go after the variants first, try to get the spreads as small as possible,
  because if you try to make changes while you have high variance, you dont even
  know if you're making progress or it's noise
- reliable measurements
- sources of variability:
  - deamons and backround jobs
  - interrupts:
  - code and data alignment: if a code goes cache lines or page boundaries
  - thread placement
  - runtime scheduler: system likes to use core 0
  - hyperthreading - simultaneous multithreading
  - multitenancy
  - dynamic voltage and frequency scaling
  - turbo boost: looks to see how many jobs are running on a multicore - if
    there's only one job running on a multicore it increases the clock frequency
    for that job
  - network traffic

- Quiescing the System:
  - make sure no other jobs are running
  - shut down daemons and cron jobs
  - disconnect the network
  - dont fiddle with the mouse
  - for serial jobs, dont run on core 0, where interrupt handles are usually
    run.
  - turn hyperthreading off
  - turn off DVFS
  - turn off Turbo boost
  - use taskset to pin Cilk workers to cores
- deterministic hard to get on modern hardware: something in hardware that's
  non-deterministic - memory errors - hardware to do error correction - DRAM
  memory is the biggest source of all these - alpha particles
- compiler: modern changes - functions start on cache lines that way if you
  change one line it only changes
- aligned code is more lkely to avoid performance anomalies but it can also
  sometimes be slower

## Tools for measuring software performance

- Ways to measure:
  - measure the program externally: `/usr/bin/time`
    - elapsed time: wall clock time
    - user time:A amount of processor time spent in user-mode code (outside the
      kernel) within the process
    - system time: amount of processor time spent in the kernel within the
      process.
  - instrument the program: `gettimeofday`, `clock_gettime`, `rdtsc`
  - interrupt the program: stop the program and ook at its internal state - gdb,
    poor man's profiler, gprof
  - explore hardware and operating systems support: perf
  - simulate the program: cachegrind

- timing call: `clock_gettime(CLOCK_MONOTONIC)` - 83ns. It guarantees never to
  run backwards.
- don't use rdtsc:
  - rdtsc may give different answers on different cores on the same machine
  - TSC sometimes runs backwards
  - the counter may not progress at a constant speed
  - converting clock cycles to seconds can be tricky
- also don't use gettimeofday - similar problems

- simulators: can deliver accurate and repeatable performance numbers. If you
  want a particular statistic, you can go in and collect it without perturbing
  the simulation.

- triangulation:
  - take at least two measurements in two different ways
  - never trust a number without having a model for what's coming
  - never trust any one number or tool

## Performane Modelling

- basic performance-engineering workflow:
  - take a program A that you want to go fast
  - measure the performance of program A
  - make a chane to program A to produce a hopefully faster program A'
  - measure the performance of program A'
  - if A' beats A, set A = A'
  - else go to step 3
- if you can't measure performance reliably, it's to make many small changes
  that add up.
- drive the variability of measurement to 0
- statistics: what statistics best represent the raw performance of the software
  - minimum does the best at noise rejection, because we expect that any
    measurements higher than the minimum are due to noise
  - median doesn't reject noise. View your program's runtime as (runtime +
    noise)
- selecting among summary statisics:
  - service as many requests as possible: arithmetic mean, CPU utilization
  - all tasks are completed within 10ms: arithmetic mean, wall-clock time
  - most service requests are satisfied within 100ms: 90th percentile, wall
    clock time
  - Meet a customer service-level agreement: some weighted combination, multiple
  - Fit into a machine with 100MB of memory: Maximum, Memory Use
  - Least Cost possible: arithmetic mean, energy use or CPU utilization
  - Fastest/biggest/best solution: arithmetic mean, speedup of wallclock time

- comparing two programs:
  - do head to head comparisons
  - null hypothesis - testing null hypothesis
  - in that environment
- fitting to a model
