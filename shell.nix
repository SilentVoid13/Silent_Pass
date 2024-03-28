{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
  # Set the name of the derivation
  name = "Silent_Pass";

  # Add executable packages to the nix-shell environment
  packages = with pkgs; [
    pkg-config
    cmake
    clang
    openssl
    nss
    libxml2
    libsecret
  ];
}
