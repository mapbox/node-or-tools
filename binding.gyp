{
    'includes': [ 'common.gypi' ],
    'variables': {
      # Includes we don't want warnings for.
      'system_includes': [
        '-isystem <(module_root_dir)/<!(node -e \'require("nan")\')',
        '-isystem <(module_root_dir)/mason_packages/.link/include',
        '-isystem <(module_root_dir)/mason_packages/.link/include/or-tools'
      ]
    },
    'targets': [
        {
          'target_name': 'action_before_build',
          'type': 'none',
          'hard_dependency': 1,
          'actions': [
            {
              'action_name': 'install-deps',
              'inputs': ['./scripts/install-deps.sh'],
              'outputs': ['./mason_packages'],
              'action': ['./scripts/install-deps.sh']
            }
          ]
        },
        {
            'target_name': '<(module_name)',
            'product_dir': '<(module_path)',
            'dependencies': [ 'action_before_build' ],
            'link_settings': {
                'libraries': [
                  '<(module_root_dir)/mason_packages/.link/lib/libortools.a',
                  '<(module_root_dir)/mason_packages/.link/lib/libprotobuf.a',
                  '<(module_root_dir)/mason_packages/.link/lib/libgflags.a',
                  '-lz',
                ],
            },
            'sources': [
                'src/main.cc',
                'src/tsp.cc',
                'src/vrp.cc',
            ],
            'ldflags': [
                '-Wl,-z,now'
            ],
            'conditions': [
                ['OS == "linux"', {
                    'cflags': [
                        '<@(system_includes)'
                    ]
                }]
            ],
            'xcode_settings': {
                'OTHER_LDFLAGS':[
                  '-Wl,-bind_at_load'
                ],
                'OTHER_CPLUSPLUSFLAGS': [
                    '<@(system_includes)'
                ],
                'GCC_ENABLE_CPP_RTTI': 'YES',
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'MACOSX_DEPLOYMENT_TARGET':'10.8',
                'CLANG_CXX_LIBRARY': 'libc++',
                'CLANG_CXX_LANGUAGE_STANDARD':'c++14',
                'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0'
            }
        },
    ],
}
