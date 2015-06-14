import qbs

CppApplication {
    type: "application" // To suppress bundle generation on Mac
    consoleApplication: true
    files: [
        "README.md",
        "src/game.cpp",
        "src/game.h",
        "src/main.cpp",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }

    cpp.dynamicLibraries: ["OgreMain", "boost_system", "CEGUIBase", "CEGUIOgreRenderer", "OIS"]
    cpp.cxxLanguageVersion: "c++11"
    cpp.warningLevel: "default"
    cpp.cppFlags: "-Wno-deprecated-declarations"
    cpp.includePaths: ["/usr/include/cegui-0", "/usr/include/OGRE"]
    cpp.defines: ["OGRE_PLUGINDIR=\"/usr/lib/OGRE\""]
}
