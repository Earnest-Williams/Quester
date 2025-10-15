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
BuildRequires:  oniguruma-devel

%description
QSP is an engine for creating and running text-based quest games. This package
provides the shared library and C headers required to build applications
against QSP.

%prep
%autosetup -n %{name}-%{version}

%build
%cmake %{_cmake_opts}
%cmake_build

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
