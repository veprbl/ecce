{
  description = "DD4hep Geometry Description of the ECCE Experiment";

  inputs = {
    eic-nix.url = "github:veprbl/eic-nix";
    nixpkgs.follows = "eic-nix/nixpkgs";
  };

  outputs = { self, nixpkgs, eic-nix }:
    let

      inherit (nixpkgs) lib;
      supportedSystems = [ "x86_64-linux" "x86_64-darwin" ];

    in
    {

      packages = lib.genAttrs supportedSystems (system:
        with import nixpkgs {
          inherit system;
        };
        {

          ecce = stdenv.mkDerivation {
            name = "ecce";

            src = self;

            postPatch = ''
              patchShebangs --host bin/make_detector_configuration

              substituteInPlace templates/ecce.xml.jinja2 \
                --replace '"ip6/' '"''${BEAMLINE_PATH}/ip6/'
            '' + lib.optionalString stdenv.isDarwin ''
              substituteInPlace templates/setup.sh.in \
                --replace LD_LIBRARY_PATH DYLD_LIBRARY_PATH
            '';

            nativeBuildInputs = [
              cmake
              python3
              python3.pkgs.jinja2
              python3.pkgs.pyyaml
            ];
            buildInputs = [
              eic-nix.packages.${system}.acts
              eic-nix.packages.${system}.dd4hep
              fmt
            ];

            cmakeFlags = [
              "-DCMAKE_CXX_STANDARD=17" # match dd4hep
            ];
          };

          default = self.packages.${system}.ecce;

        });

      # make "nix flake check" build all packages
      checks = self.packages;

    };
}
