# Message QUEUE ipc
[Back to general](architecture.md)

A posix queue is an unidirectional IPC channel which allows the transfer of packets of data with an
assigned priority such that:
+ packets with higher priority are always read before packets of lower priority
+ packets with the same priority respect the FIFO property

In this project packets of priority 0 are used to transfer the file data while a single packet of
priority 1 is used to signal that the writer has finished sending packets and no more packets will be 
added to the queue

The reader will therefore keep reading until both of the following conditions apply:
+ the "EOT" packet has been received
+ the queue is empty

# Sequence diagrams
+ [Sender diagram](resources/queue_seq_send.png)
+ [Receiver diagram](resources/queue_seq_recv.png)