# Run in shell with: sed -n -f <this-sed-script> <unit-test-log-file>
# Saves output for a test case until it can determine whether the test
# succeeded or failed. Discards output for successful test cases, prints
# output for failed test cases.

:in_testcase                # loop entry point
/^Suceeded\./ b             # test case succeeded; discard output & exit loop
/^!!! FAILED !!!$/ {x;p;b}  # test case failed; print output & exit loop
/^Executing test / h        # if new test case found, start saving output
/^Executing test /! H       # else keep saving output
n                           # grab the next line of output
b in_testcase               # branch to the top of the loop

