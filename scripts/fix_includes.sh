#!/bin/bash

find modules unit_tests -type f \( -name "*.h" -o -name "*.cpp" \) \
  -exec sed -i 's|^#include\s*"\([^"]*\)"|#include <\1>|' {} +
