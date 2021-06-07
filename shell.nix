with import <nixpkgs> { };

with pkgs;

let
  vimName = "vim-shell";
  vimShell = vim_configurable.customize {
    name = vimName;
    vimrcConfig = {
      customRC = ''
        set nocompatible
        set history=1024
        if has ("syntax")
          syntax on
        endif
        set ruler
        set mouse-=a
        set number
        set laststatus=2
        set noshowmode
      '';
      packages.myVimPackages = with vimPlugins; {
        start = [
          vim-nix
          editorconfig-vim
          vim-indent-guides
          vim-better-whitespace
          lightline-vim
        ];
      };
    };
  };
in
mkShell {
  buildInputs = [
    cmake
    gcc
    vimShell
    editorconfig-checker
  ];

  shellHook = ''
    export EDITOR="${vimShell}/bin/${vimName}"
    echo "shell.nix-configured vim is available under: ${vimName}"
    echo "EDITOR has been set to shell.nix-configured vim."
  '';
}
