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
1.  Make sure the implemented algorithm is reasonably efficient. In this case, BFS or how about bidirectional BFS? (The latter usually discovers much fewer nodes than a one directional BFS)
2.  Check how much time is spent on which part of your program. Use the provided playedin.csv and run 50 (random) queries on it. Use perf record or measure time spent from within your program. This will show you which parts of the program to optimize next.
3.  Possible issues: Too many memory allocations. Not all resources used (12 Threads are available on the server)

Besides that:
- Do the parsing of playedin.csv in a single thread.
- Use multiple threads to run queries. (std::async is your friend)
- Instead of using unordered_map<Actor, unordered_set<Movie>> use a vector<vector<Movie>>. That allows for much faster access.
- Do not use std::istringstream. This one is very slow. Instead, you can use std::iftream and the extract operator >>
