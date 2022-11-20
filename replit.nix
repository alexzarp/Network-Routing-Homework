{ pkgs }: {
	deps = [
		pkgs.openssh_with_kerberos
  pkgs.clang_12
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}