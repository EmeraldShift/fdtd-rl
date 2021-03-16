#include "fdtd.hpp"

#include "kernels.hpp"

#include <raft>

#include <cstdlib>

int fdtd(dim_t dimX, dim_t dimY, dim_t dimZ, unsigned long t, unsigned long flags)
{
	// Simulation parameters
	phys::params params(dimX, dimY, dimZ);

	Hx hx(params, t, false);
	Hy hy(params, t);
	Hz hz(params, t);
	Ex ex(params, t);
	Ey ey(params, t);
	Ez ez(params, t);

	raft::map m;

	InitialGridGenerator dummy(dimX, dimY, dimZ);

	// Self-loopback & Initial Grids
	m += *(dummy.clone()) >> hx["init_me"];
	m += *(dummy.clone()) >> hy["init_me"];
	m += *(dummy.clone()) >> hz["init_me"];
	m += *(dummy.clone()) >> ex["init_me"];
	m += *(dummy.clone()) >> ey["init_me"];
	m += *(dummy.clone()) >> ez["init_me"];

	// Attach kernels to each other
	m += hx["out_A"] >> ey["B"]["out_B"] >> hx["A"];
	m += hx["out_B"] >> ez["A"]["out_A"] >> hx["B"];
	m += hy["out_A"] >> ez["B"]["out_B"] >> hy["A"];
	m += hy["out_B"] >> ex["A"]["out_A"] >> hy["B"];
	m += hz["out_A"] >> ex["B"]["out_B"] >> hz["A"];
	m += hz["out_B"] >> ey["A"]["out_A"] >> hz["B"];

	// Final output / debugging ports
	bool silent = !(flags & FLAG_PRT);
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
