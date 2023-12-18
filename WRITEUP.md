## ECEN 4313 - Final Project ##
### Frank McDermott ###

#### Project ####
For my final project I chose to implement a concurrent ordered map that operates as a Red-Black Tree under the hood. 

#### Locking ####
To facilitate the fine-grain locking requirement, each Node in the tree has it's own mutex lock. I employ hand-over-hand locking on sets of subtrees while traversing during inserts and deletions. This is done by acquiring the root subtree node and it's two children in order to ensure that concurrent rotations and deletions can't occure while we're deleting.

#### API ###
The API for my Map class supportes put(), remove(), and at() functons. In order to maintain the ordering of the tree, as nodes are inserted, deleted, or rotated into place, keys are updated to ensure that the API has the ability to index into the Map and retreive the associated values. I made the design choice to abstract the keys away for the API, so that a user of the API can think of the Map as a flat, 2D, indexible, ordered structure. That said, the Tree very much keeps key-value pairs together. Nodes are also abstracted away such that the user can add a new value to the map simply by calling the put() member function and passing an integer in as an argument. Conversely, to remove a value from the tree, the user simply calls the remove() function with the integer value to be removed.

The API also has a getRange() function, which given a starting and ending point (both inclusive), the Map will return all key/value pairs within the range of those two values.

In addition to the at() function, I've also overloaded the operator[] to provide a cleaner, more array-like, indexing API.

#### Bugs ####
There are still a few out-standning bugs
1) Sequential put()'s and/or remove()'s work as expected, however concurrent remove()'s sometimes result in memory leaks. It seems that there is some node duplication occuring at some point during the rebalancing (deleteFix) phase of the remove call stack. This only arises when performing concurrent deletes, which suggests that there is contention/data races occuring when updating pointers. I spent days debugging this and can't figure out how to fix it.
2) Sometimes the counter gets out of sync which causes indexing error when trying to print out the ordered map. I've added detection logic for this to ensure segfaults don't occur. This is a downstream symptom of the first bug. To combat this I made the counter atomic and added a fetch-and-increment function to atomically update the counter as nodes are added and deleted
3) Another symptom of the first bug is that the destructor causes memory leaks as duplicate nodes are recusrively deleted. I haven't found the source of this issue or how to fix it.

There is about a 30% chance that memory leaks occur due to the destructor trying to delete previously freed memory locations. This has been a nightmare to debug and I can't figure out how to fix it.

As a result of these bugs, experimenting with high contention seemed like a fools errand.