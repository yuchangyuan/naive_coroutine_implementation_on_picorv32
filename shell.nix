let pkgs = import <nixpkgs> {}; in
pkgs.pkgsCross.riscv32-embedded.mkShell {}
