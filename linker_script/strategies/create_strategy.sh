#!/usr/bin/env sh

STRATEGY_NAME=$1
TEMPLATE_HEADER_NAME="template_strategy.h"
TEMPLATE_SOURCE_NAME="template_strategy.c++"

if [ "${STRATEGY_NAME}" == "" ]; then
  echo "Please provide a CamelCase strategy name as argument"
  exit 1
fi

echo "Creating ${STRATEGY_NAME} strategy"

# Convert CamelCase to snake_case
STRATEGY_SNAKE_CASE=`echo ${STRATEGY_NAME} | sed -E 's/([A-Z])/_\L\1/g'`
STRATEGY_SNAKE_CASE=${STRATEGY_SNAKE_CASE:1}

# Create the header define
STRATEGY_DEFINE="__`echo ${STRATEGY_SNAKE_CASE} | tr '[:lower:]' '[:upper:]'`__H"

# Create the filenames for the strategy
STRATEGY_HEADER_NAME="${STRATEGY_SNAKE_CASE}.h"
STRATEGY_SOURCE_NAME="${STRATEGY_SNAKE_CASE}.c++"

# Copy source files
cp ${TEMPLATE_HEADER_NAME} ${STRATEGY_HEADER_NAME}
cp ${TEMPLATE_SOURCE_NAME} ${STRATEGY_SOURCE_NAME}

# Replace define
sed -i "s/__TEMPLATE_STRATEGY__H/${STRATEGY_DEFINE}/g" ${STRATEGY_HEADER_NAME}

# Replace class name
sed -i "s/TemplateStrategy/${STRATEGY_NAME}/g" ${STRATEGY_HEADER_NAME}
sed -i "s/TemplateStrategy/${STRATEGY_NAME}/g" ${STRATEGY_SOURCE_NAME}

# Replace source file name
sed -i "s/template_strategy/${STRATEGY_SNAKE_CASE}/g" ${STRATEGY_HEADER_NAME}
sed -i "s/template_strategy/${STRATEGY_SNAKE_CASE}/g" ${STRATEGY_SOURCE_NAME}

