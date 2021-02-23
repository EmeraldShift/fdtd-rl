#include "fdtd.hpp"

#include "kernels.hpp"

#include <raft>

#include <cstdlib>

int fdtd(dim_t dimX, dim_t dimY, dim_t dimZ, unsigned long t, unsigned long flags)
{
	// Simulation parameters
	phys::params params(dimX, dimY, dimZ);

	bool print = flags & FLAG_PRT;
	Hx hx(params, t, print);
	Hy hy(params, t);
	Hz hz(params, t);
	Ex ex(params, t);
	Ey ey(params, t);
	Ez ez(params, t);

	raft::map m;

	InitialGridGenerator igh(dimX, dimY, dimZ, 1);
	InitialGridGenerator igex(dimX, dimY, dimZ, 3);
	InitialGridGenerator igey(dimX, dimY, dimZ, 3);
	InitialGridGenerator igez(dimX, dimY, dimZ, 3);

	// Self-loopback & Initial Grids
	m += *(igh.clone()) >> hx["init_Hx"]["out_Hx"] >> hx["Hx"];
	m += *(igh.clone()) >> hy["init_Hy"]["out_Hy"] >> hy["Hy"];
	m += *(igh.clone()) >> hz["init_Hz"]["out_Hz"] >> hz["Hz"];
	m += igex["0"] >> ex["init_Ex"]["out_Ex"] >> ex["Ex"];
	m += igex["1"] >> hy["init_Ex"];
	m += igex["2"] >> hz["init_Ex"];
	m += igey["0"] >> ey["init_Ey"]["out_Ey"] >> ey["Ey"];
	m += igey["1"] >> hz["init_Ey"];
	m += igey["2"] >> hx["init_Ey"];
	m += igez["0"] >> ez["init_Ez"]["out_Ez"] >> ez["Ez"];
	m += igez["1"] >> hx["init_Ez"];
	m += igez["2"] >> hy["init_Ez"];

	// Attach kernels to each other
	m += hx["out_Ey"] >> ey["Hx"]["out_Hx"] >> hx["Ey"];
	m += hx["out_Ez"] >> ez["Hx"]["out_Hx"] >> hx["Ez"];
	m += hy["out_Ez"] >> ez["Hy"]["out_Hy"] >> hy["Ez"];
	m += hy["out_Ex"] >> ex["Hy"]["out_Hy"] >> hy["Ex"];
	m += hz["out_Ex"] >> ex["Hz"]["out_Hz"] >> hz["Ex"];
	m += hz["out_Ey"] >> ey["Hz"]["out_Hz"] >> hz["Ey"];

	// Final output / debugging ports
	bool silent = flags & FLAG_SIL;
	GridPrinter phx(silent), phy(silent), phz(silent),
		    pex(silent), pey(silent), pez(silent);
	m += hx["Final"] >> phx;
	m += hz["Final"] >> phy;
	m += ex["Final"] >> phz;
	m += hy["Final"] >> pex;
	m += ey["Final"] >> pey;
	m += ez["Final"] >> pez;

#ifdef VL
	if (flags & FLAG_VTL) {
#ifdef QT
		if (flags & FLAG_QTH)
			m.exe<partition_dummy, pool_schedule, vlalloc, no_parallel>();
		else
#endif // QT
			m.exe<partition_dummy, simple_schedule, vlalloc, no_parallel>();
	} else
#endif // VL
	if (flags & FLAG_DYN) {
#ifdef QT
		if (flags & FLAG_QTH)
			m.exe<partition_dummy, pool_schedule, dynalloc, no_parallel>();
		else
#endif // QT
		m.exe<partition_dummy, simple_schedule, dynalloc, no_parallel>();
	} else {
#ifdef QT
		if (flags & FLAG_QTH)
			m.exe<partition_dummy, pool_schedule, stdalloc, no_parallel>();
		else
#endif // QT
		m.exe<partition_dummy, simple_schedule, stdalloc, no_parallel>();
	}

	return EXIT_SUCCESS;
}
