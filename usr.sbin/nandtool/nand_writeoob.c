/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2010-2012 Semihalf.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgeom.h>
#include <sys/disk.h>
#include <dev/nand/nand_dev.h>
#include "nandtool.h"

int nand_write_oob(struct cmd_param *params)
{
	struct chip_param_io chip_params;
	struct nand_oob_rw req;
	char *dev, *in;
	int fd = -1, fd_in = -1, ret = 0;
	uint8_t *buf = NULL;
	int page;

	if (!(dev = param_get_string(params, "dev"))) {
		fprintf(stderr, "Please supply valid 'dev' parameter.\n");
		return (1);
	}

	if (!(in = param_get_string(params, "in"))) {
		fprintf(stderr, "Please supply valid 'in' parameter.\n");
		return (1);
	}

	if ((page = param_get_int(params, "page")) < 0) {
		fprintf(stderr, "Please supply valid 'page' parameter.\n");
		return (1);
	}

	if ((fd = g_open(dev, 1)) == -1) {
		perrorf("Cannot open %s", dev);
		return (1);
	}

	if ((fd_in = open(in, O_RDONLY)) == -1) {
		perrorf("Cannot open %s", in);
		ret = 1;
		goto out;
	}

	if (ioctl(fd, NAND_IO_GET_CHIP_PARAM, &chip_params) == -1) {
		perrorf("Cannot ioctl(NAND_IO_GET_CHIP_PARAM)");
		ret = 1;
		goto out;
	}

	buf = malloc(chip_params.oob_size);
	if (buf == NULL) {
		perrorf("Cannot allocate %d bytes\n", chip_params.oob_size);
		ret = 1;
		goto out;
	}

	if (read(fd_in, buf, chip_params.oob_size) == -1) {
		perrorf("Cannot read from %s", in);
		ret = 1;
		goto out;
	}

	req.page = page;
	req.len = chip_params.oob_size;
	req.data = buf;

	if (ioctl(fd, NAND_IO_OOB_PROG, &req) == -1) {
		perrorf("Cannot write OOB to %s", dev);
		ret = 1;
		goto out;
	}

out:
	g_close(fd);
	if (fd_in != -1)
		close(fd_in);
	if (buf)
		free(buf);

	return (ret);
}


