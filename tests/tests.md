# Tests to write

## Sender
No unit test really needed, it's a thin wrapper over IPC function calls

## Receiver
Same as sender, for both a full suite of tests with interesting inputs should be devised

## Parser
+ [ ] Test conflicting flags
+ [ ] Test missing arguments

## Queue-writer (sends data)
+ [ ] Creation: 
    + pre-existing
    + no queue
    + Already with messages
+ [ ] Send:
    + Full buffer
    + Partial buffer
    + Empty buffer (should send an higher priority message end)
    + send after empty (should fail)

## Queue-reader
+ [ ] Creation:
    + pre-existing
    + no queue
    + second reader
+ [ ] Ready:
    + no writer_finished:
        + 0 message => true
        + 1 message (writer_finished) => false
        + 1 message (normal) => true
        + 2 messages (mixed) => true
    + writer_finished:
        + 0 => false
        + 1+ => true
    + updated from "receive" (send two messages, check ready, receive, again check ready)
+ [ ] Receive:
    + from buffer (reuse setup from 1 message normal)
    + first message is writer_finished
    + full
    + partial
