/*	$NecBSD: tmc18c30_pisa.c,v 1.22 1998/11/26 01:59:21 honda Exp $	*/
/*	$NetBSD$	*/

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * [Ported for FreeBSD]
 *  Copyright (c) 2000
 *      Noriaki Mitsunaga, Mitsuru Iwasaki and Takanori Watanabe.
 *      All rights reserved.
 * [NetBSD for NEC PC-98 series]
 *  Copyright (c) 1996, 1997, 1998
 *	NetBSD/pc98 porting staff. All rights reserved.
 *  Copyright (c) 1996, 1997, 1998
 *	Naofumi HONDA. All rights reserved.
 *  Copyright (c) 1996, 1997, 1998
 *	Kouichi Matsuda. All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/bus.h>
#include <sys/malloc.h>
#include <sys/errno.h>

#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/rman.h>

#include <cam/scsi/scsi_low.h>

#include <dev/stg/tmc18c30reg.h>
#include <dev/stg/tmc18c30var.h>
#include <dev/stg/tmc18c30.h>

static int
stg_isa_probe(device_t dev)
{
	struct stg_softc	*sc = device_get_softc(dev);
	int			error;

	sc->port_rid = 0;
	sc->irq_rid = 0;
	error = stg_alloc_resource(dev);
	if (error) {
		return(error);
	}

	if (stg_probe(dev) == 0) {
		stg_release_resource(dev);
		return(ENXIO);
	}

	stg_release_resource(dev);

	return (BUS_PROBE_DEFAULT);
}

static int
stg_isa_attach(device_t dev)
{
	struct stg_softc	*sc = device_get_softc(dev);
	int			error;

	sc->port_rid = 0;
	sc->irq_rid = 0;
	error = stg_alloc_resource(dev);
	if (error) {
		return(error);
	}

	error = bus_setup_intr(dev, sc->irq_res, INTR_TYPE_CAM | INTR_ENTROPY |
	    INTR_MPSAFE, NULL, stg_intr, sc, &sc->stg_intrhand);
	if (error) {
		stg_release_resource(dev);
		return(error);
	}

	if (stg_attach(dev) == 0) {
		stg_release_resource(dev);
		return(ENXIO);
	}

	return(0);
}

static device_method_t stg_isa_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		stg_isa_probe),
	DEVMETHOD(device_attach,	stg_isa_attach),
	DEVMETHOD(device_detach,	stg_detach),

	{ 0, 0 }
};

static driver_t stg_isa_driver = {
	"stg",
	stg_isa_methods,
	sizeof(struct stg_softc),
};

DRIVER_MODULE(stg, isa, stg_isa_driver, stg_devclass, 0, 0);
MODULE_DEPEND(stg, scsi_low, 1, 1, 1);
