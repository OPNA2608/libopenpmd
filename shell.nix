{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  buildInputs = [
    cmake
    gcc
    editorconfig-checker
    valgrind
    gdb
  ] ++ lib.optionals (stdenv.hostPlatform.isx86) [
    open-watcom-v2
    dosbox
  ];

  shellHook = ''
    unset CC
    unset CXX
    export VERBOSE=1
  '';
}
