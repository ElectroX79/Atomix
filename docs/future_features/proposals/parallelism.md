# Parallelism

>  This is a proposed execution model. The design is highly experimental and needs to be
> studied, discussed and refined.
## Goal

The proposed concurrency model is task-oriented. Threads are execution
resources, while tasks define the work to be performed.

```text
                       Guardian
                          │
                       Objective
                          │
                          ▼
                        Master
                          │
                 ┌────────┼────────┐
                 ▼        ▼        ▼
               Task A   Task B   Task C
                 │        │        │
                 ▼        ▼        ▼
              Thread    Thread    Thread
```


The **Guardian** distributes high-level objectives. A **Master** is not a
special kind of thread. It is the thread currently responsible for guaranteeing
the lifetime of an objective and distributing its work into tasks.

The Master should also execute work itself whenever possible. It is
both a coordinator and a worker, and should return to ordinary task execution
after distributing its work.

Tasks operate on existing data through non-owning views such as `span`. The
existing chunked column layout provides a natural way to divide data between
tasks. 

**Note: Avoiding conflicting accesses is the responsibility of the task
designer.**

## Execution and completion

A Master normally follows a simple prologue, task distribution, and epilogue.
It does not block while waiting for workers. Task groups use atomic counters to
detect completion, and the final notification is sent to the Master's private
VIP queue.

Each thread tries to complete its VIP queue after completing a task before returning to
the global task queue.

```text
Task finished
     │
     ▼
 Clean VIP queue
     │
     ▼
 Global task queue
     │
     ▼
 execute task
```

## Thread-local memory

Each execution thread is assigned two private linear arenas:

* **Instruction arena** for temporary instruction-related state.
* **Data arena** for temporary data and intermediate results.

## Scheduling

Threads consume tasks from a global queue. When no work is available, a thread
spins briefly before sleeping. Additional threads may be awakened when the
workload increases or when active threads are occupied coordinating other
tasks.

The maximum number of execution threads is initially user-configurable.

## Failure model

The initial design uses a simple failure model: a failed task terminates the
whole execution.


