with import <nixpkgs> { };

with pkgs;

mkShell {
  buildInputs = [
    cmake
    dosbox
    open-watcom-v2
  ];

  shellHook = ''
    export SDL_VIDEODRIVER=dummy
  '';
}
