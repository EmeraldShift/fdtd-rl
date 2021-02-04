#include "fdtd.hpp"

#include "kernels.hpp"

#include <raft>

#include <cstdlib>

int fdtd(dim_t dimX, dim_t dimY, dim_t dimZ, unsigned long t)
{
	// Simulation parameters
	phys::params params(dimX, dimY, dimZ);

	Hx hx(params, t);
	Hy hy(params, t);
	Hz hz(params, t);
	Ex ex(params, t);
	Ey ey(params, t);
	Ez ez(params, t);

	raft::map m;

	InitialGridGenerator zgg(dimX, dimY, dimZ);

	// Self-loopback
	Loopback lhx, lhy, lhz, lex, ley, lez;
	m += *(zgg.clone()) >> lhx["initial"] >> hx["Hx"]["out_Hx"] >> lhx["loop"];
	m += *(zgg.clone()) >> lhy["initial"] >> hy["Hy"]["out_Hy"] >> lhy["loop"];
	m += *(zgg.clone()) >> lhz["initial"] >> hz["Hz"]["out_Hz"] >> lhz["loop"];
	m += *(zgg.clone()) >> lex["initial"] >> ex["Ex"]["out_Ex"] >> lex["loop"];
	m += *(zgg.clone()) >> ley["initial"] >> ey["Ey"]["out_Ey"] >> ley["loop"];
	m += *(zgg.clone()) >> lez["initial"] >> ez["Ez"]["out_Ez"] >> lez["loop"];

	// Attach kernels to each other
	Loopback lhxey, lhxez;
	m += *(zgg.clone()) >> lhxey["initial"] >> ey["Hx"];
	m += hx["out_Ey"] >> lhxey["loop"];
	m += *(zgg.clone()) >> lhxez["initial"] >> ez["Hx"];
	m += hx["out_Ez"] >> lhxez["loop"];

	Loopback lhyez, lhyex;
	m += *(zgg.clone()) >> lhyez["initial"] >> ez["Hy"];
	m += hy["out_Ez"] >> lhyez["loop"];
	m += *(zgg.clone()) >> lhyex["initial"] >> ex["Hy"];
	m += hy["out_Ex"] >> lhyex["loop"];

	Loopback lhzex, lhzey;
	m += *(zgg.clone()) >> lhzex["initial"] >> ex["Hz"];
	m += hz["out_Ex"] >> lhzex["loop"];
	m += *(zgg.clone()) >> lhzey["initial"] >> ey["Hz"];
	m += hz["out_Ey"] >> lhzey["loop"];

	Loopback lexhy, lexhz;
	m += *(zgg.clone()) >> lexhy["initial"] >> hy["Ex"];
	m += ex["out_Hy"] >> lexhy["loop"];
	m += *(zgg.clone()) >> lexhz["initial"] >> hz["Ex"];
	m += ex["out_Hz"] >> lexhz["loop"];

	Loopback leyhz, leyhx;
	m += *(zgg.clone()) >> leyhz["initial"] >> hz["Ey"];
	m += ey["out_Hz"] >> leyhz["loop"];
	m += *(zgg.clone()) >> leyhx["initial"] >> hx["Ey"];
	m += ey["out_Hx"] >> leyhx["loop"];

	Loopback lezhx, lezhy;
	m += *(zgg.clone()) >> lezhx["initial"] >> hx["Ez"];
	m += ez["out_Hx"] >> lezhx["loop"];
	m += *(zgg.clone()) >> lezhy["initial"] >> hy["Ez"];
	m += ez["out_Hy"] >> lezhy["loop"];

	// Final output / debugging ports
	GridPrinter phx, phy, phz, pex, pey, pez;
	m += hx["Final"] >> phx;
	m += hz["Final"] >> phy;
	m += ex["Final"] >> phz;
	m += hy["Final"] >> pex;
	m += ey["Final"] >> pey;
	m += ez["Final"] >> pez;

	m.exe();

	return EXIT_SUCCESS;
}
