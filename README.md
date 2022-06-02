# OS-Scheduler

<h3>Build With : </h3>
 <ul>
  <li><a href="https://gcc.gnu.org/">GCC</a></li>
 </ul>

   
   
<h3>Getting Started</h3>
<blockquote>
  <p>This project is made on a unix-like operating system and this is a list of needed instructions to set up your project locally, to get a local copy up and running follow these instructions.
 </p>
</blockquote>
<h3 href="#Installation">Installation</h3>
<ol>
  <li><strong><em>Clone the repository</em></strong>
    <blockquote>$ git clone https://github.com/MoazHassan2022/OS-Scheduler.git</blockquote>
  </li>
  <li> 
  <strong><em>Navigate to repository directory
</em></strong>
    <blockquote>$ cd OS-Scheduler</blockquote>
  </li>
  <li> 
  <strong><em>Install dependencies
</em></strong>
    <blockquote>(Ubuntu)$ sudo apt install build-essential</blockquote>
  </li>
</ol>
<h3 href="#Running">Running</h3>
<ol>
  <li><strong><em>Compile the OS Scheduler</em></strong>
       <blockquote>$ make</blockquote>
  </li>
  <li><strong><em>Run the OS Scheduler</em></strong>
       <blockquote>$ make run</blockquote>
  </li>
    <li><strong><em>Generate new processes information to processes.txt (Our input file)</em></strong>
    <blockquote>$ gcc test_generator.c -o test_generator.out</blockquote>
    <blockquote>$ ./test_generator.out</blockquote>
  </li>
 
</ol>

<h2 href="#Structure">Project Structure</h2>
 <div> 
  <pre>
OS-Scheduler
├── algorithms.h
├── bud.h
├── commons.h
├── headers.h
├── priority_queue.h
├── queue.h
├── vector.h
├── keyFile
├── MakeFile
├── memory.log
├── scheduler.log
├── scheduler.pref
├── clk.c
├── prgenerator.c
├── process.c
├── process_generator.c
├── scheduler.c
├── test_generator.c
├── processes.txt
  </pre>
</div>

<h3>Description</h3>
<blockquote>
  <p>
  This project was implemented for Operating Systems course in Cairo University, Faculty of Engineering, Commputer Engineering Department.
  <br>
  It is a a processes scheduler with limited memory (1024 Byte) that takes inputs from processes.txt file that has these columns for every process: 
  <ol>
  <li>
  <h4>id.</h4>
 </li>
 <li>
  <h4>arrival.</h4>
 </li>
 <li>
  <h4>runtime(Burst time).</h4>
 </li>
  <li>
  <h4>priority.</h4>
 </li>
 <li>
  <h4>memorysize.</h4>
 </li>
 </ol>
 Note that every process can have less than or equals 256 byte memory size.
 <br>
 Then the scheduler simulates running these processes with one of three algorithms that the user can choose:
 <ol>
  <li>
  <h4>Heighst Priority First(Non-Preemptive and heighst priority process with least priority number).</h4>
 </li>
 <li>
  <h4>Shortest Remaining Time Next.</h4>
 </li>
 <li>
  <h4>Round Robin(User chooses the quantum).</h4>
 </li>
 </ol>
 Then outputs details about scheduling scenarios to scheduler.log file that has this details for every process:
 <br>
 # At time x process y state arr w total z remain j wait k
 <br>
 And details about CPU utilization, Avg Weighted Turnaround Time, Avg Waiting Time and standered deviation of Weighted Turnaround Time to scheduler.pref.
<br>
 And finally details about memory allocating and deallocating to memory.log that has this details for every process:
 <br>
 # At time x allocated(Or deallocated) y bytes for Processs z from i to j
 </p>
</blockquote>