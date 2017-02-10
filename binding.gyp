{
    "includes": [ "common.gypi" ],
    "targets": [
        {
          'target_name': 'action_before_build',
          'type': 'none',
          'hard_dependency': 1,
          'actions': [
            {
              'action_name': 'install-deps',
              'inputs': ['./install-deps.sh'],
              'outputs': ['./mason_packages'],
              'action': ['./install-deps.sh']
            }
          ]
        },
        {
            "target_name": "node_or_tools",
            'dependencies': [ 'action_before_build' ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "./mason_packages/.link/include",
                "./mason_packages/.link/include/or-tools"
            ],
            "link_settings": {
                "libraries": ["-lortools"],
                "library_dirs": [
                  "<(module_root_dir)/mason_packages/.link/lib"
                ]
            },
            "sources": [
                "src/main.cc",
                "src/tsp.cc",
                "src/vrp.cc",
            ],
            'ldflags': [
                '-Wl,-z,now'
            ],
            'xcode_settings': {
                'OTHER_LDFLAGS':[
                  '-Wl,-bind_at_load'
                ],
                'GCC_ENABLE_CPP_RTTI': 'YES',
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'MACOSX_DEPLOYMENT_TARGET':'10.8',
                'CLANG_CXX_LIBRARY': 'libc++',
                'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
                'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0'
            }
        },
    ],
}
