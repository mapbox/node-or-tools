{
    "variables": {
        "OR_TOOLS_INCLUDE_DIR%": "",
        "OR_TOOLS_LIBRARY_DIR%": ""
    },
    "targets": [
        {
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<@(OR_TOOLS_INCLUDE_DIR)"
            ],
            "link_settings": {
                "libraries": ["-lortools"],
                "library_dirs": ["<@(OR_TOOLS_LIBRARY_DIR)"]
            },
            "target_name": "node_or_tools",
            "sources": [
                "src/main.cc",
                "src/tsp.cc",
                "src/vrp.cc",
            ],
            "conditions": [
                ["OS != 'win'",{
                    "cflags_cc": ["-std=c++11 -Wall -Wextra -Wno-sign-compare -Wno-deprecated -Wno-ignored-qualifiers"],
                    "cflags_cc!": ["-fno-exceptions", "-fno-rtti"],
                    "libraries": ["-lortools"],
                }]
            ],
        },
    ],
}
