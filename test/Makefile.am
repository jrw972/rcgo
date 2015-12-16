LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/build-aux/tap-driver.sh

AM_TESTS_ENVIRONMENT = RC='$(top_builddir)/src/rc'; export RC; PATH="$(top_builddir)/src:$$PATH"; export PATH;
#LOG_COMPILER = $(SH)

AM_CXXFLAGS = -I $(top_srcdir)/src

check_PROGRAMS = heap
heap_SOURCES = heap.cpp
heap_LDADD = $(top_builddir)/src/librc.la

TESTS = heap \
mutability.sh \
foreign_safe.sh \
boolean_type.rc \
int_type.rc \
heap_type.rc \
variable_initialization.rc \
two_port_calls.sh \
reaction_param_activation.sh \
return_leak.sh \
two_reactions.sh \
initializer_args.rc \
var.rc \
logical_operators.rc \
integer_literals.rc \
float_literals.rc \
imaginary_literals.rc \
rune_literals.rc \
illegal_rune_literals.sh \
string_literals.rc \
illegal_string_literals.sh \
conversion.rc \
illegal_conversion.sh \
illegal_composition.sh \
call.rc

EXTRA_DIST = $(TESTS) \
helpers.sh \
mutability_a_dmf.rc \
mutability_a_dmi.rc \
mutability_a_dmm.rc \
mutability_a_mfi.rc \
mutability_a_mfs.rc \
mutability_a_mii.rc \
mutability_a_mis.rc \
mutability_a_mmi.rc \
mutability_a_mms.rc \
mutability_a_rff.rc \
mutability_a_rfi.rc \
mutability_a_rfm.rc \
mutability_a_rif.rc \
mutability_a_rii.rc \
mutability_a_rim.rc \
mutability_a_rmf.rc \
mutability_a_rmi.rc \
mutability_a_rmm.rc \
mutability_dmf_a.rc \
mutability_dmi_a.rc \
mutability_dmm_a.rc \
mutability_fm_a_mms.rc \
mutability_fmi_a.rc \
mutability_fms_a.rc \
mutability_im_a_mms.rc \
mutability_imi_a.rc \
mutability_ims_a.rc \
mutability_mf_a_ffp.rc \
mutability_mf_a_ffs.rc \
mutability_mf_a_fip.rc \
mutability_mf_a_fis.rc \
mutability_mf_a_fmp.rc \
mutability_mf_a_fms.rc \
mutability_mf_a_ifp.rc \
mutability_mf_a_ifs.rc \
mutability_mf_a_iip.rc \
mutability_mf_a_iis.rc \
mutability_mf_a_imp.rc \
mutability_mf_a_ims.rc \
mutability_mf_a_mfp.rc \
mutability_mf_a_mfs.rc \
mutability_mf_a_mip.rc \
mutability_mf_a_mis.rc \
mutability_mf_a_mmp.rc \
mutability_mf_a_mms.rc \
mutability_mi_a_ffp.rc \
mutability_mi_a_ffs.rc \
mutability_mi_a_fip.rc \
mutability_mi_a_fis.rc \
mutability_mi_a_fmp.rc \
mutability_mi_a_fms.rc \
mutability_mi_a_ifp.rc \
mutability_mi_a_ifs.rc \
mutability_mi_a_iip.rc \
mutability_mi_a_iis.rc \
mutability_mi_a_imp.rc \
mutability_mi_a_ims.rc \
mutability_mi_a_mfp.rc \
mutability_mi_a_mfs.rc \
mutability_mi_a_mip.rc \
mutability_mi_a_mis.rc \
mutability_mi_a_mmp.rc \
mutability_mi_a_mms.rc \
mutability_mm_a_ffp.rc \
mutability_mm_a_ffs.rc \
mutability_mm_a_fip.rc \
mutability_mm_a_fis.rc \
mutability_mm_a_fmp.rc \
mutability_mm_a_fms.rc \
mutability_mm_a_ifp.rc \
mutability_mm_a_ifs.rc \
mutability_mm_a_iip.rc \
mutability_mm_a_iis.rc \
mutability_mm_a_imp.rc \
mutability_mm_a_ims.rc \
mutability_mm_a_mfp.rc \
mutability_mm_a_mfs.rc \
mutability_mm_a_mip.rc \
mutability_mm_a_mis.rc \
mutability_mm_a_mmp.rc \
mutability_mm_a_mms.rc \
mutability_mmi_a.rc \
mutability_mms_a.rc \
mutability_pis.rc \
foreign_safe_getter.rc \
foreign_safe_getter_return.rc \
foreign_safe_initializer.rc \
foreign_safe_pull_port.rc \
foreign_safe_pull_port_return.rc \
foreign_safe_push_port.rc \
foreign_safe_reaction.rc \
two_port_calls.rc \
reaction_param_activation.rc \
return_leak.rc \
two_reactions.rc \
recursive_composition.rc \
same_port.rc \
incompatible_reactions.rc \
write_in_method.rc \
too_many_characters.rc \
too_few_hexadecimal_digits.rc \
too_few_octal_digits.rc \
rune_surrogate_half.rc \
rune_invalid_unicode_code_point.rc \
illegal_octal.rc \
string_surrogate_half.rc \
string_invalid_unicode_code_point.rc \
illegal_conversion1.rc \
illegal_conversion2.rc \
illegal_conversion3.rc \
illegal_conversion4.rc
