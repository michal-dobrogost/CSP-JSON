with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [ cmake tcl ]; # build time
  buildInputs = []; # runtime
}
