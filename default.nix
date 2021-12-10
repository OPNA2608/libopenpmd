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

  postUnpack = ''
    rm -rf $sourceRoot/{.git,build}/
  '';

  nativeBuildInputs = [ cmake toilet valgrind ];

  cmakeBuildType = "Debug";

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
    run_test ./tools/loadtest ../examples/P86/RC1.P86

    print COMPARING ORIGINAL AGAINST EXPORT TESTS
    compare_two_hashes ../examples/P86/RC1.P86 TEST_MEM.P86
    compare_two_hashes ../examples/P86/RC1.P86 TEST_FIL.P86

    print P86EXTRACT
    run_test ./tools/p86extract TEST_MEM.P86

    print P86CREATE
    run_test ./tools/p86create TEST.***.RAW

    # print COMPARING HASHES
    # compare_two_hashes TEST_MEM.P86 TEST.P86
  '';
}

  ) { };
}
