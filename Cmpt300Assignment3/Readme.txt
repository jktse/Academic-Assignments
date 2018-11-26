All commands follows with the assignments descriptions

The following commands were not imlpemented, N, P, V

NOTE that this program has very limited error checking and will cause crashes if the input were not inputed as requested

The rest are implemented with the following design choices,

When Creating the user need to follow the prompts and the pcb will be created.
When forking a pcb the old process id is not reused, rather a new on will be assigned to the fored process
Kill and exit are essentially the same thing except on requires an input to specify which proces to kill

When sending, follow the prompt and process will be blocked untill it is killed or gets a repy from the process it wanted
When receiving, will block until someone sends them a message,
reply will unblock a process who are awaitng reply from that specific process id

The display messages are will do as the assignment description.
