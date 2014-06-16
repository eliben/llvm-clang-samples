#!/bin/bash
set -eu

pss --cpp -f src_llvm src_clang | xargs clang-format -style=LLVM -i
