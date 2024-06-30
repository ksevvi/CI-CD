#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""

cp C3_SimpleBashUtils/src/cat/s21_cat .


./s21_cat test.txt > test_s21_cat.log
cat test.txt > test_sys_cat.log
DIFF_RES="$(diff -s test_s21_cat.log test_sys_cat.log)"
(( COUNTER++ ))
if [ "$DIFF_RES" == "Files test_s21_cat.log and test_sys_cat.log are identical" ]; then
    (( SUCCESS++ ))
else
    (( FAIL++ ))
fi

rm test_s21_cat.log test_sys_cat.log

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"

if [ $FAIL -eq 0 ]; then
    exit 0
else
    exit 1
fi
