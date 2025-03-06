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

.PHONY: all clean $(APPS) clean-apps library clean-library

all: library $(APPS)

# Library targets
library:
	@echo "Building library..."
	$(MAKE) -C library all

clean-library:
	@echo "Cleaning library..."
	$(MAKE) -C library clean

# App targets (build only what's specified)
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
clean: clean-library clean-apps
	@echo "Cleaned all OpenWearable components"

# Help target
help:
	@echo "OpenWearable Build System"
	@echo "--------------------------"
	@echo "Main targets:"
	@echo "  all         - Build library and all apps"
	@echo "  library     - Build only the library"
	@echo "  <app-name>  - Build specific app (e.g., 'make template')"
	@echo ""
	@echo "Clean targets:"
	@echo "  clean       - Clean everything"
	@echo "  clean-library - Clean only library"
	@echo "  clean-apps  - Clean all apps"
	@echo "  clean-<app> - Clean specific app (e.g., 'make clean-template')"
	@echo ""
	@echo "Available apps:"
	@for app in $(APPS); do \
		echo "  $$app"; \
	done
