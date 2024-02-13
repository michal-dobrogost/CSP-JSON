#! /usr/bin/env nix-shell
#! nix-shell -i bash --pure build.nix
#
# The above shebang sets up the necessary packages to run this script without
# installing the packages globally on your computer. It's probably just CMake
# but you can check by reading the scripts/setup.nix file.
#
# If you have all the necessary tools already installed by a non-nix mechanism
# you can change to `#! /usr/bin/env bash` and run these scripts that way.
#

set -ex

ctest --test-dir build -V
