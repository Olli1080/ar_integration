vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Olli1080/CoordTrafoUtil
    REF v0.1.0
    SHA512 adaa0f17b93f8d49a21abae4b19a61221fe20c38397c6ca0a035a5939413ce97fcbc5e2c2c294086db34f637d3c3f6e93d2773b5ccdfe1cbb2b1a25037dc6d5b
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        eigen USE_EIGEN
        pcl   USE_PCL
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    PACKAGE_NAME base-transformation
    CONFIG_PATH share/base-transformation
)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug" "${CURRENT_PACKAGES_DIR}/share/base-transformation/debug")
