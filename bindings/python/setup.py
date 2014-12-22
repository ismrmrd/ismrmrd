import os
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import numpy

ismrmrd_home = os.environ.get('ISMRMRD_HOME', '')

ext = Extension(
    "ismrmrd",
    ["cismrmrd.pxd", "ismrmrd.pyx"],
    include_dirs=[os.path.join(ismrmrd_home, 'include'),
            '/usr/local/include', '/usr/include',
            numpy.get_include()],
    library_dirs=[os.path.join(ismrmrd_home, 'lib'),
            '/usr/local/lib', '/usr/lib'],
    libraries=["ismrmrd"],
    extra_link_args=[],
)

setup(
    name='ismrmrd',
    version='1.2.0',
    author='ISMRMRD Developers',
    author_email='ismrmrd@googlegroups.com',
    description='Python bindings to ISMRMRD',
    license='Public Domain',
    keywords='ismrmrd',
    url='https://ismrmrd.github.io',
    packages=[],
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Science/Research',
        'License :: Public Domain',
        'Operating System :: OS Independent',
        'Programming Language :: Cython',
        'Topic :: Scientific/Engineering :: Medical Science Apps.'
    ],
    requires=['Cython', 'numpy'],

    ext_modules=[ext],
    cmdclass={'build_ext':build_ext}
)
