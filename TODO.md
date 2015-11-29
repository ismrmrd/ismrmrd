To Do List for 2.x
==================

* Should the stream number be and argument for append, since it is part of the header?
* Is it OK to remove measurement UID.
* Should the interface functions for times (time stamps and dwell times) include units (ns) to be explicit?
* Tests, should we test entity_type, storage_type, stream members? How?
* Should "contrast" be renames to "echo"? Should "echo" be added as an additional loop counter variable?
* Is the kspace_encode_step_0 handled correctly, or should it be kspace_encode_step_3 (we could add 4 too??)