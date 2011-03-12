Name: test
Version: 0
Release: 0.%{buildhost}
Group: test
Summary: test
License: test

%description
test

%prep
echo prep section

%build
gcc --version

%install
mkdir -p "%buildroot";

%files
