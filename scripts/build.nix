with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [
    cmake
    (pkgs.python3.withPackages (python-pkgs: [
      python-pkgs.pytest
      python-pkgs.pytest-xdist
    ]))
  ]; # build time
  buildInputs = []; # runtime
}
