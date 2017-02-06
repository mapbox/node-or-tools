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
                "main.cc"
            ],
            "conditions": [
                ["OS != 'win'",{
                    "cflags_cc": ["-Wall -Wextra -pedantic -std=c++11"],
                    "cflags_cc!": ["-fno-exceptions", "-fno-rtti"],
                    "libraries": ["-lortools"],
                }]
            ],
        },
    ],
}
