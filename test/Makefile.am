LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
             $(top_srcdir)/build-aux/tap-driver.sh

AM_TESTS_ENVIRONMENT = RC='$(top_builddir)/src/rc'; export RC;
#LOG_COMPILER = $(SH)

TESTS = read_immutable.sh \
write_immutable.sh \
immutable_leak.sh \
immutable_leak2.sh \
immutable_leak3.sh \
port_leak.sh \
reaction_leak.sh \
read_foreign.sh \
write_foreign.sh \
save_foreign.sh \
boolean_operators.sh \
int_operators.sh \
variable_zero.sh \
two_port_calls.sh \
reaction_param_activation.sh \
return_leak.sh \
two_reactions.sh \
same_port.sh \
change_root-1.sh \
change_root-2.sh

EXTRA_DIST = $(TESTS) \
read_immutable.rc \
write_immutable.rc \
immutable_leak.rc \
immutable_leak2.rc \
immutable_leak3.rc \
port_leak.rc \
reaction_leak.rc \
read_foreign.rc \
write_foreign.rc \
save_foreign.rc \
boolean_operators.rc \
int_operators.rc \
variable_zero.rc \
two_port_calls.rc \
reaction_param_activation.rc \
return_leak.rc \
two_reactions.rc \
same_port.rc \
change_root-1.rc \
change_root-2.rc
