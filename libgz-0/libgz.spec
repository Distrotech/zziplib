# norootforbuild
%define hostproject zziplib
Summary:      LibGZ - libZ-based File-Reader
Name:         libgz
Version:      0.1.2
Release:      1
License:      Artistic
Group:        Development/Libraries
URL:          http://zziplib.sf.net
Vendor:       Guido Draheim <guidod@gmx.de>
Source0:      http://prdownloads.sf.net/%{hostproject}/%{name}-%{version}.tar.bz2
BuildRoot:    %{_tmppath}/%{name}-%{version}-%{release}

Distribution: Original
Packager:     Guido Draheim <guidod@gmx.de>
Requires:      zlib
BuildRequires: zlib-devel

%package devel
Summary:      LibGZ - Development Files
Group:        Development/Libraries
Requires:     libzcat = %version
Requires:     pkgconfig

%description
 : libgz provides read access to gzip-copmpresses files, i.e. "*.gz" ,
 : using compression based solely on free algorithms provided by zlib.

%description devel
 : libgz provides read access to gzip-copmpresses files, i.e. "*.gz" ,
 : using compression based solely on free algorithms provided by zlib.
 these are the header files needed to develop programs using libzcat.
 there are test binaries to hint usage of the library in user programs.

%prep
%setup
%configure

%build
%__make

%install
%__make install DESTDIR=%{buildroot}

%clean
%__rm -rf %{buildroot}

%files
      %defattr(-,root,root)
      %{_libdir}/lib*.so.*

%post
/sbin/ldconfig || true
%postun
/sbin/ldconfig || true

%files devel
      %defattr(-,root,root)
%dir  %{_includedir}/%{name}
      %{_includedir}/%{name}/*
      %{_libdir}/lib*.so
      %{_libdir}/lib*.a
      %{_libdir}/lib*.la
      %{_libdir}/pkgconfig/*
      %{_bindir}/*
