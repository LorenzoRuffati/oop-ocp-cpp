#!/bin/sh
bazel run @hedron_compile_commands//:refresh_all
mv compile_commands.json .vscode/compile_commands.json