require 'mkmf'
CONFIG['CXX'] = "g++ -Wno-deprecated"
have_library("boost_iostreams-mt")
have_library("boost_filesystem-mt")
create_makefile('connectivity')


