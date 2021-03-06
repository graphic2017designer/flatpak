AM_TESTS_ENVIRONMENT = FLATPAK_TESTS_DEBUG=1 \
	FLATPAK_TRIGGERSDIR=$$(cd $(top_srcdir) && pwd)/triggers \
	FLATPAK_DBUSPROXY=$$(cd $(top_builddir) && pwd)/flatpak-dbus-proxy \
	GI_TYPELIB_PATH=$$(cd $(top_builddir) && pwd)$${GI_TYPELIB_PATH:+:$$GI_TYPELIB_PATH} \
	LD_LIBRARY_PATH=$$(cd $(top_builddir)/.libs && pwd)$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH} \
	PATH=$$(cd $(top_builddir) && pwd):$${PATH} \
	$(NULL)

if WITH_SYSTEM_BWRAP
AM_TESTS_ENVIRONMENT += FLATPAK_BWRAP=$(BWRAP)
else
AM_TESTS_ENVIRONMENT += FLATPAK_BWRAP=$$(cd $(top_builddir) && pwd)/flatpak-bwrap
endif

testlibrary_CFLAGS = $(AM_CFLAGS) $(BASE_CFLAGS) $(OSTREE_CFLAGS) -DFLATPAK_COMPILATION
testlibrary_LDADD = \
             $(AM_LDADD) \
             $(BASE_LIBS) \
             $(OSTREE_LIBS) \
             libglnx.la \
             libflatpak.la \
             $(NULL)
testlibrary_SOURCES = tests/testlibrary.c

tests/services/org.freedesktop.Flatpak.service: session-helper/org.freedesktop.Flatpak.service.in
	mkdir -p tests/services
	$(AM_V_GEN) $(SED) -e "s|\@libexecdir\@|$(abs_top_builddir)|" $< > $@

tests/services/org.freedesktop.Flatpak.SystemHelper.service: system-helper/org.freedesktop.Flatpak.SystemHelper.service.in
	mkdir -p tests/services
	$(AM_V_GEN) $(SED) -e "s|\@libexecdir\@|$(abs_top_builddir)|" -e "s|\@extraargs\@| --session --no-idle-exit|" $< > $@

tests/libtest.sh: tests/services/org.freedesktop.Flatpak.service tests/services/org.freedesktop.Flatpak.SystemHelper.service

install-test-data-hook:
if ENABLE_INSTALLED_TESTS
	mkdir -p $(DESTDIR)$(installed_testdir)/services
	ln -sf $(dbus_servicedir)/org.freedesktop.Flatpak.service $(DESTDIR)$(installed_testdir)/services/
	$(AM_V_GEN) $(SED) -e "s|\@libexecdir\@|$(libexecdir)|" -e "s|\@extraargs\@| --session --no-idle-exit|" $(top_srcdir)/system-helper/org.freedesktop.Flatpak.SystemHelper.service.in > $(DESTDIR)$(installed_testdir)/services/org.freedesktop.Flatpak.SystemHelper.service
endif

tests/package_version.txt: Makefile
	echo $(PACKAGE_VERSION) > tests/package_version.txt

tests/test-basic.sh: tests/package_version.txt

dist_installed_test_extra_scripts += \
	buildutil/tap-driver.sh \
	tests/make-multi-collection-id-repo.sh \
	tests/make-test-app.sh \
	tests/make-test-runtime.sh \
	tests/make-test-bundles.sh \
	tests/test-webserver.sh \
	$(NULL)

dist_installed_test_data = \
	tests/libtest.sh \
	tests/org.test.Hello.png \
	tests/package_version.txt \
	tests/session.conf.in \
	$(NULL)

installed_test_keyringdir = $(installed_testdir)/test-keyring
installed_test_keyring2dir = $(installed_testdir)/test-keyring2

if ENABLE_INSTALLED_TESTS
dist_installed_test_keyring_DATA = \
	tests/test-keyring/README \
	tests/test-keyring/pubring.gpg \
	tests/test-keyring/secring.gpg \
	$(NULL)
dist_installed_test_keyring2_DATA = \
	tests/test-keyring2/README \
	tests/test-keyring2/pubring.gpg \
	tests/test-keyring2/secring.gpg \
	$(NULL)
endif

dist_test_scripts = \
	tests/test-basic.sh \
	tests/test-build-update-repo.sh \
	tests/test-run.sh \
	tests/test-run-system.sh \
	tests/test-run-deltas.sh \
	tests/test-run-system-deltas.sh \
	tests/test-repo.sh \
	tests/test-repo-collections.sh \
	tests/test-repo-collections-server-only.sh \
	tests/test-repo-system.sh \
	tests/test-extensions.sh \
	tests/test-bundle.sh \
	tests/test-bundle-system.sh \
	tests/test-oci.sh \
	tests/test-unsigned-summaries.sh \
	tests/test-update-remote-configuration.sh \
	$(NULL)

test_programs = testlibrary

@VALGRIND_CHECK_RULES@
VALGRIND_SUPPRESSIONS_FILES=tests/flatpak.supp tests/glib.supp
EXTRA_DIST += tests/flatpak.supp tests/glib.supp
DISTCLEANFILES += \
	tests/services/org.freedesktop.Flatpak.service \
	tests/services/org.freedesktop.Flatpak.SystemHelper.service \
	tests/package_version.txt \
	$(NULL)
