/*
 * Copyright (c) 2000 Silicon Graphics, Inc.  All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 * 
 * Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 * Mountain View, CA  94043, or:
 * 
 * http://www.sgi.com 
 * 
 * For further information regarding this notice, see: 
 * 
 * http://oss.sgi.com/projects/GenInfo/SGIGPLNoticeExplan/
 */

#include <stdio.h>
#include <fcntl.h>
#include <volume.h>
#include <sys/stat.h>
#include "md.h"

int
md_get_subvol_stripe(
	char		*dfile,
	sv_type_t	type,
	int		*sunit,
	int		*swidth,
	struct stat64	*sb)
{
	if (sb->st_rdev >> 8 == MD_MAJOR) {
		struct md_array_info_s	md;
		int  fd;

		/* Open device */
		fd = open(dfile, O_RDONLY);
		if (fd == -1)
			return 0;

		/* Is this thing on... */
		if (ioctl(fd, GET_ARRAY_INFO, &md)) {
			fprintf(stderr, "Error getting MD array info from %s\n",
				dfile);
			exit(1);
		}

		/* Check state */
		if (md.state) {
			fprintf(stderr, "MD array %s not in clean state\n",
				dfile);
			exit(1);
		}

		/* Deduct a disk from stripe width on RAID4/5 */
		if (md.level == 4 || md.level == 5)
			md.nr_disks--;

		/* Update sizes */
		*sunit = md.chunk_size >> 9;
		*swidth = *sunit * md.nr_disks;

		return 1;
	}
	return 0;
}