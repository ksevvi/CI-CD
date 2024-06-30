#!/bin/bash
# cd integration_tests/

RESULT=$(bash ./cat_test.sh | grep "FAIL: 0")
if [ -z "$RESULT" ]; then
    echo 'Integration tests failed.'
    exit 1
fi

RESULT=$(bash ./grep_test.sh | grep "FAIL: 0")
if [ -z "$RESULT" ]; then
    echo 'Integration tests failed.'
    exit 1
fi

echo 'Integration tests are successful.'
exit 0