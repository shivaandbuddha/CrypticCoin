package=openssl
$(package)_version=1.0.2o
$(package)_sha256_hash=ec3f5c9714ba0fd45cb4e087301eb1336c317e0d20b575a125050470e8089e4d
$(package)_download_path=https://www.openssl.org/source
$(package)_file_name=$(package)-$($(package)_version).tar.gz

define $(package)_set_vars
  $(package)_config_opts=--prefix=$(host_prefix) --openssldir=$(host_prefix)/etc/openssl
  $(package)_config_env=AR="$($(package)_ar)" RANLIB="$($(package)_ranlib)" CC="$($(package)_cc)"
  $(package)_config_opts+=no-shared
  $(package)_config_opts+=no-unit-test
  $(package)_config_opts+=$($(package)_cflags) $($(package)_cppflags)
  $(package)_config_opts+=-DPURIFY
  $(package)_config_opts_linux=-fPIC -Wa,--noexecstack
  $(package)_config_opts_x86_64_linux=linux-x86_64
  $(package)_config_opts_i686_linux=linux-generic32
  $(package)_config_opts_arm_linux=linux-generic32
  $(package)_config_opts_aarch64_linux=linux-generic64
  $(package)_config_opts_mipsel_linux=linux-generic32
  $(package)_config_opts_mips_linux=linux-generic32
  $(package)_config_opts_powerpc_linux=linux-generic32
  $(package)_config_opts_x86_64_darwin=darwin64-x86_64-cc
  $(package)_config_opts_x86_64_mingw32=mingw64
  $(package)_config_opts_i686_mingw32=mingw
endef

define $(package)_preprocess_cmds
  sed -i.old "/define DATE/d" util/mkbuildinf.pl && \
  sed -i.old "s|\"engines\", \"apps\", \"test\"|\"engines\"|" Configure
endef

define $(package)_config_cmds
  ./Configure $($(package)_config_opts)
endef

define $(package)_build_cmds
  $(MAKE) -j1 build_libs libcrypto.pc libssl.pc openssl.pc
endef

define $(package)_stage_cmds
  $(MAKE) INSTALL_PREFIX=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
  rm -rf share bin etc
endef
