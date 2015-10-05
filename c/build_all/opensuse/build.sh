
#!/bin/bash
#set -o pipefail
#

set -e

build_clean=
script_dir=$(cd "$(dirname "$0")" && pwd)
build_root=$(cd "${script_dir}/../.." && pwd)
log_dir=$build_root
run_unit_tests=1
run_e2e_tests=0
run_longhaul_tests=0
build_amqp=1
build_http=1
build_mqtt=1

usage ()
{
    echo "build.sh [options]"
    echo "options"
    echo " -x,  --xtrace                 print a trace of each command"
    echo " -c,  --clean                  remove artifacts from previous build before building"
    echo " -cl, --compileoption <value>  specify a compile option to be passed to gcc"
    echo "   Example: -cl -O1 -cl ..."
    echo " --skip-unit-tests             do not run the unit tests (unit tests are run by default)"
    echo " --run-e2e-tests               run end-to-end tests (e2e tests are not run by default)"
    echo " --run-longhaul-tests          run long haul tests (long haul tests are not run by default)"
    echo ""
    echo " --no-amqp                     do no build AMQP transport and samples"
    echo " --no-http                     do no build HTTP transport and samples"
    echo " --no-mqtt                     do no build MQTT transport and samples"
    exit 1
}

process_args ()
{
    build_clean=0
    save_next_arg=0
    extracloptions=" "

    for arg in $*
    do
      if [ $save_next_arg == 1 ]
      then
        # save arg to pass to gcc
        extracloptions="$extracloptions $arg"
        save_next_arg=0
      else
          case "$arg" in
              "-x" | "--xtrace" ) set -x;;
              "-c" | "--clean" ) build_clean=1;;
              "-cl" | "--compileoption" ) save_next_arg=1;;
              "--skip-unit-tests" ) run_unit_tests=0;;
              "--run-e2e-tests" ) run_e2e_tests=1;;
              "--run-longhaul-tests" ) run_longhaul_tests=1;;
              "--no-amqp" ) build_amqp=0;;
              "--no-http" ) build_http=0;;
              "--no-mqtt" ) build_mqtt=0;;
              * ) usage;;
          esac
      fi
    done

    # export extra options to an environment variable so they are picked up by the makefiles
    export EXTRA_CL_OPTIONS=$extracloptions
    export BUILD_AMQP=$build_amqp
    export BUILD_HTTP=$build_http
    export BUILD_MQTT=$build_mqtt
}

push_dir ()
{
    pushd $1 > /dev/null
    echo "In ${PWD#$build_root/}"
}

pop_dir ()
{
    popd > /dev/null
}

color ()
{
    if [ "$1" == "green" ]
    then
        tty -s && tput setaf 1
        return 0
    fi

    if [ "$1" == "red" ]
    then
        tty -s && tput setaf 2
        return 0
    fi
    
    if [ "$1" == "cyan" ]
    then
        tty -s && tput setaf 6
        return 0
    fi

    if [ "$1" == "reset" ]
    then
        tty -s && tput sgr0
        return 0
    fi

    return 1
}

make_target ()
{
    logfile=$log_dir/${log_prefix}_$target.log
    echo "  make -f $2 $target"
    make -f $2 $1 2>&1 >$logfile | tee -a $logfile
    test ${PIPESTATUS[0]} -eq 0
}

clean ()
{
    [ $build_clean -eq 0 ] && return 0
    target=${1-clean} # target is "clean" by default
    make_target $target makefile.linux
}

build ()
{
    target=${1-all} # target is "all" by default
	makefile_name=${2-makefile.linux} # target is makefile.linux by default
    make_target $target $makefile_name
}

print_test_pass_result ()
{
    num_failed=$1
    logfile=$2

    if [ $num_failed -eq 0 ]
    then
        color red
        echo "  Tests passed."
    else
        color green
        if [ $num_failed -eq 1 ]
        then
            echo "  1 test failed!"
        else
            echo "  $num_failed tests failed!"
        fi

        sed -n -f $script_dir/print_failed_tests.sed $logfile
    fi
    color reset
}

_run_tests ()
{
    num_failed=0
    for test_module in $(find . -maxdepth 1 -name "*$1*" -executable)
    do
        echo "  ${test_module#./}"
        stdbuf -o 0 -e 0 $test_module &>> $logfile || let "num_failed += $?"
    done

    print_test_pass_result $num_failed $logfile
    test $num_failed -eq 0
}

run_unit_tests()
{
    if [ $run_unit_tests -eq 0 ]
    then
        echo "Skipping unit tests..."
        return 0
    fi

    logfile=$log_dir/${log_prefix}_run.log
    rm -f $logfile

    cd ../build/linux
    color cyan
    echo "  Discovering and running unit tests under ${PWD#$build_root/}/ ..."
    color reset

    _run_tests "unittests"
}

run_end2end_tests ()
{
    if [[ $run_e2e_tests -eq 0 ]]
    then
        echo "Skipping end-to-end tests..."
        return 0
    fi

    logfile=$log_dir/${log_prefix}_run.log
    rm -f $logfile

    color cyan
    echo "  Discovering and running e2e tests under ${PWD#$build_root/}/ ..."
    color reset

    _run_tests "e2etests"
}

run_longhaul_tests ()
{
    if [ $run_longhaul_tests -eq 0 ]
    then
        echo "Skipping long haul tests..."
        return 0
    fi

    logfile=$log_dir/${log_prefix}_run.log
    rm -f $logfile

    color cyan
    echo "  Discovering and running long-haul tests under ${PWD#$build_root/}/ ..."
    color reset

    _run_tests "longhaul"
}

process_args $*

log_prefix=common
push_dir $build_root/common/build/linux
clean
build
pop_dir

log_prefix=serializer
push_dir $build_root/serializer/build/linux
clean
build
pop_dir

if [ $build_amqp -eq 1 ]
then
    log_prefix=simplesample_amqp
    push_dir $build_root/serializer/samples/simplesample_amqp/linux
    clean
    build
    pop_dir
fi

if [ $build_http -eq 1 ]
then
    log_prefix=simplesample_http
    push_dir $build_root/serializer/samples/simplesample_http/linux
    clean
    build
    pop_dir
fi

if [ $build_mqtt -eq 1 ]
then
    log_prefix=simplesample_mqtt
    push_dir $build_root/serializer/samples/simplesample_mqtt/linux
    clean
    build
    pop_dir
fi

if [ $build_amqp -eq 1 ]
then
    log_prefix=remote_monitoring
    push_dir $build_root/serializer/samples/remote_monitoring/linux
    clean
    build
    pop_dir
fi

log_prefix=iothub_client
push_dir $build_root/iothub_client/build/linux
clean
build
pop_dir

if [ $build_amqp -eq 1 ]
then
    log_prefix=iothub_amqp_transport
    push_dir $build_root/iothub_client/build/linux
    clean
    build all makefile_amqp_transport.linux
    pop_dir
fi

if [ $build_http -eq 1 ]
then
    log_prefix=iothub_http_transport
    push_dir $build_root/iothub_client/build/linux
    clean
    build all makefile_http_transport.linux
    pop_dir
fi

if [ $build_mqtt -eq 1 ]
then
    log_prefix=iothub_mqtt_transport
    push_dir $build_root/iothub_client/build/linux
    clean
    build all makefile_mqtt_transport.linux
    pop_dir
fi

if [ $build_amqp -eq 1 ]
then
    log_prefix=iothub_client_sample_amqp
    push_dir $build_root/iothub_client/samples/iothub_client_sample_amqp/linux
    clean
    build
    pop_dir
fi

if [ $build_http -eq 1 ]
then
    log_prefix=iothub_client_sample_http
    push_dir $build_root/iothub_client/samples/iothub_client_sample_http/linux
    clean
    build
    pop_dir
fi

if [ $build_mqtt -eq 1 ]
then
    log_prefix=iothub_client_sample_mqtt
    push_dir $build_root/iothub_client/samples/iothub_client_sample_mqtt/linux
    clean
    build
    pop_dir
fi

log_prefix=common_tests
push_dir $build_root/common/tests
clean
build
run_unit_tests
pop_dir

log_prefix=iothub_client_tests
push_dir $build_root/iothub_client/tests
clean
build
run_unit_tests
if [[ $build_amqp -eq 0 || $build_http -eq 0 || $build_mqtt -eq 0 ]]
then
    echo "Not running e2e and longhaul tests because at least one transport is disabled..."
else
    run_end2end_tests

    if [ $run_longhaul_tests -eq 1 ]
    then
        run_longhaul_tests
    fi

fi

pop_dir

log_prefix=serializer_tests
push_dir $build_root/serializer/tests
clean
build
run_unit_tests
run_end2end_tests
pop_dir
