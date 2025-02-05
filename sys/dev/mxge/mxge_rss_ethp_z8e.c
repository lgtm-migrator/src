/*
 * from: FreeBSD: src/sys/tools/fw_stub.awk,v 1.6 2007/03/02 11:42:53 flz
 */
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/linker.h>
#include <sys/firmware.h>
#include <sys/systm.h>
#include <dev/mxge/rss_ethp_z8e.h>

static int
mxge_rss_ethp_z8e_fw_modevent(module_t mod, int type, void *unused)
{
	const struct firmware *fp, *parent;
	int error;
	switch (type) {
	case MOD_LOAD:

		fp = firmware_register("mxge_rss_ethp_z8e", rss_ethp_z8e, 
				       (size_t)rss_ethp_z8e_length,
				       rss_ethp_z8e_uncompressed_length, NULL);
		if (fp == NULL)
			goto fail_0;
		parent = fp;
		return (0);
	fail_0:
		return (ENXIO);
	case MOD_UNLOAD:
		error = firmware_unregister("mxge_rss_ethp_z8e");
		return (error);
	}
	return (EINVAL);
}

static moduledata_t mxge_rss_ethp_z8e_fw_mod = {
        "mxge_rss_ethp_z8e_fw",
        mxge_rss_ethp_z8e_fw_modevent,
        0
};
DECLARE_MODULE(mxge_rss_ethp_z8e_fw, mxge_rss_ethp_z8e_fw_mod, SI_SUB_DRIVERS, SI_ORDER_FIRST);
MODULE_VERSION(mxge_rss_ethp_z8e_fw, 1);
MODULE_DEPEND(mxge_rss_ethp_z8e_fw, firmware, 1, 1, 1);

