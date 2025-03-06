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

.PHONY: all clean $(APPS) clean-apps

all: $(APPS)

# App targets (build each app)
$(APPS):
	@echo "Building $@..."
	cd apps && $(MAKE) $@

# Clean specified app
clean-%:
	@echo "Cleaning $*..."
	cd apps && $(MAKE) clean-$*

# Clean all apps
clean-apps:
	@for app in $(APPS); do \
		echo "Cleaning $$app..."; \
		cd apps && $(MAKE) clean-$$app; \
	done

# Clean everything
clean: clean-apps
	@echo "Cleaned all OpenWearable components"

# Help target
help:
	@echo "OpenWearable Build System"
	@echo "--------------------------"
	@echo "Main targets:"
	@echo "  all         - Build all apps"
	@echo "  <app-name>  - Build specific app (e.g., 'make template')"
	@echo ""
	@echo "Clean targets:"
	@echo "  clean       - Clean everything"
	@echo "  clean-apps  - Clean all apps"
	@echo "  clean-<app> - Clean specific app (e.g., 'make clean-template')"
	@echo ""
	@echo "Available apps:"
	@for app in $(APPS); do \
		echo "  $$app"; \
	done