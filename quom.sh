#!/bin/bash

if !(hash quom &>/dev/null;) then
    pip install --user quom
fi

quom src/json_parser.cpp header_only/jp.hpp \
    --include_directory include \
    --source_directory src

