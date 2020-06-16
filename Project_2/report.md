# Project 2
**Group Members:**
- William Croteau, crote010@umn.edu
- Renda Zhang, zhan4067@umn.edu

# Lab Report:
The purpose of the experiments is to compare different page replacement policy
under different benchmark programs. The policies are rand, FIFO, and custom. Rand
policy randomly choose a page to evict for page table; FIFO policy used first
come first out style to evict page; custom policy implements an algorithm that evicts
least evicted page first algorithm. The benchmark programs are scan, sort and focus.
I ran the experiments on a CSE lab machine located in the Keller Hall 4250,
University of Minnesota. The command line arguments are:
```
$ ./virtmem [npages] [nframes] [rand|fifo|custom] [scan|sort|focus]
```
## Custom Page Replacement - LEF

My custom page replacement implements an algorthm I will call Least Evicted First (LEF).
LEF chooses the frame to evict based on which page has been evicted the least. This
attempts to approximate a 'fair' environment for pages. This prevents any page from being
moved back and forth repeatedly from disk. This can prevent two page from contiually swapping
places and causing runaway page faults. 

This is implemented by adding an array called `page_evicts` to keep track of how many faults 
each page has had. This is initialized with all zeros. Each time that a frame is evicted,
the page associated with that frame has its `page_evicts` value incremented by 1.
When determining which frame to evict, the algorithm looks at each frame to find the
minimum value of evictions for the page held there. If there is a tie, the page with the
lowest frame number is evicted.

## Experiment
The three algorithms were tested for three different programs to simulate page access.
The `scan` program accesses data sequentially by first writing once and then reading 10 times.
The `sort` program uses the `qsort` function to sort randomly assigned data.
The `focus` program randomly assigns data to different places in memory.
Each program was tested using 100 unique pages with a varying number of available physical frames.
The number of simulated page faults, disk reads, and disk writes were observed and plotted in the
graphs below.

## Results

### Page Faults

<p align="center"><img src="https://willcro.com/5103/scan-faults.png" alt="Drawing" width="60%"/><br>
<i>Figure 1. Graph of page faults in the "scan" program</i>
</p>

<p align="center"><img src="https://willcro.com/5103/sort-faults.png" alt="Drawing" width="60%"/><br>
<i>Figure 2. Graph of page faults in the "sort" program</i>
</p>

<p align="center"><img src="https://willcro.com/5103/focus-faults.png" alt="Drawing" width="60%"/><br>
<i>Figure 3. Graph of page faults in the "focus" program</i>
</p>

<p align="center"><img src="https://willcro.com/5103/average-faults.png" alt="Drawing" width="60%"/><br>
<i>Figure 4. Graph of average page faults over the three programs</i>
</p>

### Reads

<p align="center"><img src="https://willcro.com/5103/scan-reads.png" alt="Drawing" width="60%"/><br>
<i>Figure 5. Graph of disk reads in the "scan" program</i>
</p>
<p align="center"><img src="https://willcro.com/5103/sort-reads.png" alt="Drawing" width="60%"/><br>
<i>Figure 6. Graph of disk reads in the "sort" program</i>
</p>
<p align="center"><img src="https://willcro.com/5103/focus-reads.png" alt="Drawing" width="60%"/><br>
<i>Figure 7. Graph of disk reads in the "focus" program</i>
</p>
<p align="center"><img src="https://willcro.com/5103/average-reads.png" alt="Drawing" width="60%"/><br>
<i>Figure 8. Graph of average disk reads over the three programs</i>
</p>

### Writes

<p align="center"><img src="https://willcro.com/5103/scan-writes.png" alt="Drawing" width="60%"/><br>
<i>Figure 9. Graph of disk writes in the "scan" program</i>
</p>
<p align="center"><img src="https://willcro.com/5103/sort-writes.png" alt="Drawing" width="60%"/><br>
<i>Figure 10. Graph of disk writes in the "sort" program</i>
</p>
<p align="center"><img src="https://willcro.com/5103/focus-writes.png" alt="Drawing" width="60%"/><br>
<i>Figure 11. Graph of disk writes in the "focus" program</i>
</p>
<p align="center"><img src="https://willcro.com/5103/average-writes.png" alt="Drawing" width="60%"/><br>
<i>Figure 12. Graph of average disk writes over the three programs</i>
</p>

## Analysis
### Scan
For the `scan` program, we observe first that the worst algorithm is by a wide margin FIFO.
Since the data is accessed sequentially, the program will page fault on every single access if
there are fewer frames than pages. The data is read in once, then set to allow writes, then
accessed ten times. This results in 12 faults per page, with 1200 faults overall. LEF did quite
well, allowing the fewest faults when less than 65 frames were available. This is likely due to
the fact that LEF allows the most recently used page to be evicted first, which could be bad in
general, but good for sequential access. Rand roughly serves as a control for these tests, and
it was able to do the best for 65 or more frames. Disk reads are roughly the same as page faults for 
all programs and algorithms and the reasoning is the same.

When observing disk writes for `scan` we see that both FIFO and LEF write each page once, no matter
what. This is because each algorithm attempts to be "fair", not allowing one page to be evicted many
times more than another. Therefore, each one will evict all written-to pages exactly once. Random has
the advantage here because it allows for some pages to sit in memory the whole time while others get
repeatedly evicted. The ones that never get evicted never get written to disk, explaining the dip at the
end of figure 9.

### Sort
Complex analysis of the `sort` program is more difficult, since the program calls a library function.
We can guess that this function likely uses some sort of recursive algorithm like merge-sort
due to the shape of the page faults in FIFO. FIFO has plateaus of constant page faults that abruptly drop
around 50, 25, 12, and 6 frames. Since merge-sort would split the data in half continuously, there is likely
a lot of iterating over chunks that are roughly even splits of the data like 50 or 25. If the chunk is smaller
than the number of frames, there is no page fault. We can also see this pattern somewhat in LEF. LEF
is quite bad for low numbers of frames for the same reason it was good for `scan`. Evicting a page that
was just brought in can be bad when the sort algorithm is doing comparisons. This effect matters less towards
the end. If you have the right number of frames, FIFO is the best choice, but rand is good if little is known
about the size of the set.

Observing disk reads and writes shows similar patterns as the page faults. This is likely for the same reason.
Again, FIFO can be good if you have the right number of frames, and LEF can be good if there is a 
sufficiently large number of frames. If not much is known, rand is the safest choice.

### Focus
`focus` is mostly just random assignment of data, so attempts to be smart about choosing an replacement
algorithm can be tricky. All three algorithms page faults decrease at a roughly constant pace. Random
chance can cause some voltality at low and high numbers of frames for FIFO and LEF, but the patterns would
likely be much different if this was run with a different seed. These same patterns are evident in
the disk reads and writes.

### Conclusion
While testing page replacement algorithms FIFO, LEF, and rand, we can observe that the best choice is
largely dependent on the situation. Without knowing what sort of page ordering we can expect to
see, it can be difficult to declare one of these three the winner. All three had situations where
they had the lowest page faults, writes, or reads, but none had a program where it was always the best.
In general, the choice that had the lowest variability for these three statistics was random selection.
In all the graphs, it can be seen that when FIFO or LEF beat it, it wasn't by much. There were, however, scenarios
where the "smart" algorithms did significantly worse than rand, such as LEF in the sort program.
If nothing is known about the page ordering, the choice with the lowest risk is random selection.

An obvious limitation of this experiment was the inability for the alogorithms to be notified of
page accesses that don't cause a fault. Adding this would allow algorithms like LRU or clock to be used.
They are able to approximate the minimum algorithm much better, and would likely be a better choice.
In future research, the API would expanded to allow for checking the page usage to develop smarter algorithms.
