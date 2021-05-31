
import os
import ycm_core
 
flags=[
    '-Wall',
    '-Wextra',
    '-Werror',
    '-Wno-long-long',
    '-Wno-variadic-macros',
    '-fexceptions',
    '-DNDEBUG',
    '-std=c99',
    '-x',
    'c',
    '-D__KERNEL__',
    '-isystem',
    '/home/fenghan/arm-linux/linux-2.6.22.6/include/',
    ]
 
SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', ]
 
def FlagsForFile( filename, **kwargs ):
  return {
    'flags': flags,
    'do_cache': True
}
