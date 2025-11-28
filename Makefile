OUTDIR = bin
SRCDIR = src
TESTDIR = tests
CPP = g++
CPPFLAGS_COMMON = -std=c++17 -O2 -Wall -Wextra -DBUILDING_DOTENV_DLL -fPIC
CERT_FILE ?= certificate.pfx
CERT_PASSWORD ?= dotenv123
TIMESTAMP_URL = http://timestamp.digicert.com

# Detect operating system
ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    LIB_EXT = dll
    IMPORT_LIB = $(OUTDIR)/libdotenv.a
    SHARED_FLAGS = -shared -Wl,--out-implib,$(IMPORT_LIB)
    LINK_FLAGS = -static-libgcc -static-libstdc++
    MKDIR = if not exist $(OUTDIR) mkdir $(OUTDIR)
    RM = if exist $(OUTDIR) rmdir /s /q $(OUTDIR)
    EXE_EXT = .exe
    CPPFLAGS = $(CPPFLAGS_COMMON)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = linux
        LIB_EXT = so
        SHARED_FLAGS = -shared -Wl,-soname,libdotenv.so
        LINK_FLAGS = -lpthread
        CPPFLAGS = $(CPPFLAGS_COMMON) -fvisibility=hidden
    else ifeq ($(UNAME_S),Darwin)
        PLATFORM = macos
        LIB_EXT = dylib
        SHARED_FLAGS = -dynamiclib -install_name @rpath/libdotenv.dylib
        LINK_FLAGS = -lpthread
        CPPFLAGS = $(CPPFLAGS_COMMON) -fvisibility=hidden
    else
        PLATFORM = unknown
        LIB_EXT = so
        SHARED_FLAGS = -shared
        LINK_FLAGS =
        CPPFLAGS = $(CPPFLAGS_COMMON)
    endif
    MKDIR = mkdir -p $(OUTDIR)
    RM = rm -rf $(OUTDIR)
    EXE_EXT =
endif

LIB_NAME = libdotenv.$(LIB_EXT)
LIB_PATH = $(OUTDIR)/$(LIB_NAME)

# Default target
.PHONY: all
all: $(LIB_PATH)

# Create output directory
$(OUTDIR):
	@$(MKDIR)

# Compile object file
$(OUTDIR)/dotenv.o: $(SRCDIR)/dotenv.cpp $(SRCDIR)/dotenv.hpp | $(OUTDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

# Build shared library
$(LIB_PATH): $(OUTDIR)/dotenv.o
	$(CPP) $(SHARED_FLAGS) -o $@ $< $(LINK_FLAGS)

# Alias targets for convenience
.PHONY: lib dll so dylib
lib: $(LIB_PATH)
dll: $(LIB_PATH)
so: $(LIB_PATH)
dylib: $(LIB_PATH)

# Build test executable
$(OUTDIR)/test$(EXE_EXT): $(TESTDIR)/test_dotenv.cpp $(LIB_PATH) | $(OUTDIR)
	$(CPP) -std=c++17 -Wall -Wextra -I$(SRCDIR) $< -o $@ -L$(OUTDIR) -ldotenv -Wl,-rpath,'$$ORIGIN' $(LINK_FLAGS)

.PHONY: test
test: $(OUTDIR)/test$(EXE_EXT)
	@cd $(OUTDIR) && ./test$(EXE_EXT)

# Code signing (Windows only)
.PHONY: sign
ifeq ($(PLATFORM),windows)
sign: $(LIB_PATH)
ifeq ($(wildcard $(CERT_FILE)),)
	@echo "Error: Certificate file '$(CERT_FILE)' not found."
	@echo "Run: powershell -ExecutionPolicy Bypass -File create-cert.ps1"
	@exit 1
else
	signtool sign /f "$(CERT_FILE)" /p "$(CERT_PASSWORD)" /t $(TIMESTAMP_URL) /fd SHA256 /d "Dotenv C++ Library" $<
endif
else
sign:
	@echo "Code signing is only supported on Windows"
	@echo "On Linux/macOS, consider using gpg signing instead"
endif

# Clean build artifacts
.PHONY: clean
clean:
	@$(RM)

# Install target (Unix-like systems)
.PHONY: install
ifeq ($(PLATFORM),linux)
PREFIX ?= /usr/local
install: $(LIB_PATH)
	install -d $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include
	install -m 755 $(LIB_PATH) $(DESTDIR)$(PREFIX)/lib/
	install -m 644 $(SRCDIR)/dotenv.hpp $(DESTDIR)$(PREFIX)/include/
	ldconfig || true
else ifeq ($(PLATFORM),macos)
PREFIX ?= /usr/local
install: $(LIB_PATH)
	install -d $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include
	install -m 755 $(LIB_PATH) $(DESTDIR)$(PREFIX)/lib/
	install -m 644 $(SRCDIR)/dotenv.hpp $(DESTDIR)$(PREFIX)/include/
else
install:
	@echo "Use MSYS2/MinGW or Visual Studio for Windows installation"
endif

# Uninstall target (Unix-like systems)
.PHONY: uninstall
ifneq ($(PLATFORM),windows)
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libdotenv.*
	rm -f $(DESTDIR)$(PREFIX)/include/dotenv.hpp
endif

# Help target
.PHONY: help
help:
	@echo "Dotenv C++ Library Build System"
	@echo "================================"
	@echo "Platform: $(PLATFORM)"
	@echo "Library:  $(LIB_NAME)"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the shared library (default)"
	@echo "  lib      - Alias for building the library"
	@echo "  test     - Build and run tests"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install library and headers (Unix)"
	@echo "  uninstall- Remove installed files (Unix)"
	@echo "  sign     - Code sign the DLL (Windows only)"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Build variables:"
	@echo "  PREFIX=$(PREFIX) - Installation prefix"
	@echo "  CERT_FILE=$(CERT_FILE) - Certificate for signing"
	@echo ""
	@echo "Examples:"
	@echo "  make"
	@echo "  make test"
	@echo "  sudo make install PREFIX=/usr"
