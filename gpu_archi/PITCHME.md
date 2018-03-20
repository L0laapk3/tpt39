### Accelerator Design with OpenCL
##### (Athens Week 19-24 March, 2018) 
---
### What do we know so far ?
- There are three types of parallelism |
	- Task Parallelism |
	- Data Parallelism |
	- Pipeline |
- We saw the reasons for memory stalls and latency. |

- The techniques to hide latency through Caching. |

- The Virtual Memory. |
---
### What do we know so far ?
- We saw the evolution of processors from |
	- Uniprocessor to ... |
	- Multicores with Simultaneous Multi-Threading. |

- And we said hello to the world from our GPU (Mali-T628). |
---
### GPU Architecture : Uniprocessor
+++?image=assets/fig/uniprocessor.svg&size=auto 90%
---
### GPU Architecture : Evolution

- GPUs took a completely different path of evolution. |
- Because they live in a embarrasingly data-parallel environment. |
- The memory stalls/latency problems are still there. |
- So are the solutions to hide them. |
---
### GPU Architecture : MIMD
+++?image=assets/fig/gpu_mimd.svg&size=auto 90%
--- 
GPU Architecture : Evolution
* MIMD, but wait, we don't need the mutliple-instruction streams. |
* let' get rid of them. |
---
### GPU Architecture : SIMD
+++?image=assets/fig/gpu_simd1.svg&size=auto 90%
---
### GPU Architecture : More SIMD
+++?image=assets/fig/gpu_simd2.svg&size=auto 90%
---
### GPU Architecture : More SIMD
- Let's not forget our old friend Multi-Threading.
- Which helped us manage latency.
---
### GPU Architecture : SIMD with multi-threading.
+++?image=assets/fig/gpu_simd3.svg&size=auto 90% 
---
### Quiz 
* What is the peak performance of this core in Gflops ?
---
### GPU Architecture : Refinements
+++?image=assets/fig/gpu_refined.svg&size=auto 90% 
---
### GPU Architecture : Refinements
- The context memory is configurable for different number of threads.
- Addinc Cache is helpful.
+++?image=assets/fig/gpu_refined_cache.svg&size=auto 90% 
---
### GPU Architecture : Refinements
- Adding Scratchpad memory, so that threads can communicate locally.
+++?image=assets/fig/gpu_refined_cache_sp.svg&size=auto 90% 
---
### GPU: Multiple Shader Cores
+++?image=assets/fig/gpu_refined_multi.svg&size=auto 90% 
---
### Our GPU : Mali T628
- ARM MidGard family.
- Can be configures for 4-16 cores.
- configurable SIMD
	- 2x FP64, 4x FP32, 8x FP16, 2x int64, 4x int32, 8x int16, 16x int8
- Two L1 Caches/ Shader core 16KB
- L2 Cache can be configured for upto 64KB.
- Each core Rated at 17 Flops/cycle. (FP32)
- 64 byte Cache lines
- source <a https://community.arm.com/graphics/b/blog/posts/the-mali-gpu-an-abstract-machine-part-3---the-midgard-shader-core a/>
- source <a https://community.arm.com/graphics/f/discussions/6557/mali-t628-gpu-activity-in-streamline a/>
### Example Heterogeneous SoCs
+++?image=assets/acecontext.svg&size=auto 90%
---
---
### Domain Specific Architecture
