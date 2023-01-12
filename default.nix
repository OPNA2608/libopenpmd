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

  src = lib.cleanSource ./.;

  postUnpack = ''
    rm -rf $sourceRoot/build
  '';

  nativeBuildInputs = [ cmake toilet valgrind ];

  cmakeBuildType = "Debug";

  cmakeFlags = [
    # "-DUSE_LOWMEM=ON"
  ];

  doCheck = true;

  checkPhase = ''
    print() {
      toilet -f wideterm -F border $*
    }
    run_test() {
      valgrind --leak-check=full --track-origins=yes -s $* 2>&1 | tee log
      grep -q 'ERROR SUMMARY: 0 errors' log || exit 1
    }
    compare_two_hashes() {
      hash1="$(sha256sum "$1")"
      hash2="$(sha256sum "$2")"

      echo $hash1
      echo $hash2
      if [[ "$(echo "$hash1" | cut -d' ' -f1)" != "$(echo "$hash2" | cut -d' ' -f1)" ]]; then
        echo "Hashes don't match!"
        exit 1
      fi
    }

    print LOADTEST
    run_test ./tools/p86test ../examples/P86/RC1.P86 LOADTEST.P86

    print COMPARING ORIGINAL AGAINST EXPORT TEST
    compare_two_hashes ../examples/P86/RC1.P86 LOADTEST.P86

    print P86EXPORT
    run_test ./tools/p86exp ../examples/P86/RC1.P86 'EXP%03u.RAW'

    print P86IMPORT
    run_test ./tools/p86imp 'EXP%03u.RAW' IMEXTEST.P86

    # print COMPARING HASHES
    compare_two_hashes ../examples/P86/RC1.P86 IMEXTEST.P86
  '';
}

  ) { };
}
