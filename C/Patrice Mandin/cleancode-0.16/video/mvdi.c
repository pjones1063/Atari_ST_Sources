/*
	Milan VDI predefined video modes

	Copyright (C) 2002	Patrice Mandin

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include "mvdi.h"

mvdimode_t mvdimodelist[NUM_MVDIMODELIST]={
	{0x1000,640,400,1,INTERLEAVE_PLANES},
	{0x1001,640,400,8,PACKEDPIX_PLANES},
	{0x1002,640,400,15,PACKEDPIX_PLANES},
	{0x1003,640,400,16,PACKEDPIX_PLANES},
	{0x1004,640,400,32,PACKEDPIX_PLANES},

	{0x1010,640,480,1,INTERLEAVE_PLANES},
	{0x1011,640,480,8,PACKEDPIX_PLANES},
	{0x1012,640,480,15,PACKEDPIX_PLANES},
	{0x1013,640,480,16,PACKEDPIX_PLANES},
	{0x1014,640,480,32,PACKEDPIX_PLANES},

	{0x1020,800,608,1,INTERLEAVE_PLANES},
	{0x1021,800,608,8,PACKEDPIX_PLANES},
	{0x1022,800,608,15,PACKEDPIX_PLANES},
	{0x1023,800,608,16,PACKEDPIX_PLANES},
	{0x1024,800,608,32,PACKEDPIX_PLANES},

	{0x1030,1024,768,1,INTERLEAVE_PLANES},
	{0x1031,1024,768,8,PACKEDPIX_PLANES},
	{0x1032,1024,768,15,PACKEDPIX_PLANES},
	{0x1033,1024,768,16,PACKEDPIX_PLANES},
	{0x1034,1024,768,32,PACKEDPIX_PLANES},

	{0x1040,1152,864,1,INTERLEAVE_PLANES},
	{0x1041,1152,864,8,PACKEDPIX_PLANES},
	{0x1043,1152,864,15,PACKEDPIX_PLANES},
	{0x1043,1152,864,16,PACKEDPIX_PLANES},
	{0x1044,1152,864,32,PACKEDPIX_PLANES},

	{0x1050,1280,1024,1,INTERLEAVE_PLANES},
	{0x1051,1280,1024,8,PACKEDPIX_PLANES},
	{0x1052,1280,1024,15,PACKEDPIX_PLANES},
	{0x1053,1280,1024,16,PACKEDPIX_PLANES},
	{0x1054,1280,1024,32,PACKEDPIX_PLANES},

	{0x1060,1600,1200,1,INTERLEAVE_PLANES},
	{0x1061,1600,1200,8,PACKEDPIX_PLANES},
	{0x1062,1600,1200,15,PACKEDPIX_PLANES},
	{0x1063,1600,1200,16,PACKEDPIX_PLANES},
	{0x1064,1600,1200,32,PACKEDPIX_PLANES}
};