%global _cmake_opts -DCMAKE_BUILD_TYPE=Release -DUSE_INSTALLED_ONIGURUMA=ON

Name:           qsp
Version:        5.9.4
Release:        1%{?dist}
Summary:        QSP game engine shared library

License:        MPL-2.0 AND BSD-2-Clause AND BSD-3-Clause
URL:            https://github.com/QSPFoundation/qsp
Source0:        %{url}/archive/refs/tags/v%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(oniguruma)
BuildRequires:  oniguruma-devel

%description
QSP is an engine for creating and running text-based quest games. This package
provides the shared library and C headers required to build applications
against QSP. Fedora ships only the optimized upstream release build, compiled
without the `_DEBUG` flag that would enable the bundled GPL-2.0-or-later
MemWatch debugger; the shipped binaries therefore remain under MPL-2.0 and BSD
licenses. Rebuilding with debug instrumentation would introduce GPL-2.0-or-
later licensing obligations for the resulting artifacts.

%prep
%autosetup -n %{name}-%{version}

%build
%cmake %{_cmake_opts}
(cd %{_vpath_builddir} && grep -E "/usr/lib.*/libonig\\.so" CMakeFiles/CMakeOutput.log)
%cmake_build

%check
pushd packaging/fedora/tests
gcc $(pkg-config --cflags oniguruma) -I../../.. check_onig_encoding.c -o check_onig_encoding $(pkg-config --libs oniguruma)
./check_onig_encoding
popd

%install
%cmake_install

%files
%license LICENSE LICENSE_BSD_2Clause LICENSE_BSD_3Clause LICENSE_MPL2
%doc README.md
%{_libdir}/libqsp.so*
%{_includedir}/qsp/
%{_libdir}/cmake/Qsp/

%changelog
* Tue May 28 2024 Your Name <you@example.com> - 5.9.4-1
- Initial packaging for Fedora
