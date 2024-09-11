# Nvidia-GPU-Spoof
Spoofing the NVIDIA GPU UUID by modifying "nvlddmkm.sys"

How this function works:
- Scans for the "rcdbDumpSystemInfo_IMPL" function using code-style patterns
- Iterates through the GPU devices until it reaches the hardcoded gpuUuid.isInitialized boolean in the GPU structure
- writes to the next member of the struct which is always the UUID.

How to update / find the initialized offset:
- While the exact flavor nvidia uses for the Windows gpu driver isn't publicly available, it is nearly identical to the open source linux version
- Load the nvlddmkm.sys file into IDA or a similar program
- Use the sigmaker plugin to find the function using the provided signature and mask ["\xE8\x00\x00\x00\x00\x48\x8B\xD8\x48\x85\xC0\x0F\x84\x00\x00\x00\x00\x44\x8B\x80\x00\x00\x00\x00\x48\x8D\x15", "x????xxxxxxxx????xxx????xxx"]

- navigate to the located function
  ![image](https://github.com/user-attachments/assets/3e18508c-4605-43a6-921c-6f4990730db0)

- press "F5" to view the pseudocode of said function
  
- using the open source nvidia driver [https://github.com/NVIDIA/open-gpu-kernel-modules/blob/315fd96d2d1fbe429a50bdb260d2220f8ef6b10f/src/nvidia/src/kernel/diagnostics/journal.c#L1497] we can identify that the uuid of the gpu(s) are located near a function that iterates through an int, and has an if case that consists of an int64 variable plus an arbitrary offset that calls a function if true which contains the same variable, but referencing the same offset but increased by one decimal value as the third parameter.

- using this logic we can safely assume that this offset (when comparing the pseudocode to the open source driver) is the hex value of the uuid variable that we need to write to.
  ![image](https://github.com/user-attachments/assets/4105f9dd-8ec5-498d-9b99-51c1ac094587)
  ![image](https://github.com/user-attachments/assets/e5c4e3eb-a5cb-44f2-85bd-f28e38d678a0)

- lastly convert the given decimal offset which in this case is 2861 to its hex equivalent, which in this case is B2D / 0xB2D.


