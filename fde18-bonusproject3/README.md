## Bonus Project 3
### Task
In this project, we revisit the calculation of the Bacon-Number.  Only this time, we
do not only care for the distance of Kevin Bacon to everyone else,  but for the shortest
chain length between any two given actors.
Write a program that uses a given file playedin of csv entries with these columns:
1.  actor id (integer)
2.  movie id (integer)
The integer values form a dense range starting from 0 to the number of elements enumerated.
Based  on  this  graph  and  two  actor  ids,  your  program  must  compute  the  length  of  the
shortest coworking chain between two actors.  Chains are built of actors that acted in the
same movie.  For example if Kevin Bacon and Dustin Hoffman worked on the same movie,
the chain length between them is 1.  If Dustin Hoffman then worked with Barbra Streisand,
but Barbra Streisand did not work with Kevin Bacon, the shortest chain length is 2.  This
extends to arbitrary chain lengths.  If no chain can be found, your program should return -1.  
The chain length from an actor to himself is defined to be 0.
### Runtime requirement
20 seconds

### Solution 
**Runtime(ms):** 15373   
1. `perf record` to indentify which parts of the program to optimize.
2. Bidirectional BFS implemented. It usually discovers much fewer nodes than a one directional BF.
3. `mmap` to reduce copying of data.
4. Multiple threads to run queries. (`std::async`)

Besides that:
- Instead of using `unordered_map<Actor, unordered_set<Movie>>` a `vector<vector<Movie>>` is used. That allows for much faster access.
- Instead of using `std::istringstream` a `std::iftream and the extract operator >>` was used.
- `vector<bool>` was used to store visited nodes.
