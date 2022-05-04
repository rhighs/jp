#!/bin/bash

QUOM_COMMAND="quom"

if !(hash $QUOM_COMMAND &>/dev/null;) then
    pip install --user quom
fi

# quom path might not be registered in PATH env (It could happen when using mingw)
if !(hash $QUOM_COMMAND &>/dev/null;) then
    QUOM_COMMAND="python -m quom"
fi

$QUOM_COMMAND src/json_parser.cpp header_only/jp.hpp \
    --include_directory include \
    --source_directory src

