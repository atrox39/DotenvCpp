OUTDIR = bin
SRCDIR = src
CPP = g++
CPPFLAGS = -std=c++11 -O2 -Wall -Wextra -DBUILDING_DOTENV_DLL
SYS = WIN32
CERT_FILE ?= certificate.pfx
CERT_PASSWORD ?= dotenv123
TIMESTAMP_URL = http://timestamp.digicert.com

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp | $(OUTDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OUTDIR)/dotenv.dll: $(OUTDIR)/dotenv.o
	$(CPP) -shared -o $@ $< -Wl,--out-implib,$(OUTDIR)/libdotenv.a -static-libgcc -static-libstdc++

ifeq ($(SYS), WIN32)
sign: $(OUTDIR)/dotenv.dll
ifeq ($(wildcard $(CERT_FILE)),)
	@powershell -Command "Write-Host 'Error: Certificate file ''$(CERT_FILE)'' not found.' -ForegroundColor Red"
	@powershell -Command "Write-Host 'Run: powershell -ExecutionPolicy Bypass -File create-cert.ps1' -ForegroundColor Yellow"
	@exit 1
else
	@powershell -Command "&signtool sign /f '$(CERT_FILE)' /p '$(CERT_PASSWORD)' /t $(TIMESTAMP_URL) /fd SHA256 /d 'Dotenv C++ Library' $<"
endif
else
sign: $(OUTDIR)/dotenv.dll
	@echo "Code signing is only supported on Windows"
	@exit 1
endif

ifeq ($(SYS), WIN32)
$(OUTDIR):
	New-Item -ItemType Directory -Path $(OUTDIR) -Force | Out-Null
clean:
	Remove-Item -Recurse -Force $(OUTDIR) -ErrorAction SilentlyContinue
else
$(OUTDIR):
	mkdir -p $(OUTDIR)
clean:
	rm -rf $(OUTDIR)
endif
