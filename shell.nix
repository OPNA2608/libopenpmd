{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  buildInputs = [
    cmake
    gcc
    editorconfig-checker
    valgrind
    gdb
  ];

  shellHook = ''
    unset CC
    unset CXX
    export VERBOSE=1
  '';
}
