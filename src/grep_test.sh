#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
FILE=test.txt

cp C3_SimpleBashUtils/src/grep/s21_grep .

make s21_grep

RES1=$(./s21_grep 12 "$FILE")
((COUNTER++))

if [[ -f "$FILE" ]]; then
    RES2=$(grep 12 "$FILE")

    if [[ "$RES1" = "$RES2" ]]; then
        ((SUCCESS++))
        echo "SUCCESS"
    else
        ((FAIL++))
        echo "FAIL"
    fi
else
    echo "File not found"
    ((FAIL++))
fi

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"

if [[ $FAIL -eq 0 ]]; then
    exit 0
else
    exit 1
fi
