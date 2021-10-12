let
  pkgs = import <nixpkgs> { };
in {
  default = pkgs.callPackage (

{ stdenv
, lib
, cmake
, toilet
, valgrind
}:

stdenv.mkDerivation rec {
  pname = "libopenpmd";
  version = "0.0.0-local";

  src = ./.;

  preConfigure = ''
    rm -rf .git build/
  '';

  nativeBuildInputs = [ cmake toilet valgrind ];

  cmakeBuildType = "Debug";

  # stub until install step is implemented
  installPhase = ''
    print() {
      toilet -f wideterm -F border $*
    }
    run_test() {
      valgrind --leak-check=full --track-origins=yes -s $* 2>&1 | tee log
      grep -q 'ERROR SUMMARY: 0 errors' log || exit 1
    }

    print LOADTEST
    run_test ./tools/loadtest

    print P86EXTRACT
    run_test ./tools/p86extract test.P86

    print P86CREATE
    run_test ./tools/p86create TEST.***.RAW

    print COMPARING HASHES
    sha256sum test.P86 TEST.P86
    if [[ "$(sha256sum test.P86 | cut -d' ' -f1)" != "$(sha256sum TEST.P86 | cut -d' ' -f1)" ]]; then
      echo "Hashes don't match!"
      exit 1
    fi

    touch $out
  '';
}

  ) { };
}
