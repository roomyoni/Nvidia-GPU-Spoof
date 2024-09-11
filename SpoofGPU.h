#pragma once
#include <ntifs.h>
#include <wdm.h>

typedef unsigned long long uint64_t;
typedef unsigned int       uint32_t;

namespace GPU {
	NTSTATUS SpoofGpu()
	{
		uint64_t nvlddmkm_base = (uint64_t)GetModuleBase("nvlddmkm.sys");
		if (!nvlddmkm_base)
		{
			return STATUS_UNSUCCESSFUL;
		}
		DebugPrint("Found nv base: %p\n", nvlddmkm_base);
		//uint32_t UuidValidOffset = 0xA84;
		uint32_t UuidValidOffset = 0xB2C;
		uint64_t found = (uint64_t)FindPatternImage((PVOID)nvlddmkm_base,
			skCrypt("\xE8\x00\x00\x00\x00\x48\x8B\xD8\x48\x85\xC0\x0F\x84\x00\x00\x00\x00\x44\x8B\x80\x00\x00\x00\x00\x48\x8D\x15"),
			skCrypt("x????xxxxxxxx????xxx????xxx"));
		if (!found || *(uint8_t*)(found + 0x3C) != 0xE8)
		{
			DebugPrint("Could not find gpu pattern\n");
			return STATUS_UNSUCCESSFUL;
		}
		else
		{
			uintptr_t offset = found - nvlddmkm_base;
			DebugPrint("Found gpu pattern at offset: 0x%llx\n", offset);
		}

		uint64_t found_ = found;
		found_ += *(int*)(found_ + 1) + 5;
		uint64_t(*result)(int) = (uint64_t(*) (int)) (found_);

		found += 0x3C;

		found += *(int*)(found + 1) + 5;


		if (!UuidValidOffset)
		{
			DebugPrint("Failed to find offset\n");
			return STATUS_UNSUCCESSFUL;
		}
		else
		{
			DebugPrint("Found offset\n");
		}

		for (int i = 0; i < 32; i++)
		{
			uint64_t ProbedGPU = result(i);



			if (!ProbedGPU)
			{
				continue;
			}
			else
			{
				uintptr_t offset = ProbedGPU - nvlddmkm_base;
				DebugPrint("Found probed gpu pattern at offset: %p\n", ProbedGPU);


				for (int j = 0; j < 1; j++)
				{
					if (!*(BOOL*)(ProbedGPU + UuidValidOffset))
					{
						DebugPrint("ProbedGPU + UuidValidOffset invalid\n");
						UuidValidOffset += 1;
					}
					else
					{
						DebugPrint("Found GPU!\n");
						//GUID* originalguid = (GUID*)(ProbedGPU + UuidValidOffset + 1);
						GUID* currentUUID = reinterpret_cast<GUID*>(ProbedGPU + UuidValidOffset + 1);
						if (IsGuidFormatProper(currentUUID))
						{
							DebugPrint("GUID is valid!\n");
							GUID newUUID = GenerateRandomGUID();
							//GenerateRandomGUID(newUUID);
							//ExUuidCreate(&newUUID);
							const uint8_t* newUUIDBytes = reinterpret_cast<const uint8_t*>(&newUUID);
							for (int b = 0; b < sizeof(GUID); b++) {
								*(uint8_t*)(ProbedGPU + UuidValidOffset + 1 + b) = newUUIDBytes[b];
							}
						}
						else
						{
							DebugPrint("GUID is invalid!\n");
						}
					/*	GUID newUUID;
						ExUuidCreate(&newUUID);
						const uint8_t* newUUIDBytes = reinterpret_cast<const uint8_t*>(&newUUID);
						for (int b = 0; b < sizeof(GUID); b++) {
							*(uint8_t*)(ProbedGPU + UuidValidOffset + 1 + b) = newUUIDBytes[b];
						}*/
					}
				}
			}
		}


		return STATUS_SUCCESS;
	}
}