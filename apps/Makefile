# Apps directory Makefile
# Delegates to individual app directories

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
	$(MAKE) -C $@ all

# Clean specified app
clean-%:
	@if [ -d $* ]; then \
		echo "Cleaning $*..."; \
		$(MAKE) -C $* clean; \
	else \
		echo "App $* not found"; \
	fi

# Clean all apps
clean-apps:
	@for app in $(APPS); do \
		if [ -d $$app ]; then \
			echo "Cleaning $$app..."; \
			$(MAKE) -C $$app clean; \
		fi; \
	done

# Clean everything
clean: clean-apps
	@echo "Cleaned all apps"