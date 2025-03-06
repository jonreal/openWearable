# Top-level Makefile for OpenWearable

# List of all applications
APPS = template \
      reflex \
      load-cell-test \
      lut-example \
      sysID-dual-actuators \
      plotter-example \
      pam-cycle-test \
      udptest \
      test-pwm \
      test-maxon \
      pam-step-response \
      close-all-valves \
      maxon-freq-sweep \
      antagonist-freq-sweep \
      reflex-tracking \
      tare-encoder \
      thumbsup-triggered \
      thumbsup-gas \
      thumbsup-assist \
      pamtest \
      pam-starecase \
      haptic-tracking \
      haptic-eval \
      haptic-experiment \
      imutest \
      buttontest \
      test-blue-servos

.PHONY: all clean $(APPS) clean-apps prepare_lib clean-lib

all: prepare_lib $(APPS)

# Prepare library source files
prepare_lib:
	@echo "Preparing library source files..."
	$(MAKE) -C library

clean-lib:
	@echo "Cleaning library..."
	$(MAKE) -C library clean

# App targets (build each app)
$(APPS):
	@echo "Building $@..."
	$(MAKE) -C apps/$@ all

# Clean specified app
clean-%:
	@if [ -d apps/$* ]; then \
		echo "Cleaning $*..."; \
		$(MAKE) -C apps/$* clean; \
	else \
		echo "App $* not found"; \
	fi

# Clean all apps
clean-apps:
	@for app in $(APPS); do \
		if [ -d apps/$$app ]; then \
			echo "Cleaning $$app..."; \
			$(MAKE) -C apps/$$app clean; \
		fi; \
	done

# Clean everything
clean: clean-lib clean-apps
	@echo "Cleaned all OpenWearable components"

# Help target
help:
	@echo "OpenWearable Build System"
	@echo "--------------------------"
	@echo "Main targets:"
	@echo "  all         - Build library sources and all apps"
	@echo "  prepare_lib - Prepare library source files"
	@echo "  <app-name>  - Build specific app (e.g., 'make template')"
	@echo ""
	@echo "Clean targets:"
	@echo "  clean       - Clean everything"
	@echo "  clean-lib   - Clean only library"
	@echo "  clean-apps  - Clean all apps"
	@echo "  clean-<app> - Clean specific app (e.g., 'make clean-template')"
	@echo ""
	@echo "Available apps:"
	@for app in $(APPS); do \
		echo "  $$app"; \
	done