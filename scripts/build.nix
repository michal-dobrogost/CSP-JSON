with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [ cmake ]; # build time
  buildInputs = []; # runtime
}
