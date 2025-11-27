# Create Self-Signed Code Signing Certificate
# Usage: .\create-cert.ps1

$certName = "DotenvLibrary"
$certPassword = "dotenv123"
$pfxFile = "certificate.pfx"

Write-Host "Creating self-signed code signing certificate..." -ForegroundColor Green

# Create the certificate
$cert = New-SelfSignedCertificate `
    -Type CodeSigningCert `
    -Subject "CN=$certName" `
    -KeyAlgorithm RSA `
    -KeyLength 2048 `
    -CertStoreLocation "Cert:\CurrentUser\My" `
    -NotAfter (Get-Date).AddYears(5)

if ($null -eq $cert) {
    Write-Host "Error: Failed to create certificate" -ForegroundColor Red
    exit 1
}

Write-Host "Certificate created with thumbprint: $($cert.Thumbprint)" -ForegroundColor Cyan

# Export to PFX file
$password = ConvertTo-SecureString -String $certPassword -Force -AsPlainText
Export-PfxCertificate -Cert $cert -FilePath $pfxFile -Password $password | Out-Null

Write-Host "Certificate exported to: $pfxFile" -ForegroundColor Green
Write-Host "Password: $certPassword" -ForegroundColor Yellow
Write-Host ""
Write-Host "WARNING: This is a self-signed certificate for TESTING ONLY." -ForegroundColor Red
Write-Host "It will trigger security warnings when the DLL is used." -ForegroundColor Red
Write-Host ""
Write-Host "To sign the DLL, run:" -ForegroundColor Cyan
Write-Host "  make sign" -ForegroundColor White
