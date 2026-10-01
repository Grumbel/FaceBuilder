{
  description = "FaceBuilder — compose faces from parts (C++/Qt6)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = rec {
          default = facebuilder;
          facebuilder = pkgs.callPackage ./facebuilder.nix { };
        };

        apps.default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/facebuilder";
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ self.packages.${system}.facebuilder ];
          packages = with pkgs; [
            cmake
            qt6.qttools
          ];
        };
      }
    );
}
