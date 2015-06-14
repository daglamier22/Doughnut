import qbs

CppApplication {
    type: "application" // To suppress bundle generation on Mac
    consoleApplication: true
    files: [
        "README.md",
        "datafiles/fonts/DejaVuSans-10.font",
        "datafiles/fonts/DejaVuSans-12.font",
        "datafiles/imagesets/TaharezLook.imageset",
        "datafiles/imagesets/TaharezLook.png",
        "datafiles/looknfeel/TaharezLook.looknfeel",
        "datafiles/materials/BluePill.material",
        "datafiles/materials/RedPill.material",
        "datafiles/meshes/Pill.mesh",
        "datafiles/schemes/TaharezLook.scheme",
        "src/game.cpp",
        "src/game.h",
        "src/main.cpp",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }

    cpp.dynamicLibraries: ["OgreMain", "boost_system", "CEGUIBase", "CEGUIOgreRenderer", "OIS", "BulletDynamics", "BulletCollision", "LinearMath"]
    cpp.cxxLanguageVersion: "c++11"
    cpp.warningLevel: "default"
    cpp.cppFlags: "-Wno-deprecated-declarations"
    cpp.includePaths: ["/usr/include/cegui-0", "/usr/include/bullet"]
    cpp.defines: ["OGRE_PLUGINDIR=\"/usr/lib/OGRE\""]
}
