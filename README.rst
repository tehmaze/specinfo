==========
 specinfo
==========

Gather information from a spec file, as used by the RPM Package Manager
build specification files.


Requirements
============

This was tested on RPM version 4 (Red Hat Enterprise Linux 5.6). You need
at least the ``rpm-build`` and ``gcc`` packages installed.


Example
=======

::

    $ ./specinfo test.spec
    file:       test.spec
    name:       test
    version:    0
    release:    0.RHEL5_x64
    group:      test
    license:    test
    summary:    test
    requires:   %{requires}
    provides:   %{provides}
    obsoletes:  %{obsoletes}

