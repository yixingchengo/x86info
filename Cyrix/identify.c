/*
 *  $Id: identify.c,v 1.13 2002/10/30 03:11:19 davej Exp $
 *  This file is part of x86info. 
 *  (C) 2001 Dave Jones.
 *
 *  Licensed under the terms of the GNU GPL License version 2.
 *
 * Cyrix bits.
 */

#include <stdio.h>
#include "../x86info.h"
#include "Cyrix.h"

/* Decode TLB and cache info descriptors */
void decode_Cyrix_TLB (int x)
{
	switch (x & 0xff) {
	case 0:
		break;
	case 0x70:
		printf ("TLB: 32 entries 4-way associative 4KB pages\n");
		break;
	case 0x80:
		printf ("L1 Cache: 16KB 4-way associative 16 bytes/line\n");
		break;
	}
}

/* Cyrix-specific information */
void Identify_Cyrix (struct cpudata *cpu)
{
	unsigned long eax, ebx, ecx, edx;

	cpu->vendor = VENDOR_CYRIX;

	/* Do standard stuff */
	if (cpu->maxi < 1)
		return;

	cpuid (cpu->number, 1, &eax, &ebx, &ecx, &edx);
	cpu->stepping = eax & 0xf;
	cpu->model = (eax >> 4) & 0xf;
	cpu->family = (eax >> 8) & 0xf;

	switch (tuple(cpu) & 0xff0) {
		case 0x450:	sprintf (cpu->name, "%s", "MediaGX");
					break;

		case 0x520:	sprintf (cpu->name, "%s", "6x86");
					break;
		case 0x524:	sprintf (cpu->name, "%s", "GXm");
					break;

		case 0x600:	sprintf (cpu->name, "%s", "6x86/MX");
					break;
		case 0x620:	sprintf (cpu->name, "%s", "MII");
					break;
		default:	sprintf (cpu->name, "%s", "Unknown CPU");
					break;
	}

	/* Check for presence of extended info */
	if (cpu->maxei < 0x80000000)
		return;

	if (cpu->maxei >= 0x80000001) {
		cpuid (cpu->number, 0x80000001, &eax, &ebx, &ecx, &edx);
		cpu->stepping = eax & 0xf;
		cpu->model = (eax >> 4) & 0xf;
		cpu->family = (eax >> 8) & 0xf;

		switch (cpu->family) {
			case 4:	sprintf (cpu->name, "MediaGX");
					break;
			case 5:	sprintf (cpu->name, "6x86/GXm");
					break;
			case 6:	sprintf (cpu->name, "6x86/MX");
					break;
		}
	}
}

void display_Cyrix_info(struct cpudata *cpu)
{
	unsigned int i, ntlb;
	unsigned long eax, ebx, ecx, edx;

	printf ("Family: %d Model: %d Stepping: %d [%s]\n",
		cpu->family, cpu->model, cpu->stepping, cpu->name);
	get_model_name (cpu);

	decode_feature_flags (cpu);

	printf ("TLB & L1 Cache info\n");
	if (cpu->maxi >= 2 && show_cacheinfo) {
		/* TLB and L1 Cache info */
		ntlb = 255;
		for (i = 0; i < ntlb; i++) {
			cpuid (cpu->number, 2, &eax, &ebx, &ecx, &edx);
			ntlb = eax & 0xff;
			decode_Cyrix_TLB (eax >> 8);
			decode_Cyrix_TLB (eax >> 16);
			decode_Cyrix_TLB (eax >> 24);

			/* ebx and ecx are reserved */
			if ((edx & 0x80000000) == 0) {
				decode_Cyrix_TLB (edx);
				decode_Cyrix_TLB (edx >> 8);
				decode_Cyrix_TLB (edx >> 16);
				decode_Cyrix_TLB (edx >> 24);
			}
		}
	}

	printf ("TLB & L1 Cache info from extended info\n");
	if (cpu->maxei >= 0x80000005 && show_cacheinfo) {
		/* TLB and L1 Cache info */
		ntlb = 255;
		for (i = 0; i < ntlb; i++) {
			cpuid (cpu->number, 0x80000005, &eax, &ebx, &ecx, &edx);
			ntlb = eax & 0xff;
			decode_Cyrix_TLB (ebx >> 8);
			decode_Cyrix_TLB (ebx >> 16);
			decode_Cyrix_TLB (ebx >> 24);

			/* eax and edx are reserved */
			if ((ecx & 0x80000000) == 0) {
				decode_Cyrix_TLB (ecx);
				decode_Cyrix_TLB (ecx >> 8);
				decode_Cyrix_TLB (ecx >> 16);
				decode_Cyrix_TLB (ecx >> 24);
			}
		}
	}
}

