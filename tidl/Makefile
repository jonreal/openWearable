# tidl/Makefile — standalone build of the openWearable fleet-free C7x TIDL firmware.
#
# This is the WHOLE build. It does NOT use vision_apps' fleet build (no concerto, no mcu2_0/
# c6x_x, and crucially NO R5F/C66/A72 toolchains). It compiles, with cl7x only:
#   - our 4 firmware sources (src/) + 2 single-file SDK helpers (ti_mem_manager.c, ipc_trace.c)
#   - the exact C71/FreeRTOS app_utils sources concerto would pick (-> a small archive)
# and links them against the SDK's PREBUILT libs (CSL, udma, dmautils, sciclient, osal, FreeRTOS
# kernel, mmalib, TIDL) + our two linker scripts. Result: the same firmware concerto produces.
#
# USAGE (x86_64 Linux host with the TI PSDK + C7000 CGT installed):
#     make -C tidl/ PSDK=/path/to/ti-processor-sdk-rtos-j721e-evm-10_01_00_04
#   or edit the PSDK/CGT defaults below, then:  make -C tidl/
# Output: tidl/firmware/vx_app_rtos_linux_c7x_1.out  (stripped; scp to board, symlink j7-c71_0-fw).

# ── EDIT THESE (or pass on the command line / environment) ──────────────────────────────────────
PSDK ?= $(HOME)/psdk-rtos/ti-processor-sdk-rtos-j721e-evm-10_01_00_04
CGT  ?= $(HOME)/ti/ti-cgt-c7000_4.1.0.LTS

# ── derived ─────────────────────────────────────────────────────────────────────────────────────
HERE   := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
SRC    := $(HERE)/src
BUILD  := $(HERE)/build
LNKDIR := $(HERE)/linker
OUT    := $(HERE)/firmware/vx_app_rtos_linux_c7x_1.out
CL7X   := $(CGT)/bin/cl7x
AR7X   := $(CGT)/bin/ar7x
STRIP  := $(CGT)/bin/strip7x

TIDL   := $(PSDK)/c7x-mma-tidl
PDK    := $(PSDK)/pdk_jacinto_10_01_00_25/packages
MMALIB := $(PSDK)/mmalib_10_01_00_05
AU     := $(PSDK)/app_utils

# ── compile flags (verbatim from the concerto build) ────────────────────────────────────────────
DEFS := -DSOC_J721E -DC7X_FAMILY -DCPU_c7x_1 -DFREERTOS -DJ721E -DC71=C71 \
        -DTARGET_NUM_CORES=1 -DTARGET_ARCH=64 -DARCH_64 -DDSP -DTARGET_BUILD=1 -DNDEBUG
CFLAGS := --abi=eabi --gcc --gen_func_subsections --opt_level=3 --gen_opt_info=2 \
          --silicon_version=7100 --silicon_errata_i2117 --advice:performance=none \
          --emit_warnings_as_errors --diag_suppress=10063

IDIRS := \
  -I"$(SRC)" \
  -I"$(PSDK)/tiovx/include" -I"$(PSDK)/tiovx/kernels/include" -I"$(PSDK)/tiovx/utils/include" \
  -I"$(PSDK)/tiovx/kernels_j7/include" \
  -I"$(TIDL)/arm-tidl/tiovx_kernels/include" -I"$(TIDL)/arm-tidl/rt/inc" \
  -I"$(TIDL)/ti_dl/inc" -I"$(TIDL)/ti_dl/custom" -I"$(TIDL)/arm-tidl/rt/test/src" \
  -I"$(PSDK)/ivision" -I"$(CGT)/include" \
  -I"$(PDK)" -I"$(PDK)/ti/osal" -I"$(PDK)/ti/drv" -I"$(PDK)/ti/drv/udma" -I"$(PDK)/ti/csl/arch/c7x" \
  -I"$(PDK)/ti/kernel/freertos/FreeRTOS-LTS/FreeRTOS-Kernel/include" \
  -I"$(PDK)/ti/kernel/freertos/config/j721e/c7x" \
  -I"$(PDK)/ti/kernel/freertos/portable/TI_CGT/c7x" \
  -I"$(AU)" \
  -I"$(PSDK)/vision_apps" \
  -I"$(PSDK)/vision_apps/platform/j721e/rtos" \
  -I"$(PSDK)/vision_apps/platform/j721e/rtos/common" \
  -I"$(PSDK)/vision_apps/platform/j721e/rtos/common_linux" \
  -I"$(PSDK)/vision_apps/platform/j721e/rtos/c7x_1"

# ── firmware objects: our 3 vendored sources + 2 single-file SDK helpers ─────────────────────────
OBJS := $(addprefix $(BUILD)/, main.obj ow_c7x_tidl.obj tidl_rt.obj ti_mem_manager.obj ipc_trace.obj)
vpath %.c $(SRC) $(TIDL)/arm-tidl/rt/test/src $(PSDK)/vision_apps/platform/j721e/rtos

# ── app_utils: the exact C71/FreeRTOS source set the .map proves is linked -> a tiny archive ─────
# (app_utils source is compiled with -DBUILD_C7X, which its own build adds; our firmware sources
#  get it transitively via app.h so we keep their flags verbatim.)
AU_C := \
  console_io/src/app_log_writer.c console_io/src/app_log_rtos.c \
  misc/src/app_c7x_init.c misc/src/app_utils_task_init.c \
  perf_stats/src/app_perf_stats_freertos.c \
  rtos/src/app_rtos_pdk.c sciclient/src/app_sciclient.c timer/src/app_timer_rtos.c \
  udma/src/app_udma.c
AU_DEFS := $(DEFS) -DBUILD_C7X
AULIB   := $(BUILD)/libow_apputils.a
AU_OBJS := $(addprefix $(BUILD)/au/, $(notdir $(AU_C:.c=.obj)))
vpath %.c $(addprefix $(AU)/utils/, $(sort $(dir $(AU_C))))

# ── link: trimmed to the libs the .map proves are referenced (no fleet vx_*/ptk_* libs) ──────────
SEARCH := \
  --search_path="$(PDK)/ti/csl/lib/j721e/c7x/release" \
  --search_path="$(PDK)/ti/osal/lib/freertos/j721e/c7x/release" \
  --search_path="$(PDK)/ti/drv/udma/lib/j721e/c7x_1/release" \
  --search_path="$(PDK)/ti/drv/sciclient/lib/j721e/c7x_1/release" \
  --search_path="$(PDK)/ti/drv/ipc/lib/j721e/c7x_1/release" \
  --search_path="$(PDK)/ti/kernel/lib/j721e/c7x_1/release" \
  --search_path="$(MMALIB)/lib/C7100/release" \
  --search_path="$(TIDL)/ti_dl/lib/J721E/dsp/algo/release" \
  --search_path="$(CGT)/lib"
LIBS := \
  --library=common_C7100.lib --library=mmalib_C7100.lib --library=mmalib_cn_C7100.lib \
  --library=tidl_algo.lib --library=tidl_priv_algo.lib --library=tidl_obj_algo.lib \
  --library=tidl_custom.lib \
  --library=ti.osal.ae71 --library=ipc.ae71 --library=dmautils.ae71 --library=sciclient.ae71 \
  --library=udma.ae71 --library=ti.kernel.freertos.ae71 --library=ti.csl.ae71 \
  --library=ti.csl.init.ae71 --library=libc.a
LNKCMDS := "$(LNKDIR)/j721e_linker_freertos.cmd" "$(LNKDIR)/linker_mem_map.cmd"
LFLAGS  := -z --warn_sections --reread_libs --rom_model

# ── rules ───────────────────────────────────────────────────────────────────────────────────────
.PHONY: all clean
all: $(OUT)

$(BUILD) $(BUILD)/au $(dir $(OUT)):
	@mkdir -p $@

$(BUILD)/%.obj: %.c | $(BUILD)
	@echo "[C7X] cc  $(notdir $<)"
	@$(CL7X) $(CFLAGS) $(DEFS) $(IDIRS) -fr="$(BUILD)/" -fs="$(BUILD)/" -ft="$(BUILD)/" -eo=.obj -fc="$<"

$(BUILD)/au/%.obj: %.c | $(BUILD)/au
	@echo "[C7X] cc  app_utils/$(notdir $<)"
	@$(CL7X) $(CFLAGS) $(AU_DEFS) $(IDIRS) -fr="$(BUILD)/au/" -fs="$(BUILD)/au/" -ft="$(BUILD)/au/" -eo=.obj -fc="$<"

$(AULIB): $(AU_OBJS)
	@echo "[C7X] ar  $(notdir $@)"
	@rm -f $@
	@$(AR7X) r $@ $(AU_OBJS)

$(OUT): $(OBJS) $(AULIB) | $(dir $(OUT))
	@echo "[C7X] link $(notdir $@)"
	@$(CL7X) $(CFLAGS) $(LFLAGS) $(LNKCMDS) $(OBJS) $(AULIB) $(SEARCH) $(LIBS) \
	    --output_file="$@" --map_file="$(BUILD)/vx_app_rtos_linux_c7x_1.map"
	@$(STRIP) -p "$@"
	@echo "Done -> $@   (scp to board /lib/firmware, symlink j7-c71_0-fw, reboot)"

clean:
	rm -rf $(BUILD)
