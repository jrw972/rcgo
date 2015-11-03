#!/bin/bash

source $srcdir/helpers.sh

# Test the intrinsic and dereference mutability semantics.
# Test description is:
# (intrinsic mutability) (dereference mutability) (op) (intrinsic mutability) (dereference mutability) (type)

echo 1..84

expect_success 1  "mutable mutable = mutable mutable int"        mutability_mm_a_mms.rc
expect_success 2  "mutable mutable = mutable mutable *int"       mutability_mm_a_mmp.rc
expect_success 3  "mutable mutable = mutable immutable int"      mutability_mm_a_mis.rc
expect_error   4  "mutable mutable = mutable immutable *int"     mutability_mm_a_mip.rc E123
expect_success 5  "mutable mutable = mutable foreign int"        mutability_mm_a_mfs.rc
expect_error   6  "mutable mutable = mutable foreign *int"       mutability_mm_a_mfp.rc E123
expect_success 7  "mutable mutable = immutable mutable int"      mutability_mm_a_ims.rc
expect_success 8  "mutable mutable = immutable mutable *int"     mutability_mm_a_imp.rc
expect_success 9  "mutable mutable = immutable immutable int"    mutability_mm_a_iis.rc
expect_error   10 "mutable mutable = immutable immutable *int"   mutability_mm_a_iip.rc E123
expect_success 11 "mutable mutable = immutable foreign int"      mutability_mm_a_ifs.rc
expect_error   12 "mutable mutable = immutable foreign *int"     mutability_mm_a_ifp.rc E123
expect_success 13 "mutable mutable = foreign mutable int"        mutability_mm_a_fms.rc
expect_success 14 "mutable mutable = foreign mutable *int"       mutability_mm_a_fmp.rc
expect_success 15 "mutable mutable = foreign immutable int"      mutability_mm_a_fis.rc
expect_error   16 "mutable mutable = foreign immutable *int"     mutability_mm_a_fip.rc E123
expect_success 17 "mutable mutable = foreign foreign int"        mutability_mm_a_ffs.rc
expect_error   18 "mutable mutable = foreign foreign *int"       mutability_mm_a_ffp.rc E123

expect_success 19 "mutable immutable = mutable mutable int"      mutability_mi_a_mms.rc
expect_success 20 "mutable immutable = mutable mutable *int"     mutability_mi_a_mmp.rc
expect_success 21 "mutable immutable = mutable immutable int"    mutability_mi_a_mis.rc
expect_success 22 "mutable immutable = mutable immutable *int"   mutability_mi_a_mip.rc
expect_success 23 "mutable immutable = mutable foreign int"      mutability_mi_a_mfs.rc
expect_error   24 "mutable immutable = mutable foreign *int"     mutability_mi_a_mfp.rc E123
expect_success 25 "mutable immutable = immutable mutable int"    mutability_mi_a_ims.rc
expect_success 26 "mutable immutable = immutable mutable *int"   mutability_mi_a_imp.rc
expect_success 27 "mutable immutable = immutable immutable int"  mutability_mi_a_iis.rc
expect_success 28 "mutable immutable = immutable immutable *int" mutability_mi_a_iip.rc
expect_success 29 "mutable immutable = immutable foreign int"    mutability_mi_a_ifs.rc
expect_error   30 "mutable immutable = immutable foreign *int"   mutability_mi_a_ifp.rc E123
expect_success 31 "mutable immutable = foreign mutable int"      mutability_mi_a_fms.rc
expect_success 32 "mutable immutable = foreign mutable *int"     mutability_mi_a_fmp.rc
expect_success 33 "mutable immutable = foreign immutable int"    mutability_mi_a_fis.rc
expect_success 34 "mutable immutable = foreign immutable *int"   mutability_mi_a_fip.rc
expect_success 35 "mutable immutable = foreign foreign int"      mutability_mi_a_ffs.rc
expect_error   36 "mutable immutable = foreign foreign *int"     mutability_mi_a_ffp.rc E123

expect_success 37 "mutable foreign = mutable mutable int"        mutability_mf_a_mms.rc
expect_success 38 "mutable foreign = mutable mutable *int"       mutability_mf_a_mmp.rc
expect_success 39 "mutable foreign = mutable immutable int"      mutability_mf_a_mis.rc
expect_success 40 "mutable foreign = mutable immutable *int"     mutability_mf_a_mip.rc
expect_success 41 "mutable foreign = mutable foreign int"        mutability_mf_a_mfs.rc
expect_success 42 "mutable foreign = mutable foreign *int"       mutability_mf_a_mfp.rc
expect_success 43 "mutable foreign = immutable mutable int"      mutability_mf_a_ims.rc
expect_success 44 "mutable foreign = immutable mutable *int"     mutability_mf_a_imp.rc
expect_success 45 "mutable foreign = immutable immutable int"    mutability_mf_a_iis.rc
expect_success 46 "mutable foreign = immutable immutable *int"   mutability_mf_a_iip.rc
expect_success 47 "mutable foreign = immutable foreign int"      mutability_mf_a_ifs.rc
expect_success 48 "mutable foreign = immutable foreign *int"     mutability_mf_a_ifp.rc
expect_success 49 "mutable foreign = foreign mutable int"        mutability_mf_a_fms.rc
expect_success 50 "mutable foreign = foreign mutable *int"       mutability_mf_a_fmp.rc
expect_success 51 "mutable foreign = foreign immutable int"      mutability_mf_a_fis.rc
expect_success 52 "mutable foreign = foreign immutable *int"     mutability_mf_a_fip.rc
expect_success 53 "mutable foreign = foreign foreign int"        mutability_mf_a_ffs.rc
expect_success 54 "mutable foreign = foreign foreign *int"       mutability_mf_a_ffp.rc

expect_error   55 "immutable mutable = mutable mutable int"      mutability_im_a_mms.rc E45
# Other cases are redundant.
expect_error   56 "foreign mutable = mutable mutable int"        mutability_fm_a_mms.rc E45
# Other cases are redundant.

expect_success 57 "*(mutable mutable) = value"                   mutability_dmm_a.rc
expect_error   58 "*(mutable immutable) = value"                 mutability_dmi_a.rc E45
expect_error   59 "*(mutable foreign) = value"                   mutability_dmf_a.rc E45

expect_success 60 "mutable mutable = *(mutable mutable)"         mutability_a_dmm.rc
expect_error   61 "mutable mutable = *(mutable immutable)"       mutability_a_dmi.rc E123
expect_error   62 "mutable mutable = *(mutable foreign)"         mutability_a_dmf.rc E123

expect_success 63 "mutable mutable = &(mutable mutable)"         mutability_a_rmm.rc
expect_error   64 "mutable mutable = &(immutable mutable)"       mutability_a_rim.rc E123
expect_error   65 "mutable mutable = &(foreign mutable)"         mutability_a_rfm.rc E123

expect_error   66 "mutable mutable = &(mutable immutable)"       mutability_a_rmi.rc E123
expect_error   67 "mutable mutable = &(immutable immutable)"     mutability_a_rii.rc E123
expect_error   68 "mutable mutable = &(foreign immutable)"       mutability_a_rfi.rc E123

expect_error   69 "mutable immutable = &(mutable foreign)"       mutability_a_rmf.rc E123
expect_error   70 "mutable immutable = &(immutable foreign)"     mutability_a_rif.rc E123
expect_error   71 "mutable immutable = &(foreign foreign)"       mutability_a_rff.rc E123

expect_success 72 "(mutable mutable). = value"                   mutability_mms_a.rc
expect_error   73 "(immutable mutable). = value"                 mutability_ims_a.rc E45
expect_error   74 "(foreign mutable). = value"                   mutability_fms_a.rc E45

expect_success 75 "mutable mutable = (mutable mutable)."         mutability_a_mms.rc
expect_error   76 "mutable mutable = (mutable immutable)."       mutability_a_mis.rc E123
expect_error   77 "mutable mutable = (mutable foreign)."         mutability_a_mfs.rc E123

expect_success 78 "(mutable mutable)[] = value"                  mutability_mmi_a.rc
expect_error   79 "(immutable mutable)[] = value"                mutability_imi_a.rc E45
expect_error   80 "(foreign mutable)[] = value"                  mutability_fmi_a.rc E45

expect_success 81 "mutable mutable = (mutable mutable)[]"        mutability_a_mmi.rc
expect_error   82 "mutable mutable = (mutable immutable)[]"      mutability_a_mii.rc E123
expect_error   83 "mutable mutable = (mutable foreign)[]"        mutability_a_mfi.rc E123

expect_error   84 "pointer in struct"                            mutability_pis.rc E123
