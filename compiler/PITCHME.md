### Accelerator Design with OpenCL
##### (Athens Week 19-24 March, 2018) 
---
### Parallelizing codes: Techniques

---
### Loops: Sequential
<img src="http://perso.telecom-paristech.fr/~chaudhur/images/tpt39/fig/para_seq.svg" height="400"/>
---
### Loops: Intra Task Parallelism
<img src="http://perso.telecom-paristech.fr/~chaudhur/images/tpt39/fig/para_intra.svg" height="400"/>
---
### Loops: Inter Task Parallelism
<img src="http://perso.telecom-paristech.fr/~chaudhur/images/tpt39/fig/para_inter.svg" height="400"/>
---
### Loops: Intra & Inter Task Parallelism
<img src="http://perso.telecom-paristech.fr/~chaudhur/images/tpt39/fig/para_inter_intra.svg" height="400"/>
---
### Loops: Inter Iteration Parallelism
<img src="http://perso.telecom-paristech.fr/~chaudhur/images/tpt39/fig/para_inter_iter.svg" height="400"/>
---
### Loops: Inter Instruction Parallelism
- Taken care of by Compilers
---

### Parallelizing Codes: Libraries
- Distributed Shared Memory (NUMA) & Clusters
	- MPI
- Shared memory multiprocessors
	- OpenMP
- Heterogeneous Multi-Processors
	- OpenCL
- Can be used in combination
---
### Synchronization: OpenCL events
- CL_EVENT
	- 	CL_QUEUED
	-	CL_SUBMITED
	-	CL_RUNNING
	-	CL_COMPLETE
	- 	ERROR_CODE

- Useful Functions:
	- clWaitForEvents
---
### Synchronization: OpenCL Command Barriers
-	Command Barrier
	-	cl_int clEnqueueBarrier (	cl_command_queue command_queue)
	-	all queued commands in command_queue before barrier must finish
		before starting the commands after barrier.

	
