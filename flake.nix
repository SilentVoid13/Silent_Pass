{
  description = "Silent_Pass";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    ...
  } @ inputs: let
  in
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {inherit system;};
        buildInputs = with pkgs; [
          clang
          openssl
          nss
          libxml2
          libsecret
          sqlite
        ];
        nativeBuildInputs = with pkgs; [
          pkg-config
          cmake
        ];
      in {
        devShells.default = pkgs.mkShell {
          inherit buildInputs;
          inherit nativeBuildInputs;
        };
      }
    );
}
