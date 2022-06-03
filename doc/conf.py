# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Doxygen generation

from pathlib import Path
import subprocess

this_folder = Path(__file__).parent

def configureDoxyfile(input_dir, output_dir):
    with open(this_folder/'Doxyfile.in', 'r') as file :
        filedata = file.read()

    filedata = filedata.replace('@CMAKE_SOURCE_DIR@', str(input_dir))
    filedata = filedata.replace('@CMAKE_CURRENT_BINARY_DIR@', str(output_dir))

    with open('Doxyfile', 'w') as file:
        file.write(filedata)

doxygen_inputdir = (this_folder/"..").resolve()
doxygen_output = (this_folder/"doxygen").resolve()

if not Path(this_folder/"doxygen").exists():
    configureDoxyfile(doxygen_inputdir, doxygen_output)
    subprocess.check_call('doxygen', cwd=this_folder)

breathe_projects = {'ISMRMRD': str(doxygen_output/"xml")}
breathe_default_project = 'ISMRMRD'

# -- Project information -----------------------------------------------------

project = 'ISMRMRD'
copyright = '2022, ISMRMRD Working Group'
author = 'ISMRMRD Working Group'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ['breathe', 'myst_parser']

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']