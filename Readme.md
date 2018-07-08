#### Alternative implementation:
In addition to the normal readers/writers using condition variables and mutexes, I have implemented the `LightSwitch` solution as described in `The little book of Semaphores`. 
__All credit of the idea can be attributed to the author of the book, howevever I have added C specific implementation details__.

##### Usage:
To avoid changing `myserver.c` the only way this can be easily ran is if we simply overwrite the entire `rw.c` file with my `rwplus.c` file. It should just work to `ctrl-a`, `ctrl-c` inside `rwplus.c`, followed by `ctrl-v` into `rw.c`

##### Idea/Description:

__Reader__: The idea for the readers is that we want to queue up a list of readers that are waiting for the first reader to get access to the data. This means that any readers that want to access the data must wait for the last writer to finish writing before they can start reading.

__Writer__: The idea for writers is that we want to queue up a list of writers that are waiting for the first writer to get access to the data. In addition, any writers must wait untill there is no more writers before it can attempt to write to the data. The solution uses a writers priority by queuing up the writers and signaling to them before any readers get signaled.
