{
  description = "A Nix-flake-based Arduino development environment";

  inputs.nixpkgs.url =
    "https://flakehub.com/f/NixOS/nixpkgs/0"; # stable Nixpkgs

  outputs = { self, ... }@inputs:

    let
      supportedSystems =
        [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forEachSupportedSystem = f:
        inputs.nixpkgs.lib.genAttrs supportedSystems
        (system: f { pkgs = import inputs.nixpkgs { inherit system; }; });
    in {
      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell.override { } {
          packages = with pkgs; [
            # C++ tooling
            clang-tools
            clang
            cmake
            # Arduino
            arduino-cli
          ];
        };
      });
    };
}
