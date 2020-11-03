#!/usr/bin/env python3

# Merlin++: C++ Class Library for Charged Particle Accelerator Simulations
# Copyright (c) 2001-2018 The Merlin++ developers
# This file is covered by the terms the GNU GPL version 2, or (at your option) any later version, see the file COPYING

# This test should fail occasionally (1% of runs).
# random_test.cpp draws "nthrows" values from each random distribution
# random_test.py compares this to the probabilities in random_test_gendata.dat which is generated by random_test_gendata.py (using numpy and GSL)

from __future__ import division, print_function
import numpy
import subprocess
import os, sys
from scipy.stats import chi2

if "--exe" in sys.argv:
	i = sys.argv.index("--exe")
	sys.argv.pop(i)
	exe_name = sys.argv.pop(i)
else:
	exe_name = os.path.join(os.path.dirname(__file__), "random_test")

do_plot  = False
if "plot" in sys.argv:
	do_plot = True
	sys.argv.remove("plot")
	from matplotlib import pyplot

ref_run  = False
if "ref_run" in sys.argv:
	ref_run = True
	sys.argv.remove("ref_run")

seed = 0
if len(sys.argv) > 1:
	seed = int(sys.argv[1]) # zero for seed from clock


def find_data_file(fname):
	paths = ["DeveloperTools/CodeTesting/data/", "data/", "../data/"]

	for path in paths:
		if os.path.exists(os.path.join(path, fname)):
			return os.path.join(path, fname)
	print("Could not find data file '%s'. Try running from cmake build directory, or the executable directory"% fname)
	sys.exit(1)

tol = 0
pval = 1e-3


# read setup file
random_setup_fname = find_data_file("random_test_setup.tfs")
dt = numpy.dtype({"names":["name", "dist", "var1", "var2", "var3", "range_min", "range_max"], "formats":["U32", "U32", "f", "f", "f", "f", "f"]})
random_setup = numpy.loadtxt(random_setup_fname, skiprows=2, dtype=dt)

# read merlin output
results_file = "random_test_output.dat"

try:
	os.remove(results_file)
except OSError:
	pass
print("Running random_test")
args = [str(seed)]

print("running:", [exe_name]+args)
ret = subprocess.call([exe_name]+args)

if ret != 0:
	print("Failed to run executable %s, error %s"%(exe_name, ret))
	exit(ret)

if ref_run:
	print("Made reference file:", results_file)
	exit(0)

try:
	fh = open(results_file)
	head1 = fh.readline()
	head2 = fh.readline()
	fh.close()

	results_data = numpy.loadtxt(results_file, skiprows=2)
except IOError:
	print("Failed to read", results_file)
	exit(1)

metadata = {}
for ms in head1.lstrip("#").split():
	bits = ms.partition("=")
	metadata[bits[0].strip()] = bits[2].strip()

nthrows = metadata["throws"]
col_names = head2.split()

# read test dists
gendata_file = find_data_file("random_test_gendata.dat")
try:
	fh = open(gendata_file)
	head1 = fh.readline()
	head2 = fh.readline()
	fh.close()

	gendata_data = numpy.loadtxt(gendata_file, skiprows=2)
except IOError:
	print("Failed to read", gendata_file)
	exit(1)

all_good = True

for mn, mode in enumerate(random_setup):
	#if mode["dist"] != "poisson": continue
	#if mode["name"] != "uniform_0_1": continue
	print(mode["name"])
	if (mode["name"] != col_names[mn]):
		print("Columns in", results_file, "do not match", random_setup_fname)
		print("You may need to rerun random_test_gendata.py")
		exit(1)

	bs = numpy.arange(int(metadata["bins"])+2)
	ys_m = results_data[:, mn]
	ys_g = gendata_data[:, mn+1]
	expected = ys_g.copy() * int(metadata["throws"])

	if (ys_m.shape != ys_g.shape):
		print("Number of bins in results_file", results_file, "do not match", gendata_file)
		print("You may need to rerun random_test_gendata.py")
		exit(1)

	bin_thresh = 0
	useful_bins = (expected >= bin_thresh)
	df = useful_bins.sum() - 1
	if df < 3:
		all_good == False
		print("  Too few particles tracked")
		continue

	with numpy.errstate(divide='ignore', invalid='ignore'): # hide warning divide by zero
		sq_diff = ((ys_m-expected)/(numpy.sqrt(expected)+expected*tol))**2
		# should be using poisson stats
		# as a quick compensation, lower penalty for bins with low expected counts
		sq_diff[expected<0.1] *= 0.2
	sq_diff[expected <= 0] = 0 # remove nans, from when expected == zero
	chi_sq = sq_diff[useful_bins].sum()

	print("  Chi^2 =", chi_sq, "(df =",df, ")")
	cdf = chi2.cdf(chi_sq, df)
	print("  cdf(chi^2)=", cdf)
	print("  percent of chi^2 higher even if true dist:", (1-cdf)*100)

	if (1-cdf) > pval:
		print(" ",mode["name"], "does not contain significant deviations")

	else:
		all_good = False
		print(" ",mode["name"], "does contain a significant deviation")
		worst_bins = numpy.argsort(sq_diff * useful_bins)
		print("  Worst 10 bins:")
		for i in worst_bins[-10:]:
			print("    Bin %5i: Expected %8.5f, got %i (chi^2=%.2f)"%(i, expected[i], ys_m[i], sq_diff[i]))


	if do_plot:
		pyplot.plot(bs[useful_bins], ys_m[useful_bins], "rx", label="merlin")
		bs_m = bs[expected>=0]
		exp_m = expected[expected>=0]
		pyplot.fill_between(x=bs_m, y1=exp_m-numpy.sqrt(exp_m), y2=exp_m+numpy.sqrt(exp_m), color="k", alpha=0.5)
		pyplot.plot(bs_m, exp_m, "k-", label="reference")
		pyplot.xlabel("Bin number")
		pyplot.tight_layout()
		pyplot.show()

if all_good:
	print("Pass")
	os.remove(results_file)
else:
	print("Fail")
	exit(1)
