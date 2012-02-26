'''
This builds the raster_measure library. It's an SCons configuration file.

Build: "scons"
Targets:
  - cpp, builds all the pure C++ (Default)
  - wrapper, builds the Python wrapper dll
  - tiff_test, cluster_test are two test cases.
Clean: "scons -c"
Reset cache: "rm -rf .scon*"
Debug: "scons --echo"

There is a configuration file called default.cfg. If you want to change
values for a particular OS, like "linux2" or "darwin", then edit
linux2.cfg or darwin.cfg. If you have defaults just for your machine,
then create local.cfg and put those defaults there.

Want to know which of the variables in the cfg files were read? Look
at used.cfg after the build is done.
'''

import os
import sys
import subprocess
import distutils.sysconfig
import SConsDebug
import SConsCheck
from SConsVars import cfg
import logging


logger=logging.getLogger('SconsRoot')
logging.basicConfig(level=logging.DEBUG)

# The goals of the build decide what we find and include.
# 1. Command line determines what targets we need.
# 2. Targets have requirements.
# 3. Availability can determine optional things to include (like tcmalloc).

# Set preferred directories here. There is a hierarchy of what gets chosen.
# 1. Set from command line
# 2. Set from environment variable.
# 3. Set for this particular machine in some config file.
# 4. Set for this architecture.
# 5. Set as common default, such as /usr/include.


# Command-line Options that are arguments to SCons.
file_cpu=int(cfg.get('General','num_jobs'))
num_cpu=int(os.environ.get('NUM_CPU',file_cpu))
SetOption('num_jobs', num_cpu)
logger.info('running with -j %d' % int(GetOption('num_jobs')))

AddOption('--echo', dest='echo', action='store_true',default=False,
          help='This echoes what scons runs as tests in order to debug failed builds.')
AddOption('--tcmalloc', dest='tcmalloc', type='int',action='store',default=0,
      help='Whether to use tcmalloc to debug the heap.')
AddOption('--cpp_compiler', dest='cpp_compiler', action='store', default=None,
          help='Set the C++ compiler.')
AddOption('--c_compiler', dest='c_compiler', action='store', default=None,
          help='Set the C compiler.')

env=Environment()

if GetOption('echo'):
    env['SPAWN']=SConsDebug.echospawn
cpp_compiler=GetOption('cpp_compiler')
cpp_compiler=cpp_compiler or cfg.get('General','cpp_compiler')
cpp_compiler=cpp_compiler or SConsCheck.latest_gcc(env)

c_compiler=GetOption('c_compiler')
c_compiler=c_compiler or cfg.get('General','c_compiler')

if cpp_compiler:
    env['CXX']=cpp_compiler
if c_compiler:
    env['CC']=cpp_compiler


def DisallowSubst():
    #AllowSubstExceptions()
    # Cannot use AllowSubstExceptions without defining these variables
    # which the default tool leaves undefined.
    env.SetDefault(CPPFLAGS=[])
    env.SetDefault(CPPDEFINES=[])
    env.SetDefault(CXXCOMSTR='')
    env.SetDefault(RPATH=[])
    env.SetDefault(LIBPATH=[])
    env.SetDefault(LIBS=[])
    env.SetDefault(LINKCOMSTR='')
    env.SetDefault(SHCXXCOMSTR='')
    env.SetDefault(SHLINKCOMSTR='')



DisallowSubst()

env.AppendUnique(CCFLAGS=['-fPIC'])
env.AppendUnique(LINKFLAGS=['-fPIC'])
env.AppendUnique(CPPPATH=['.'])
optimization=cfg.get('General','optimization').strip().split()
if optimization:
    env.AppendUnique(CCFLAGS = optimization )


# These are the boost libraries needed by the code.
boost_libs  = ['boost_system','boost_chrono','boost_random',
    'boost_program_options']


conf = Configure(env, custom_tests = {
     'CheckBoost' : SConsCheck.CheckBoost(boost_libs),
     'CheckCPP11' : SConsCheck.CheckCPP11(),
    })

if False:
    svnversion = subprocess.check_output(['svn','info','--xml'])
    f=open('raster_version.hpp','w')
    f.write('''
    #define RASTER_STATS_VERSION R"(%s)"
    ''' % (svnversion.replace(os.linesep,''),))
    f.close()
    
if GetOption('echo'):
    conf.logstream = sys.stdout

if not conf.CheckCXX():
    logger.debug('CXX %s' % env['CXX'])
    logger.debug('CXXCOM %s' % env['CXXCOM'])
    logger.error('The compiler isn\'t compiling.')
    Exit(1)

failure_cnt=0

if not conf.CheckCPP11():
    logger.error('Compiler does not support c++0x standard.')
    failure_cnt+=1

if not conf.CheckBoost():
    logger.error('Boost not found')
    failure_cnt+=1
    
if failure_cnt:
    Exit(2)

env = conf.Finish()


# Testing build environment
test_env = env.Clone()
# Use dynamic library variant of the unit testing framework.
test_env.Append(CCFLAGS = ['-DBOOST_TEST_DYN_LINK'])


# Now begin building.
gen = env.Program(target='gen',source=['main.cpp'])

cpp_target=Alias(gen)
Default(cpp_target)

cfg.write('used.cfg')
logger.info('End of first pass of SConstruct\n\n\n')
