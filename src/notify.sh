#!/bin/bash

TELEGRAM_API_TOKEN="7328414157:AAGfio9LhEmqBK8JDmVqVjud7crxbotxAJs"
CHAT_ID="1341204637"

URL="https://api.telegram.org/bot$TELEGRAM_API_TOKEN/sendMessage"
TEXT="Deploy status: $1%0AProject name: $CI_PROJECT_NAME%0AURL: $CI_PROJECT_URL/pipelines/$CI_PIPELINE_ID/"

curl $URL?"chat_id=$CHAT_ID&disable_web_page_preview=1&text=$TEXT" > /dev/null
