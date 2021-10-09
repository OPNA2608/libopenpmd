let
  pkgs = import <nixpkgs> { };
in {
  default = pkgs.callPackage (

{ stdenv
, lib
, cmake
}:

stdenv.mkDerivation rec {
  pname = "libopenpmd";
  version = "0.0.0-local";

  src = ./.;

  postUnpack = ''
    rm -rf .git build/
  '';

  nativeBuildInputs = [ cmake ];

  # stub until install step is implemented
  installPhase = ''
    echo "##### LOADTEST #####"
    ./tools/loadtest
    echo "##### P86EXTRACT #####"
    ./tools/p86extract test.P86
    echo "##### P86CREATE #####"
    ./tools/p86create TEST.***.RAW

    echo "##### COMPARING HASHES #####"
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
