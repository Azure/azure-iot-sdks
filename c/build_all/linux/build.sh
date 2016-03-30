#!/bin/bash
#set -o pipefail
#

set -e

script_dir=$(cd "$(dirname "$0")" && pwd)
build_root=$(cd "${script_dir}/../.." && pwd)
log_dir=$build_root
run_e2e_tests=ON
run_longhaul_tests=OFF
build_amqp=ON
build_http=ON
build_mqtt=ON
skip_unittests=OFF
build_python=OFF
run_valgrind=0

usage ()
{
    echo "build.sh [options]"
    echo "options"
    echo " -cl, --compileoption <value>  specify a compile option to be passed to gcc"
    echo "   Example: -cl -O1 -cl ..."
    echo " --skip-e2e-tests              skip the running of end-to-end tests (e2e tests are run by default)"
    echo " --skip-unittests              skip the running of unit tests (unit tests are run by default)"
	echo " --run-longhaul-tests          run long haul tests (long haul tests are not run by default)"
    echo ""
    echo " --no-amqp                     do no build AMQP transport and samples"
    echo " --no-http                     do no build HTTP transport and samples"
    echo " --no-mqtt                     do no build MQTT transport and samples"
    echo " --toolchain-file <file>       pass cmake a toolchain file for cross compiling"
    echo " --build-python                build Python C wrapper module (requires boost)"
    echo " -rv, --run_valgrind           will execute ctest with valgrind"
    exit 1
}

process_args ()
{
    save_next_arg=0
    extracloptions=" "
    toolchainfile=" "

    for arg in $*
    do      
      if [ $save_next_arg == 1 ]
      then
        # save arg to pass to gcc
        extracloptions="$arg $extracloptions"
        save_next_arg=0
      elif [ $save_next_arg == 2 ]
      then
        # save arg to pass as toolchain file
        toolchainfile="$arg"
		save_next_arg=0
      else
          case "$arg" in
              "-cl" | "--compileoption" ) save_next_arg=1;;
              "--skip-e2e-tests" ) run_e2e_tests=OFF;;
			  "--skip-unittests" ) skip_unittests=ON;;
              "--run-longhaul-tests" ) run_longhaul_tests=ON;;
              "--no-amqp" ) build_amqp=OFF;;
              "--no-http" ) build_http=OFF;;
              "--no-mqtt" ) build_mqtt=OFF;;
              "--build-python" ) build_python=ON;;
              "--toolchain-file" ) save_next_arg=2;;
              "-rv" | "--run_valgrind" ) run_valgrind=1;;
              * ) usage;;
          esac
      fi
    done

    if [ $toolchainfile <> " " ]
    then
      toolchainfile=$(readlink -f $toolchainfile)
      toolchainfile="-DCMAKE_TOOLCHAIN_FILE=$toolchainfile"
    fi
}

process_args $*

rm -r -f ~/cmake
mkdir ~/cmake
pushd ~/cmake
cmake $toolchainfile -Drun_valgrind:BOOL=$run_valgrind -DcompileOption_C:STRING="$extracloptions" -Drun_e2e_tests:BOOL=$run_e2e_tests -Drun_longhaul_tests=$run_longhaul_tests -Duse_amqp:BOOL=$build_amqp -Duse_http:BOOL=$build_http -Duse_mqtt:BOOL=$build_mqtt -Dskip_unittests:BOOL=$skip_unittests -Dbuild_python:BOOL=$build_python $build_root

CORES=$(grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)
make --jobs=$CORES

if [[ $run_valgrind == 1 ]] ;
then
    #use doctored openssl
    export LD_LIBRARY_PATH=/usr/local/ssl/lib
    ctest -j $CORES --output-on-failure
    export LD_LIBRARY_PATH=
else
    ctest -j $CORES -C "Debug" --output-on-failure
fi

popd