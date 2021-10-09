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
    touch $out
  '';
}

  ) { };
}
