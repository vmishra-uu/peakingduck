.. peakingduck documentation master file, created by
   sphinx-quickstart on Tue Feb 11 15:31:21 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to peakingduck's documentation!
=======================================

About
-----

Peaking identification is crucial for gamma spectroscopy
and nuclear analysis. Conventional methods (although included) are not
great at finding peaks in areas of low statistics and often fail for
multiplet identification (overlapping peaks). A new method involving
deep learning methods has been developed to improve both precision and
recall of peaks. This library contains some traditional algorithms for
peak identification and some neural networks used for more modern
approaches. The intention is to provide further analysis in the future
(peak fitting, background subtraction, etc) but for the minute just
focuses on peak identification. This can also be extended (in theory)
to any 1 dimension data set containing labelled peaks.

Status
------

Very much a work in progress, but is expected to have the first
version (0.0.1) ready for release in 2 months.

Building
--------

It is header only C++ so nothing to build (only unit tests) if using
it in C++.  If you want python bindings enabled then it needs building
(default will build them), as below.

::

    git clone --recursive -j8 https://github.com/fispact/peakingduck
    cd peakingduck
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_PY_BINDINGS=ON ..
    make -j4 ```
  
Note: Project uses cmake (> 3.2) to build peaking duck.


.. toctree::
   :maxdepth: 3
   :caption: Contents:

   installation
   1d_data
   peak_finding
   cpp_interface
   python_interface
   api/peakingduck
   cpp_api/peakingduck


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
