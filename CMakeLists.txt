cmake_minimum_required(VERSION 3.16)

project(imcap-kwin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)


find_package(
	Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED
		Core
		DBus
		Multimedia
)

add_executable(imcap-kwin main.cpp)

target_link_libraries(imcap-kwin
	Qt::DBus
	Qt::Multimedia
)

include(GNUInstallDirs)
install(TARGETS imcap-kwin RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY desktop/ DESTINATION /usr/share/applications FILES_MATCHING PATTERN "*.desktop")
