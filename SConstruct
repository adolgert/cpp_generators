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
import datetime
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
logger.debug('Running under python %s', sys.version)
file_cpu=int(cfg.get('General','num_jobs'))
num_cpu=int(os.environ.get('NUM_CPU',file_cpu))
SetOption('num_jobs', num_cpu)
logger.info('running with -j %d' % int(GetOption('num_jobs')))

AddOption('--echo', dest='echo', action='store_true',default=False,
          help='This echoes what scons runs as tests in order to debug failed builds.')
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
    env.AppendUnique(CCFLAGS   = optimization )
    env.AppendUnique(LINKFLAGS = optimization )


def add_if_mentioned(env_var, name):
    var=cfg.get(name[0], name[1])
    if var:
      var=var.strip()
    if var:
      var=var.split()
      logger.debug('Setting %s to %s' % (env_var, str(name)))
      var_dict={env_var : var}
      env.AppendUnique(*var_dict)

# What are the variables available?
# http://www.scons.org/doc/1.2.0/HTML/scons-user/a4774.html
add_if_mentioned('LIBS', ('General', 'libs_extra'))
add_if_mentioned('CCFLAGS', ('General', 'ccflags'))
add_if_mentioned('LINKFLAGS', ('General', 'linkflags'))


# These are the boost libraries needed by the code.
# The order matters. These get stacked as they are verified, and, for 
# instance, libboost_thread is needed by libbost_log.
# If you have problems, try reading "scons --echo".
boost_libs  = ['system','chrono','random','program_options','filesystem',
    'date_time', 'thread', 'log','log_setup']
boost_mt_libs=['boost_%s-mt' % x for x in boost_libs]
boost_st_libs=['boost_%s' % x for x in boost_libs]


# Each of these CheckXXX tests returns a callable object
# that adds relevant header directories and libraries to the paths.
conf = Configure(env, custom_tests = {
     'CheckBoost' : SConsCheck.CheckBoost(boost_st_libs),
     'CheckBoostMT' : SConsCheck.CheckBoost(boost_mt_libs),
     'CheckCPP11' : SConsCheck.CheckCPP11()
    })

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

if not conf.CheckBoostMT():
    failure_cnt+=1
    logger.error('Boost not found')
    #if not conf.CheckBoost():
    #    logger.error('Boost not found')
    #    failure_cnt+=1

#if not conf.CheckGeoTIFF():
#    logger.error('GeoTIFF not found')
#    failure_cnt+=1

if not conf.CheckLib('m',language='C++'):
    logger.error('No math library?!')
    failure_cnt+=1

if not conf.CheckLib('pthread',language='C++'):
    logger.error('We need pthread.')
    failure_cnt+=1

#if not conf.CheckLib('libhdf5',language='C'):
#    logger.error('Cannot load HDF5 library')
#    failure_cnt+=0

#if not conf.CheckLib('libhdf5_cpp',language='C'):
#    logger.error('Cannot load HDF5 library')
#    failure_cnt+=0

#if not conf.CheckLib('libhdf5_hl',language='C'):
#    logger.error('Cannot load HDF5 library')
#    failure_cnt+=0

#if not conf.CheckLib('libhdf5_hl_cpp',language='C'):
#    logger.error('Cannot load HDF5 library')
#    failure_cnt+=0


if cpp_compiler and os.path.split(cpp_compiler)[-1]=='icpc':
    conf.CheckLib('svml',language='C')
    conf.CheckLib('imf',language='C')
    conf.CheckLib('intlc',language='C')


#if conf.CheckTBB():
    # Not necessarily a failure.
#    tbb_exists=True
#else:
#    tbb_exists=False
    
if failure_cnt:
    cfg.write('used.cfg')
    Exit(2)

env = conf.Finish()


# Testing build environment
test_env = env.Clone()
# Use dynamic library variant of the unit testing framework.
test_env.Append(CCFLAGS = ['-DBOOST_TEST_DYN_LINK'])

test_conf = Configure(test_env, custom_tests = {})
if GetOption('echo'):
    test_conf.logstream = sys.stdout
test_conf.CheckLib('boost_unit_test_framework',language='C++')
test_env = test_conf.Finish()


#direct = env.Program(target='direct',source=['main.cpp'])
graph_test = env.Program(target='ittest',source=['iterator_test.cpp'])
#sketch = env.Program(target='sketch', source=['fsm_main.cpp'])

#cpp_target=Alias('cpp', cpp_includes)

all=Alias('all',[graph_test])
Default(all)

cfg.write('used.cfg')

# Keep this style of subprocess call for Python versions older than 2.7.
# git log HEAD^..HEAD | grep commit | cut -c8-
svn_ret=subprocess.Popen(['svn','info','--xml'], stdout=subprocess.PIPE)
svnversion = svn_ret.communicate()[0]
text_cfg=open('used.cfg','r').read().replace(os.linesep,'\t')
f=open('stochnet_version.hpp','w')
f.write('''
#define STOCHNET_VERSION R"(%s)"

#define STOCHNET_CFG R"(%s)"

#define STOCHNET_COMPILE_TIME R"(%s)"

''' % (svnversion.replace(os.linesep,''),text_cfg,
       datetime.datetime.now().isoformat()))
f.close()

logger.info('End of first pass of SConstruct')
