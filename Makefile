BUILD_DIR = build

configure:
	cmake -S . -B $(BUILD_DIR) -G "MinGW Makefiles"

all: configure
	cmake --build $(BUILD_DIR)

tests: configure
	cmake --build $(BUILD_DIR) --target lab_tests

run_tests: tests
	cd $(BUILD_DIR) &&  ctest --output-on-failure GTEST_BRIEF=1

ui: configure
	cmake --build $(BUILD_DIR) --target lab_ui

run_ui: ui
	$(BUILD_DIR)\ui\lab_ui.exe

clean:
	rmdir /S /Q $(BUILD_DIR)

rebuild: clean all

.PHONY: all tests run_tests ui run_ui clean rebuild