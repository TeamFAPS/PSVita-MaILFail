cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_C_COMPILER "arm-vita-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-vita-eabi-g++")

project(mailfail-installer)

set(CMAKE_C_FLAGS "-Wl,-q -O3 -std=c99")

add_definitions(-DSQLITE_OS_OTHER=1 -DSQLITE_TEMP_STORE=3 -DSQLITE_THREADSAFE=0)

include_directories(
	sqlite-3.6.23.1
)

add_executable(${PROJECT_NAME}.elf
	src/main.c
	src/font.c
	src/graphics.c
	src/vita_sqlite.c
	src/utils.c
	src/vita_net.c
	src/installer_sqlite.c
	sqlite-3.6.23.1/sqlite3.c
)

target_link_libraries(${PROJECT_NAME}.elf
	-lSceDisplay_stub
	-lSceCtrl_stub
	-lSceHttp_stub
	-lSceSysmodule_stub
	-lSceNet_stub
	-lSceNetCtl_stub
	-lScePower_stub
)

add_custom_target(${PROJECT_NAME}.vpk ALL
	COMMAND vita-elf-create ${PROJECT_NAME}.elf ${PROJECT_NAME}.velf
	COMMAND vita-make-fself -a 0x2F00000000000005 ${PROJECT_NAME}.velf eboot.bin
	COMMAND vita-mksfoex -s TITLE_ID=MLFI00001 "${PROJECT_NAME}" param.sfo
	COMMAND vita-pack-vpk -s param.sfo -b eboot.bin -a ../res/icon0.png=sce_sys/icon0.png -a ../res/pic0.png=sce_sys/pic0.png -a ../res/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml -a ../res/livearea/contents/default_gate.png=sce_sys/livearea/contents/default_gate.png -a ../res/livearea/contents/Email_LA_bg.png=sce_sys/livearea/contents/Email_LA_bg.png -a ../resource/index.html=data/mailfail.html -a ../base/mail.db=data/mail.db ${PROJECT_NAME}.vpk
)

add_dependencies(${PROJECT_NAME}.vpk ${PROJECT_NAME}.elf)
